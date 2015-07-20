///////////////////////////////////////////////////////////////////////////////
//
//  Module: WriteRegistry.cpp
//
//    Desc: Functions to write a registry hive to a file.
//
// Copyright (c) 2003 Grant McDorman
// This file is licensed using a BSD-type license:
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
///////////////////////////////////////////////////////////////////////////////

// Modified for p4win
//   1. UNICODE support
//   2. appends to file rather than overwriting
//   3. eliminated reliance on CRT

#include "stdafx.h"
#include <malloc.h>		// _alloca
#include "WriteRegistry.h"

static bool WriteRegValue(HANDLE hFile, const TCHAR *key_path, const TCHAR *name, 
						  int name_len, DWORD type, const unsigned char *data, 
						  DWORD data_len);
static bool WriteValuesAndSubkeys(const TCHAR *key_path, HKEY parent_key, 
								  const TCHAR *subkey, HANDLE hFile);


///////////////////////////////////////////////////////////////////////////////
// lstrrchr (avoid the C Runtime )
static TCHAR * lstrchr(LPCTSTR string, int ch)
{
	while (*string && *string != (TCHAR)ch)
		string++;

	if (*string == (TCHAR)ch)
		return((TCHAR *)string);
	return(NULL);
}
static int lstrcmpn (
        const TCHAR * first,
        const TCHAR * last,
        size_t count
        )
{
        if (!count)
                return(0);

        while (--count && *first && *first == *last)
        {
                first++;
                last++;
        }

#ifdef UNICODE
        return((int)(*first - *last));
#else
        return( *(unsigned char *)first - *(unsigned char *)last );
#endif
}

static void WriteFileString(HANDLE hFile, const TCHAR *string)
{
	DWORD written = 0;

	if (!WriteFile(hFile, string, lstrlen(string)*sizeof(TCHAR), &written, NULL) || 
		written != lstrlen(string)*sizeof(TCHAR)) 
	{
		TRACE(_T("ERROR - WriteFile failed\n"));
		throw false;
	}
}

bool WriteRegistryTreeToFile(const TCHAR *key, const TCHAR *filename)
{
	ASSERT(key && key[0] != 0);
	ASSERT(filename && filename[0] != 0);
	if (key == NULL || key[0] == 0)
		return false;
	if (filename == NULL || filename[0] == 0)
		return false;

	TCHAR *cp = lstrchr(key, _T('\\'));
	if (cp == NULL) 
		return false;

	int len = cp - key;
	HKEY hKey = NULL;

#define IS_PATH(id, short_id) \
	(lstrcmpn(key, _T(#id), len) == 0 || lstrcmpn(key, _T(#short_id), len) == 0) hKey = id

	if      IS_PATH(HKEY_CLASSES_ROOT, HKCR);
	else if IS_PATH(HKEY_CURRENT_USER, HKCU);
	else if IS_PATH(HKEY_LOCAL_MACHINE, HKLM);
	else if IS_PATH(HKEY_CURRENT_CONFIG, HKCC);
	else if IS_PATH(HKEY_USERS, HKU);
	else if IS_PATH(HKEY_PERFORMANCE_DATA, HKPD);
	else if IS_PATH(HKEY_DYN_DATA, HKDD);
	else 
	{
		return false;
	}
	return WriteRegistryTreeToFile(hKey, cp + 1, filename);
}

bool WriteRegistryTreeToFile(HKEY section, 
							 const TCHAR *subkey, 
							 const TCHAR *filename)
{
	ASSERT(section);
	ASSERT(subkey && subkey[0] != 0);
	ASSERT(filename && filename[0] != 0);
	if (section == NULL)
		return false;
	if (subkey == NULL || subkey[0] == 0)
		return false;
	if (filename == NULL || filename[0] == 0)
		return false;

    bool status = false;

    HANDLE hFile = ::CreateFile(filename,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                0);

    if (INVALID_HANDLE_VALUE != hFile) 
	{
		SetFilePointer(hFile, 0, 0, FILE_END);

		if(!GetFileSize(hFile, 0))
		{
#ifdef UNICODE
			WORD bom = 0xfeff;
			DWORD nBytesWritten;
			WriteFile(hFile, &bom, sizeof(bom), &nBytesWritten, 0);
#endif
			WriteFileString(hFile, _T("REGEDIT4\r\n"));
		}

		TCHAR * key_path = _T("UNKNOWN");

#define SET_PATH(id) (id == section) key_path = _T(#id)

		if      SET_PATH(HKEY_CLASSES_ROOT);
		else if SET_PATH(HKEY_CURRENT_USER);
		else if SET_PATH(HKEY_LOCAL_MACHINE);
		else if SET_PATH(HKEY_CURRENT_CONFIG);
		else if SET_PATH(HKEY_USERS);
		else if SET_PATH(HKEY_PERFORMANCE_DATA);
		else if SET_PATH(HKEY_DYN_DATA);

#undef SET_PATH

		try 
		{
			status = WriteValuesAndSubkeys(key_path, section, subkey, hFile);
		} 
		catch (...) 
		{
			status = false;
		}

//		hflush(hFile);
		CloseHandle(hFile);

		if (!status) 
		{
			DeleteFile(filename);
		}
	}
	else
	{
		TRACE(_T("failed to create %s\n"), filename);
	}

	return status;
}

static bool WriteValuesAndSubkeys(const TCHAR *key_path, 
								  HKEY parent_key, 
								  const TCHAR *subkey, 
								  HANDLE hFile)
{
	HKEY key = NULL;

	if (RegOpenKeyEx(parent_key, subkey, 0, KEY_READ, &key) != ERROR_SUCCESS) 
	{
		TRACE(_T("RegOpenKeyEx failed, key_path=%s  subkey=%s\n"), key_path, subkey);
		return false;
	}
	DWORD num_subkeys    = 0;
	DWORD max_subkey_len = 0;
	DWORD num_values     = 0;
	DWORD max_name_len   = 0;
	DWORD max_value_len  = 0;
	DWORD max_id_len     = 0;

	if (RegQueryInfoKey(key,
						NULL, // class
						NULL, // num_class
						NULL, // reserved
						&num_subkeys, 
						&max_subkey_len,
						NULL, // MaxClassLen
						&num_values, 
						&max_name_len, 
						&max_value_len, 
						NULL, 
						NULL) != ERROR_SUCCESS) 
	{
		TRACE(_T("RegQueryInfoKey failed, key=%s\n"), subkey);
		return false;
	}

	max_id_len = (max_name_len > max_subkey_len) ? max_name_len : max_subkey_len;
	TCHAR *this_path = reinterpret_cast<TCHAR *>
						(_alloca((lstrlen(key_path) + lstrlen(subkey) + 10)*sizeof(TCHAR)));

	// strcpy/strcat safe because of above alloca
	lstrcpy(this_path, key_path);
	lstrcat(this_path, _T("\\"));
	lstrcat(this_path, subkey);

	WriteFileString(hFile, _T("\r\n["));
	WriteFileString(hFile, this_path);
	WriteFileString(hFile, _T("]\r\n"));

	// enumerate values
	TCHAR *name = reinterpret_cast<TCHAR *>(_alloca(max_id_len*2 + 10));
	unsigned char *data = reinterpret_cast<unsigned char *>
							(_alloca(max_value_len*2 + 10));
	DWORD index = 0;
	bool status = true;

	for (index = 0; index < num_values && status; index++) 
	{
		DWORD name_len = max_id_len + 1;
		DWORD value_len = max_value_len + 1;
		DWORD type = 0;

		if (RegEnumValue(key, index, name, &name_len, NULL, &type, data, &value_len) == ERROR_SUCCESS) 
		{
			status = WriteRegValue(hFile, this_path, name, name_len, type, data, value_len);
		}
	}

	// enumerate subkeys
	for (index = 0; index < num_subkeys && status; index++) 
	{
		DWORD name_len = max_id_len + 1;
		if (RegEnumKeyEx(key, index, name, &name_len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) 
		{
			status = WriteValuesAndSubkeys(this_path, key, name, hFile);
		}
	}

	RegCloseKey(key);

	return status;
}

static bool WriteRegValue(HANDLE hFile, 
						  const TCHAR * /*key_path*/, 
						  const TCHAR *name, 
						  int /* name_len */, 
						  DWORD type, 
						  const unsigned char *data, 
						  DWORD data_len)
{
	WriteFileString(hFile, _T("\""));
	WriteFileString(hFile, name);

	TCHAR string_type[64];

	switch(type) 
	{
		case REG_DWORD:  // A 32-bit number.
			lstrcpyn(string_type, _T("\"=dword:"), sizeof(string_type)/sizeof(TCHAR));
			break;

		case REG_SZ: // A null terminated string.
			lstrcpyn(string_type, _T("\"=\""), sizeof(string_type)/sizeof(TCHAR));
			break;

		case REG_BINARY: // Binary data in any form.
			lstrcpyn(string_type, _T("\"=hex:"), sizeof(string_type)/sizeof(TCHAR));
			break;

		case REG_EXPAND_SZ:			// A null-terminated string that contains unexpanded references to environment variables (for example, "%PATH%"). It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions. To expand the environment variable references, use the ExpandEnvironmentStrings function.
		case REG_LINK:				// A Unicode symbolic link. Used internally; applications should not use this type.
		case REG_MULTI_SZ:			// An array of null-terminated strings, terminated by two null characters. 
		case REG_NONE:				// No defined value type.
		case REG_DWORD_BIG_ENDIAN:	// A 64-bit number in big-endian format.
		case REG_RESOURCE_LIST:		// A device-driver resource list.
		default:
			wsprintf(string_type, 
				_T("\"=hex(%x):"), type);
			break;
	}

	WriteFileString(hFile, string_type);

	if (type == REG_SZ || type == REG_EXPAND_SZ) 
	{
		// escape special characters; length includes trailing NUL
		// don't crash'n'burn if data_len is 0
		data_len /= sizeof(TCHAR);
		WORD *ctypes = reinterpret_cast<WORD *>(_alloca(data_len*sizeof(WORD)));
		LPCTSTR cdata = reinterpret_cast<LPCTSTR>(data);
		if(GetStringTypeEx(LOCALE_SYSTEM_DEFAULT, CT_CTYPE1, cdata, data_len, ctypes))
		{
			for (int i = 0; i < static_cast<int>(data_len) - 1; i++) 
			{
				if (cdata[i] == _T('\\') || cdata[i] == _T('"')) 
				{
					WriteFileString(hFile, _T("\\"));
				}
				if(ctypes[i] & C1_CNTRL)
				{
					_sntprintf(string_type, sizeof string_type, _T("\\%02x"), cdata[i]);
					WriteFileString(hFile, string_type);
				}
				else
				{
					DWORD written;
					if (!WriteFile(hFile, &cdata[i], sizeof(TCHAR), &written, NULL) || written != sizeof(TCHAR)) 
					{
						return false;
					}
				} 
			}
		}
		WriteFileString(hFile, _T("\""));
	} 
	else if (type == REG_DWORD) 
	{
		// write as hex, MSB first
		for (int i = static_cast<int>(data_len) - 1; i >= 0; i--) 
		{
			_sntprintf(string_type, sizeof string_type, _T("%02x"), data[i]);
			WriteFileString(hFile, string_type);
		}
	} 
	else 
	{
		// write as comma-separated hex values
		for (DWORD i = 0; i < data_len; i++) 
		{
			_sntprintf(string_type, sizeof string_type, _T("%s%02x"), 
				i > 0 ? _T(",") : _T(""), data[i]);
			WriteFileString(hFile, string_type);
			if (i > 0 && i % 16 == 0) 
			{
				WriteFileString(hFile, _T("\r\n"));
			}
		}
	}
	WriteFileString(hFile, _T("\r\n"));

	return true;
}


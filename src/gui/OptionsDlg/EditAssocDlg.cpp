//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// EditAssocDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "EditAssocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditAssocDlg dialog


CEditAssocDlg::CEditAssocDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditAssocDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditAssocDlg)
	m_Application = _T("");
	m_Extension = _T("");
	//}}AFX_DATA_INIT
}


void CEditAssocDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditAssocDlg)
	DDX_Text(pDX, IDC_APPLICATION, m_Application);
	DDV_MaxChars(pDX, m_Application, 255);
	DDX_Text(pDX, IDC_EXTENSION, m_Extension);
	DDV_MaxChars(pDX, m_Extension, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditAssocDlg, CDialog)
	//{{AFX_MSG_MAP(CEditAssocDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditAssocDlg message handlers

void CEditAssocDlg::OnOK() 
{
	if(!UpdateData(TRUE))
		return;

	CString error;

	m_Application.TrimLeft( );
	m_Extension.TrimLeft( );
	m_Application.TrimRight( );
	m_Extension.TrimRight( );

	if(!m_Application.GetLength())
		error= LoadStringResource(IDS_AN_APPLICATION_MUST_BE_SPECIFIED);
	else 
		ParseExtensions(error);
		
	if(error.GetLength())
		AfxMessageBox(error, MB_ICONSTOP);
	else
	{
		// We dont write any changes to registry unless ALL extensions 
		// appear to be valid
		UpdateRegistry();
		EndDialog(IDOK);
	}
}

void CEditAssocDlg::OnBrowse() 
{
	// Fire up a common dlg to find new file
	CFileDialog fDlg(TRUE, _T("exe"), NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_EDITOR_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 

	if(fDlg.DoModal() == IDOK)
		GetDlgItem(IDC_APPLICATION)->SetWindowText( fDlg.GetPathName() );
}

void CEditAssocDlg::ParseExtensions(CString &error)
{
	// Check for blatant errors
	if(m_Extension.GetLength()==0)
	{
		error=LoadStringResource(IDS_FILE_EXTENSION_MISSING);
		return;
	}

	if(m_Extension.FindOneOf(_T("\\/:*?|")) != -1)
	{
		error=LoadStringResource(IDS_FILE_EXTENSION_CONTAINS_ILLEGAL_CHARACTERS);
		return;
	}
	
	// Make a copy of the extensions, minus any spaces
	LPTSTR buf= new TCHAR[m_Extension.GetLength()+1];
	LPTSTR bufptr= buf;
	LPTSTR ptr= m_Extension.GetBuffer(m_Extension.GetLength());

	for(int i=m_Extension.GetLength(); i; i--)
	{
		if(*ptr != _T(' '))
		{
			*bufptr=*ptr;
			bufptr++;
		}
		ptr++;
	}
	*bufptr=_T('\0');
	
	//Split the extensions into a stringlist
	m_ExtensionList.RemoveAll();
	LPTSTR token= _tcstok(buf, _T(";\t"));
	while(token != NULL)
	{
		if(_tcschr(token, _T('.')) != token || _tcschr(token+1, _T('.')) != NULL)
		{
			error=LoadStringResource(IDS_INVALID_FILE_EXTENSION);
			error+=token;
			break;
		}

		// Don't write the '.' to the list, because a '.' is not a
		// valid first char for a registry value name
		m_ExtensionList.AddHead(token+1);
		token= _tcstok(NULL, _T(";\t"));
	}

	delete [] buf;
}

void CEditAssocDlg::UpdateRegistry()
{
	// Just run through the StringList
	POSITION pos= m_ExtensionList.GetHeadPosition();
	while(pos != NULL)
	{
		CString extension= m_ExtensionList.GetNext(pos);
		GET_P4REGPTR()->SetAssociation(extension, m_Application);	
	}
}

BOOL CEditAssocDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_EditMode)
	{
		SetWindowText(LoadStringResource(IDS_EDIT_FILE_APPLICATION_ASSOCIATION));
		GetDlgItem(IDC_EXTENSION)->EnableWindow(FALSE);
	}
	else
		SetWindowText(LoadStringResource(IDS_ADD_FILE_APPLICATION_ASSOCIATION));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

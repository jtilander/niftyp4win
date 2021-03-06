//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Change.h

#ifndef __P4CHANGE__
#define __P4CHANGE__

// Class CP4Change - a simple wrapper to parse and store info returned by 'P4 changes'

class CP4Change : public CObject
{
public:
	CP4Change();
	DECLARE_DYNCREATE(CP4Change)
	
protected:
	long m_ChangeNumber;
	CString m_ChangeDate;
	CString m_User;
	BOOL m_Initialized;
	BOOL m_Pending;
	BOOL m_MyChange;
	CString m_Description;
	
public:
	BOOL Create(LPCTSTR changesRow, LPCTSTR currentUser);  // char * as returned by 'p4 changes'

	inline BOOL IsPending() const {ASSERT(m_Initialized); return m_Pending;}
	inline BOOL IsMyChange() const {ASSERT(m_Initialized); return m_MyChange;}
	CString GetFormattedChange(BOOL showChangeDesc, BOOL sortByUser) const;

	inline long GetChangeNumber() const { ASSERT(m_Initialized); return m_ChangeNumber; }
	inline LPCTSTR GetChangeDate() const { ASSERT(m_Initialized); return LPCTSTR(m_ChangeDate); }
	inline LPCTSTR GetUser() const { ASSERT(m_Initialized); return LPCTSTR(m_User); }
	inline LPCTSTR GetDescription() const { ASSERT(m_Initialized); return LPCTSTR(m_Description); }
	inline void SetDescription(LPCTSTR str) { ASSERT(m_Initialized); m_Description = str; }

	~CP4Change();

};

#endif //__P4CHANGE__

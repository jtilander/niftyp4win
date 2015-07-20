#if !defined(AFX_BookMarkAdd_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)
#define AFX_BookMarkAdd_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BookMarkAdd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBookMarkAdd dialog

class CBookMarkAdd : public CDialog
{
// Construction
public:
	CBookMarkAdd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBookMarkAdd)
	enum { IDD = IDD_BOOKMARKADD };
	CString	m_Name;
	int		m_MenuType;
	int     m_ChgPCU;
	CString m_Port;
	CString m_Client;
	CString m_User;
	CString m_Desc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBookMarkAdd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_Title;
	CString m_LabelText;
	BOOL m_IsOK2Cr8SubMenu;
	BOOL m_RadioShow;	// 0=show both, 1=disable 1st, 2=disable 2nd, 3=hide both

public:
	CString GetNewMenuName() { return m_Name; }
	void SetNewMenuName(CString name) { m_Name = name; }
	void SetTitle(CString title) { m_Title = title; }
	void SetLabelText(CString txt) { m_LabelText = txt; }
	void SetIsSubMenu(BOOL b) { m_MenuType = b ? 1 : 0; }
	BOOL GetIsSubMenu() { return m_MenuType == 1; }
	void SetCanCr8SubMenu(BOOL b) { m_IsOK2Cr8SubMenu = b; }
	void SetRadioShow(int x) { m_RadioShow = x; }
	
protected:

	// Generated message map functions
	//{{AFX_MSG(CBookMarkAdd)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCommand();
	afx_msg void OnSubMenu();
	afx_msg void OnSamePCU();
	afx_msg void OnDiffPCU();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BookMarkAdd_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)

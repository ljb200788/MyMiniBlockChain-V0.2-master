
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList ����
class COutputWnd;
class COutputList : public CListBox
{
// ����
public:
	COutputList();
// ʵ��
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// ����
public:
	COutputWnd();

	void UpdateFonts();

// ����
protected:
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputInfo;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);
	
// ʵ��
public:
	virtual ~COutputWnd();
	void AddInfo(char* info);
	void Reset();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};


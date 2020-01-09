
// ServerView.h : CServerView ��Ľӿ�
//

#pragma once
#include "afxwin.h"
#include "MyMiniBlockChainServer.h"


class CServerView : public CFormView
{
protected: // �������л�����
	CServerView();
	DECLARE_DYNCREATE(CServerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_SERVER_FORM };
#endif

// ����
public:
	CServerDoc* GetDocument() const;

// ����
public:
	MyMiniBlockChainServer* server[2];
	int serverNumber;
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_ctrlListBoxInfo;
	void AddInfo(char* info);
	afx_msg void OnCreateworldBlock();
};

#ifndef _DEBUG  // ServerView.cpp �еĵ��԰汾
inline CServerDoc* CServerView::GetDocument() const
   { return reinterpret_cast<CServerDoc*>(m_pDocument); }
#endif


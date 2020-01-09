
// MyMiniBlockChainView.h : CMyMiniBlockChainView ��Ľӿ�
//

#pragma once
#include "MiniBlockChain.h"
#define WM_CUTNODE WM_USER+10

#include <iostream>
#include <string> 
using namespace std;

class CMyMiniBlockChainView : public CScrollView
{
protected: // �������л�����
	CMyMiniBlockChainView();
	DECLARE_DYNCREATE(CMyMiniBlockChainView)

// ����
public:
	CMyMiniBlockChainDoc* GetDocument() const;
	int  m_iVerticalSpace;
	int  m_iHorizontalSpace;
	char* m_strNode1Uuid;
	char* m_strNode2Uuid;
	CString m_strDrawInfo;
// ����
public:
	CMiniBlockChain* m_pMiniBlockChain;
	CMiniBlockChain* m_pMiniBlockChain2;
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMyMiniBlockChainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	void DrawChain2Info(string info);
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	
public:
	afx_msg void OnGetBlockData();
	virtual void OnInitialUpdate();
	afx_msg void OnNode1MineBlock();
	afx_msg void OnNode2MineBlock();
	afx_msg void OnNode1Transaction();
	afx_msg void OnNode2Transaction();
	afx_msg void OnNode2GetChain();
	afx_msg LRESULT OnEditCut(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRegisterNode2forNode1();
	afx_msg void OnRegisterNode1ForNode2();
	afx_msg void OnValidChain2();
	afx_msg void OnConsensusForNode1();
	afx_msg void OnConsensusForNode2();
};

#ifndef _DEBUG  // MyMiniBlockChainView.cpp �еĵ��԰汾
inline CMyMiniBlockChainDoc* CMyMiniBlockChainView::GetDocument() const
   { return reinterpret_cast<CMyMiniBlockChainDoc*>(m_pDocument); }
#endif


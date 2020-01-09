
// MyMiniBlockChainView.h : CMyMiniBlockChainView 类的接口
//

#pragma once
#include "MiniBlockChain.h"
#define WM_CUTNODE WM_USER+10

#include <iostream>
#include <string> 
using namespace std;

class CMyMiniBlockChainView : public CScrollView
{
protected: // 仅从序列化创建
	CMyMiniBlockChainView();
	DECLARE_DYNCREATE(CMyMiniBlockChainView)

// 特性
public:
	CMyMiniBlockChainDoc* GetDocument() const;
	int  m_iVerticalSpace;
	int  m_iHorizontalSpace;
	char* m_strNode1Uuid;
	char* m_strNode2Uuid;
	CString m_strDrawInfo;
// 操作
public:
	CMiniBlockChain* m_pMiniBlockChain;
	CMiniBlockChain* m_pMiniBlockChain2;
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMyMiniBlockChainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	void DrawChain2Info(string info);
// 生成的消息映射函数
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

#ifndef _DEBUG  // MyMiniBlockChainView.cpp 中的调试版本
inline CMyMiniBlockChainDoc* CMyMiniBlockChainView::GetDocument() const
   { return reinterpret_cast<CMyMiniBlockChainDoc*>(m_pDocument); }
#endif


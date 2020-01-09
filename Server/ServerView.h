
// ServerView.h : CServerView 类的接口
//

#pragma once
#include "afxwin.h"
#include "MyMiniBlockChainServer.h"


class CServerView : public CFormView
{
protected: // 仅从序列化创建
	CServerView();
	DECLARE_DYNCREATE(CServerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_SERVER_FORM };
#endif

// 特性
public:
	CServerDoc* GetDocument() const;

// 操作
public:
	MyMiniBlockChainServer* server[2];
	int serverNumber;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_ctrlListBoxInfo;
	void AddInfo(char* info);
	afx_msg void OnCreateworldBlock();
};

#ifndef _DEBUG  // ServerView.cpp 中的调试版本
inline CServerDoc* CServerView::GetDocument() const
   { return reinterpret_cast<CServerDoc*>(m_pDocument); }
#endif


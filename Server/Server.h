
// Server.h : Server Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CServerApp:
// �йش����ʵ�֣������ Server.cpp
//
#include "ServerDoc.h"
#include "ServerView.h"

class CServerApp : public CWinApp
{
public:
	CServerApp();
	CServerView* pServerView;

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CServerApp theApp;

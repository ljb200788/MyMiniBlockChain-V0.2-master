
// ServerView.cpp : CServerView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Server.h"
#endif

#include "ServerDoc.h"
#include "ServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PORT1 5000
#define PORT2 5001

// CServerView
IMPLEMENT_DYNCREATE(CServerView, CFormView)

BEGIN_MESSAGE_MAP(CServerView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(IDM_CREATEWORLD_BLOCK, &CServerView::OnCreateworldBlock)
END_MESSAGE_MAP()

// CServerView ����/����

CServerView::CServerView()
	: CFormView(IDD_SERVER_FORM)
{
	// TODO: �ڴ˴���ӹ������
	serverNumber = 2;
	for (int i = 0; i < serverNumber;i++)
		server[i] = new MyMiniBlockChainServer;
}

CServerView::~CServerView()
{
	for (int i = 0; i < serverNumber; i++)
		if (server[i] != NULL)
		{
			free(server[i]);
			server[i] = NULL;
		}
}

void CServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTINFO, m_ctrlListBoxInfo);
}

BOOL CServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	theApp.pServerView = this;
	cs.style &= ~WS_BORDER;
	return CFormView::PreCreateWindow(cs);
}

void CServerView::OnInitialUpdate()
{
	CServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->SetTitle(_T("΢��V0.2"));

	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}


// CServerView ���
#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG

// CServerView ��Ϣ�������
void CServerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CWnd *pWnd;
	pWnd = this->GetDlgItem(IDC_LISTINFO);
	if (pWnd == NULL)
		return;
	pWnd->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
}

void CServerView::AddInfo(char* info)
{
	TCHAR* temp = (TCHAR*)malloc(sizeof(TCHAR)*(strlen(info) + 1));
	int iLength = MultiByteToWideChar(CP_ACP, 0, info, strlen(info) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, info, strlen(info) + 1, temp, iLength);
	m_ctrlListBoxInfo.AddString(temp);
	delete temp;
	return;
}

void CServerView::OnCreateworldBlock()
{
	// TODO: �ڴ���������������
	char timeStamp[18];
	server[0]->m_pMiniBlockChain->getTimeStamp(timeStamp);
	server[0]->Init(PORT1, timeStamp);
	server[0]->Start();
	server[1]->Init(PORT2, timeStamp);
	server[1]->Start();
}

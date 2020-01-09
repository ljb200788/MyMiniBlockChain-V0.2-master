// MyMiniBlockChainView.cpp : CMyMiniBlockChainView 类的实现
//

#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "MyMiniBlockChain.h"
#endif

#include "MyMiniBlockChainDoc.h"
#include "MyMiniBlockChainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MainFrm.h"
#pragma comment(lib,"Rpcrt4.lib")  
// CMyMiniBlockChainView


#include "Wininet.h"  
#pragma comment(lib,"Wininet.lib")  
#define PORT1 5000
#define PORT2 5001

void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
	return;
}

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_ACP, NULL, tchar, -1, _char, iLength, NULL, FALSE);
	return;
}
//模拟浏览器发送HTTP请求函数  
std::string HttpRequest(char * lpHostName, short sPort, char * lpUrl, char * lpMethod, char * lpPostData, int nPostDataLen)
{
	HINTERNET hInternet = 0, hConnect = 0, hRequest = 0;

	BOOL bRet;
	TCHAR* lptHostName = (TCHAR*)malloc(sizeof(TCHAR)*(strlen(lpHostName) + 1));
	CharToTchar(lpHostName, lptHostName);
	TCHAR* lptMethod = (TCHAR*)malloc(sizeof(TCHAR)*(strlen(lpMethod) + 1));
	CharToTchar(lpMethod, lptMethod);
	TCHAR* lptUrl = NULL;
	if (lpUrl != NULL)
	{
		lptUrl = (TCHAR*)malloc(sizeof(TCHAR)*(strlen(lpUrl) + 1));
		CharToTchar(lpUrl, lptUrl);
	}

	std::string strResponse;

	hInternet = (HINSTANCE)InternetOpen(_T("User-Agent"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInternet)
		goto Ret0;

	hConnect = (HINSTANCE)InternetConnect(hInternet, lptHostName, sPort, NULL, _T("HTTP/1.1"), INTERNET_SERVICE_HTTP, 0, 0);
	if (!hConnect)
		goto Ret0;

	hRequest = (HINSTANCE)HttpOpenRequest(hConnect, lptMethod, lptUrl, _T("HTTP/1.1"), NULL, NULL, INTERNET_FLAG_RELOAD, 0);
	if (!hRequest)
		goto Ret0;

	//bRet = HttpAddRequestHeaders(hRequest,"Content-Type: application/json",Len(FORMHEADERS),HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);  
	//if(!bRet)  
	//goto Ret0;  

	bRet = HttpSendRequest(hRequest, NULL, 0, lpPostData, nPostDataLen);
	while (TRUE)
	{
		char cReadBuffer[4096];
		unsigned long lNumberOfBytesRead;
		bRet = InternetReadFile(hRequest, cReadBuffer, sizeof(cReadBuffer) - 1, &lNumberOfBytesRead);
		if (!bRet || !lNumberOfBytesRead)
			break;
		cReadBuffer[lNumberOfBytesRead] = 0;
		strResponse = strResponse + cReadBuffer;
	}

Ret0:
	if (hRequest)
		InternetCloseHandle(hRequest);
	if (hConnect)
		InternetCloseHandle(hConnect);
	if (hInternet)
		InternetCloseHandle(hInternet);

	return strResponse;
}



IMPLEMENT_DYNCREATE(CMyMiniBlockChainView, CScrollView)

BEGIN_MESSAGE_MAP(CMyMiniBlockChainView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMyMiniBlockChainView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_GET_BLOCKDATA, &CMyMiniBlockChainView::OnGetBlockData)
	ON_COMMAND(IDM_NODE1_MINE_BLOCK, &CMyMiniBlockChainView::OnNode1MineBlock)
	ON_COMMAND(IDM_NODE2_MINE_BLOCK, &CMyMiniBlockChainView::OnNode2MineBlock)
	ON_COMMAND(IDM_NODE1_TRANSACTION, &CMyMiniBlockChainView::OnNode1Transaction)
	ON_COMMAND(IDM_NODE2_TRANSACTION, &CMyMiniBlockChainView::OnNode2Transaction)
	ON_COMMAND(IDM_NODE2_GET_CHAIN, &CMyMiniBlockChainView::OnNode2GetChain)
	ON_MESSAGE(WM_CUTNODE, OnEditCut)
	ON_COMMAND(IDM_REGISTER_NODE2, &CMyMiniBlockChainView::OnRegisterNode2forNode1)
	ON_COMMAND(IDM_REGISTER_NODE1, &CMyMiniBlockChainView::OnRegisterNode1ForNode2)
	ON_COMMAND(IDM_VALID_CHAIN, &CMyMiniBlockChainView::OnValidChain2)
	ON_COMMAND(IDM_CONSENSUS_FORNODE1, &CMyMiniBlockChainView::OnConsensusForNode1)
	ON_COMMAND(IDM_CONSENSUS_FORNODE2, &CMyMiniBlockChainView::OnConsensusForNode2)
END_MESSAGE_MAP()

// CMyMiniBlockChainView 构造/析构

CMyMiniBlockChainView::CMyMiniBlockChainView()
{
	// TODO: 在此处添加构造代码
	m_pMiniBlockChain = new CMiniBlockChain;
	m_pMiniBlockChain2 = new CMiniBlockChain;
	m_iVerticalSpace = 10;
	m_iHorizontalSpace = 10;
	m_strDrawInfo = _T("");

	UUID globalUUID;
	RPC_WSTR tempRPCString;
	CString  Node1Uuid;
	CString  Node2Uuid;
	UuidCreate(&globalUUID);
	UuidToString(&globalUUID, &tempRPCString);
	Node1Uuid.Format(_T("%s"), tempRPCString);
	RpcStringFree(&tempRPCString);
	Node1Uuid.Replace(_T("-"), _T(""));
	UuidCreate(&globalUUID);
	UuidToString(&globalUUID, &tempRPCString);
	Node2Uuid.Format(_T("%s"), tempRPCString);
	RpcStringFree(&tempRPCString);
	Node2Uuid.Replace(_T("-"), _T(""));

	int length = Node1Uuid.GetLength();
	m_strNode1Uuid = (char*)malloc(sizeof(char)*(length + 1));
	TcharToChar(Node1Uuid.GetBuffer(0), m_strNode1Uuid);
	length = Node2Uuid.GetLength();
	m_strNode2Uuid = (char*)malloc(sizeof(char)*(length + 1));
	TcharToChar(Node2Uuid.GetBuffer(0), m_strNode2Uuid);
}

CMyMiniBlockChainView::~CMyMiniBlockChainView()
{
	if (m_pMiniBlockChain != NULL)
	{
		free(m_pMiniBlockChain);
		m_pMiniBlockChain = NULL;
	}

	if (m_pMiniBlockChain2 != NULL)
	{
		free(m_pMiniBlockChain2);
		m_pMiniBlockChain2 = NULL;
	}

	if (m_strNode1Uuid != NULL)
	{
		free(m_strNode1Uuid);
		m_strNode1Uuid = NULL;
	}

	if (m_strNode2Uuid != NULL)
	{
		free(m_strNode2Uuid);
		m_strNode2Uuid = NULL;
	}
}

BOOL CMyMiniBlockChainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	theApp.pMyMiniBlockChainView = this;
	return CScrollView::PreCreateWindow(cs);
}



void CMyMiniBlockChainView::OnInitialUpdate()
{
	CMyMiniBlockChainDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->SetTitle(_T("微链V0.2"));
	CScrollView::OnInitialUpdate();
	CSize sizeTotal(30000, 40000);
	CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
	CSize sizeLine(sizeTotal.cx / 100, sizeTotal.cy / 100);
	SetScrollSizes(MM_HIMETRIC, sizeTotal, sizePage, sizeLine);
}

void CMyMiniBlockChainView::OnDraw(CDC* pDC)
{
	BeginWaitCursor();
	CMyMiniBlockChainDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	_ASSERTE(_CrtCheckMemory());
	if (!pDoc)
		return;
	CRect textRect;
	int length = m_strDrawInfo.GetLength();
	char* bs = (char*)malloc(sizeof(char)*(length + 1));
	TcharToChar(m_strDrawInfo.GetBuffer(0), bs);
	char* ptemp = bs;
	char* qtemp = NULL;
	qtemp = strstr(ptemp, "\n");
	m_iVerticalSpace = 10;
	m_iHorizontalSpace = 10;
	while (qtemp != NULL)
	{
		int length = qtemp - ptemp;
		ptemp[length] = '\0';
		TCHAR * tcharTemp = (TCHAR*)malloc(sizeof(TCHAR)*(length + 1));
		CharToTchar(ptemp, tcharTemp);
		pDC->DrawText(tcharTemp, strlen(ptemp), &textRect, DT_CALCRECT);
		pDC->TextOut(m_iHorizontalSpace, m_iVerticalSpace, tcharTemp);
		m_iVerticalSpace += textRect.Height();
		ptemp = qtemp + 1;
		qtemp = strstr(ptemp, "\n");
		free(tcharTemp);
	}
	free(bs);
	EndWaitCursor();
}

void CMyMiniBlockChainView::DrawChain2Info(string info)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndOutput.Reset();
	char* ptemp = (char*)info.data();
	char* qtemp = NULL;
	qtemp = strstr(ptemp, "\n");
	while (qtemp != NULL)
	{
		int length = qtemp - ptemp;
		ptemp[length] = '\0';
		pMainFrame->m_wndOutput.AddInfo(ptemp);
		ptemp = qtemp + 1;
		qtemp = strstr(ptemp, "\n");
	}
	return;
}

// CMyMiniBlockChainView 打印
void CMyMiniBlockChainView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMyMiniBlockChainView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CMyMiniBlockChainView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CMyMiniBlockChainView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CMyMiniBlockChainView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMyMiniBlockChainView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

#ifdef _DEBUG
void CMyMiniBlockChainView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMyMiniBlockChainView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMyMiniBlockChainDoc* CMyMiniBlockChainView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyMiniBlockChainDoc)));
	return (CMyMiniBlockChainDoc*)m_pDocument;
}
#endif //_DEBUG

LRESULT CMyMiniBlockChainView::OnEditCut(WPARAM wParam, LPARAM lParam)
{
	long index = lParam;
	if (index < 0 && index> m_pMiniBlockChain2->getBlockLength())
		return index;
	m_pMiniBlockChain2->deleteNode(index);
	return 0;
}

void CMyMiniBlockChainView::OnGetBlockData()
{
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT1, "/chain", "GET", NULL, 0);
	m_strDrawInfo += strResponse.data();
	//更新区块链节点1视图
	Invalidate();
	UpdateWindow();
	strResponse = HttpRequest("127.0.0.1", PORT2, "/chain", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}

void CMyMiniBlockChainView::OnNode1MineBlock()
{
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT1, "/mine", "GET", NULL, 0);
	m_strDrawInfo += strResponse.data();
	//更新区块链节点1视图
	Invalidate();
	UpdateWindow();
}


void CMyMiniBlockChainView::OnNode2MineBlock()
{
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/mine", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}

//模拟随机产生一个用户ID作为sender与节点1用户发生交易
void CMyMiniBlockChainView::OnNode1Transaction()
{
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT1, "/transactions/new", "GET", NULL, 0);
	m_strDrawInfo = strResponse.data();
	//更新区块链节点1视图
	Invalidate();
	UpdateWindow();
}

//模拟随机产生一个用户ID作为sender与节点2用户发生交易
void CMyMiniBlockChainView::OnNode2Transaction()
{
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/transactions/new", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}

void CMyMiniBlockChainView::OnNode2GetChain()
{
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/chain", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}

void CMyMiniBlockChainView::OnRegisterNode2forNode1()
{
	m_strDrawInfo.Empty();
	char buffer[50];
	sprintf(buffer, "{ nodes:[http://127.0.0.1:%d] }", PORT2);
	std::string strResponse = HttpRequest("127.0.0.1", PORT1, "/nodes/register", "POST", buffer, strlen(buffer));
	m_strDrawInfo += strResponse.data();
	//更新区块链节点1视图
	Invalidate();
	UpdateWindow();
}

void CMyMiniBlockChainView::OnRegisterNode1ForNode2()
{
	char buffer[50];
	sprintf(buffer, "{ nodes:[http://127.0.0.1:%d] }", PORT1);
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/nodes/register", "POST", buffer, strlen(buffer));
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}


void CMyMiniBlockChainView::OnValidChain2()
{
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/valid", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}


void CMyMiniBlockChainView::OnConsensusForNode1()
{
	//共识算法解决冲突,使用网络中最长的链作为共识
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT1, "/nodes/resolve", "GET", NULL, 0);
	m_strDrawInfo += strResponse.data();
	//更新区块链节点1视图
	Invalidate();
	UpdateWindow();
}


void CMyMiniBlockChainView::OnConsensusForNode2()
{
	//共识算法解决冲突,使用网络中最长的链作为共识
	m_strDrawInfo.Empty();
	std::string strResponse = HttpRequest("127.0.0.1", PORT2, "/nodes/resolve", "GET", NULL, 0);
	//更新区块链节点2视图
	DrawChain2Info(strResponse);
}

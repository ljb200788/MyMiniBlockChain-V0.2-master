#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#include "MyMiniBlockChainDoc.h"
#include "MyMiniBlockChainView.h"
#include "MyMiniBlockChain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出选项卡窗口\n");
		return -1;      // 未能创建
	}
	// 创建输出窗格: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputInfo.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}
	UpdateFonts();

	CString strTabName=_T("区块链节点2");
	m_wndTabs.AddTab(&m_wndOutputInfo, strTabName, (UINT)0);
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::AddInfo(char* info)
{
	TCHAR* temp = (TCHAR*)malloc(sizeof(TCHAR)*(strlen(info) + 1));
	int iLength = MultiByteToWideChar(CP_ACP, 0, info, strlen(info) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, info, strlen(info) + 1, temp, iLength);
	m_wndOutputInfo.AddString(temp);
	delete temp;
	return;
}

void COutputWnd::Reset()
{
	m_wndOutputInfo.ResetContent();
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputInfo.SetFont(&afxGlobalData.fontRegular);
}
/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	if (OpenClipboard()) //首先打开一个剪切板，如果成功则返回非0值  
	{
		int iCurSel = GetCurSel();
		if (LB_ERR != iCurSel)
		{
			HANDLE hClip;      //声明一个句柄  
			CString str;
			char *pBuf;
			GetText(iCurSel, str);
			EmptyClipboard();  //置空这个剪切板，且得到剪切板的所有权  
			hClip = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
			//申请锁定一块存放数据的内存区域  
			pBuf = (char *)GlobalLock(hClip);//得到指向内存区域的第一个字节指针  
			int iLength = WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(0), -1, NULL, 0, NULL, FALSE);
			WideCharToMultiByte(CP_ACP, NULL, str.GetBuffer(0), -1, pBuf, iLength, NULL, FALSE);
			GlobalUnlock(hClip);//解除内存锁定  
			SetClipboardData(CF_TEXT, hClip);//设置数据到剪切板中  
			CloseClipboard();//关闭剪切板  
		}
	}
}

void COutputList::OnEditCut()
{
	OnEditCopy();
	int iCurSel = GetCurSel();
	DeleteString(iCurSel);
	CMyMiniBlockChainView*pView = theApp.pMyMiniBlockChainView;
	::SendMessage(pView->m_hWnd, WM_CUTNODE, NULL, iCurSel);
}

void COutputList::OnEditClear()
{
	ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner()->GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());
	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
	}
}

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

	// ����ѡ�����: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("δ�ܴ������ѡ�����\n");
		return -1;      // δ�ܴ���
	}
	// �����������: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputInfo.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("δ�ܴ����������\n");
		return -1;      // δ�ܴ���
	}
	UpdateFonts();

	CString strTabName=_T("�������ڵ�2");
	m_wndTabs.AddTab(&m_wndOutputInfo, strTabName, (UINT)0);
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	// ѡ��ؼ�Ӧ��������������: 
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
	if (OpenClipboard()) //���ȴ�һ�����а壬����ɹ��򷵻ط�0ֵ  
	{
		int iCurSel = GetCurSel();
		if (LB_ERR != iCurSel)
		{
			HANDLE hClip;      //����һ�����  
			CString str;
			char *pBuf;
			GetText(iCurSel, str);
			EmptyClipboard();  //�ÿ�������а壬�ҵõ����а������Ȩ  
			hClip = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
			//��������һ�������ݵ��ڴ�����  
			pBuf = (char *)GlobalLock(hClip);//�õ�ָ���ڴ�����ĵ�һ���ֽ�ָ��  
			int iLength = WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(0), -1, NULL, 0, NULL, FALSE);
			WideCharToMultiByte(CP_ACP, NULL, str.GetBuffer(0), -1, pBuf, iLength, NULL, FALSE);
			GlobalUnlock(hClip);//����ڴ�����  
			SetClipboardData(CF_TEXT, hClip);//�������ݵ����а���  
			CloseClipboard();//�رռ��а�  
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

#include "StdAfx.h"
#include "MainForm.h"

DUI_BEGIN_MESSAGE_MAP(CPage1, CNotifyPump)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CPage1::CPage1()
{
    m_pPaintManager = NULL;
}

void CPage1::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
    m_pPaintManager = pPaintMgr;
}

void CPage1::OnClick(TNotifyUI& msg)
{
    
}

void CPage1::OnSelectChanged(TNotifyUI &msg)
{

}

void CPage1::OnItemClick(TNotifyUI &msg)
{

}

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CPage2, CNotifyPump)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()


CPage2::CPage2()
{
    m_pPaintManager = NULL;
}

void CPage2::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
    m_pPaintManager = pPaintMgr;
}

void CPage2::OnClick(TNotifyUI& msg)
{

}

void CPage2::OnSelectChanged(TNotifyUI &msg)
{

}

void CPage2::OnItemClick(TNotifyUI &msg)
{

}

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CMainForm, WindowImplBase)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CMainForm::CMainForm(void)
{
    m_Page1.SetPaintMagager(&m_PaintManager);
    m_Page2.SetPaintMagager(&m_PaintManager);

    AddVirtualWnd(_T("page1"), &m_Page1);
    AddVirtualWnd(_T("page2"), &m_Page2);
}

CMainForm::~CMainForm(void)
{
    RemoveVirtualWnd(_T("page1"));
    RemoveVirtualWnd(_T("page2"));
}

void CMainForm::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}

DuiLib::CDuiString CMainForm::GetSkinFolder()
{
#ifdef _DEBUG
    return _T("skin\\Demo\\");
#else
    return _T("skin\\");
#endif

}

DuiLib::CDuiString CMainForm::GetSkinFile()
{
    return _T("MainForm.xml");
}

UILIB_RESOURCETYPE CMainForm::GetResourceType() const
{
#ifdef _DEBUG
    return UILIB_FILE;
#else
    return UILIB_ZIP;
#endif
}

LPCTSTR CMainForm::GetWindowClassName(void) const
{
    return _T("MainForm");
}

void CMainForm::OnClick(TNotifyUI &msg)
{
    if (msg.pSender == m_pCloseBtn)
    {
        PostQuitMessage(0); // 因为activex的原因，使用close可能会出现错误
        return;
    }
    else if (msg.pSender == m_pMinBtn)
    {
        SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return;
    }
    else if (msg.pSender == m_pMaxBtn)
    {
        SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        return;
    }
    else if (msg.pSender == m_pRestoreBtn)
    {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
        return;
    }
}

void CMainForm::OnSelectChanged(TNotifyUI &msg)
{

}

void CMainForm::OnItemClick(TNotifyUI &msg)
{
   
}

void CMainForm::Notify(TNotifyUI &msg)
{
    return WindowImplBase::Notify(msg);
}

LRESULT CMainForm::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

    bHandled = FALSE;
    return 0;
}

LRESULT CMainForm::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
    if (wParam == SC_CLOSE) {
        ::PostQuitMessage(0L);
        bHandled = TRUE;
        return 0;
    }
    BOOL bZoomed = ::IsZoomed(*this);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(*this) != bZoomed) {
        if (!bZoomed) {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
            if (pControl) pControl->SetVisible(true);
        }
        else {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
            if (pControl) pControl->SetVisible(false);
        }
    }
    return lRes;
}

void CMainForm::InitWindow()
{
    m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("closebtn")));
    m_pMaxBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("maxbtn")));
    m_pRestoreBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("restorebtn")));
    m_pMinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("minbtn")));
}

LRESULT CMainForm::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

    return 0;
}

LRESULT CMainForm::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

    return 0;
}


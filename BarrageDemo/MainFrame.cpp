//
// main_frame.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "resource.h"
#include "MainFrame.h"


const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* g_szText = _T("弹幕技术测试测试！！！！");
int g_iFont = 1;
bool g_bAlpha = true;


DWORD _d1 = 0;
DWORD _d2 = 0;
DWORD g_value = 0;
DWORD g_value1 = 1;
DWORD g_value2 = 1;
class CTestUI : public CControlUI
{
public:
    CTestUI()
        : m_pLabel(NULL)
        , m_pLabel1(NULL)
        , m_pLabel2(NULL)
    {
        m_rc.left = 0;
        m_rc.top = 0;
        m_rc.right = 0;
        m_rc.bottom = 0;

        //
    }

    bool OnPlay(void* param)
    {
        TNotifyUI *pMsg = (TNotifyUI*)param;

        if (pMsg->sType == _T("timer"))
        {
            if (pMsg->wParam == 111)
            {
                DWORD d1 = GetTickCount();
                if (_d1 == 0) _d1 = d1;

                if (d1 - _d1 >= g_value)
                {
                    _d1 = d1;

                    m_rc.left -= 4;
                    m_rc.right -= 4;

                    if (m_rc.left <= 3)
                        m_rc.left = 800 - m_szText.cx;

                    if (m_rc.right <= (3 + m_szText.cx))
                        m_rc.right = 800;
                }

                if (m_pLabel != NULL)
                {
                    DWORD d2 = d1;
                    if (_d2 == 0) _d2 = d2;

                    if (d2 - _d2 >= g_value)
                    {
                        _d2 = d2;

                        RECT rc = m_pLabel->GetPos();
                        rc.left -= 2;
                        rc.right -= 2;

                        if (rc.left <= 0)
                            rc.left = 700;

                        if (rc.right <= 100)
                            rc.right = 800;

                        m_pLabel->SetPos(rc);
                    }
                }
            }
            else if (pMsg->wParam == 222)
            {
                if (m_pLabel1 != NULL)
                {
                    RECT rc = m_pLabel1->GetPos();
                    rc.left -= g_value1;
                    rc.right -= g_value1;

                    if (rc.left <= 0)
                        rc.left = 700;

                    if (rc.right <= 100)
                        rc.right = 800;

                    m_pLabel1->SetPos(rc);
                }
            }
            else if (pMsg->wParam == 333)
            {
                if (m_pLabel2 != NULL)
                {
                    RECT rc = m_pLabel2->GetPos();
                    rc.left -= g_value2;
                    rc.right -= g_value2;

                    if (rc.left <= 0)
                        rc.left = 700;

                    if (rc.right <= 100)
                        rc.right = 800;

                    m_pLabel2->SetPos(rc);
                }
            }

           // Invalidate();
            ::InvalidateRect(m_pManager->GetPaintWindow(), NULL, FALSE);
            ::UpdateWindow(m_pManager->GetPaintWindow());
        }

        return true;
    }


    void DoInit()
    {
       
    }

    ~CTestUI(){}

    LPCTSTR GetClass() const
    {
        return _T("TestUI");
    }

    LPVOID GetInterface(LPCTSTR pstrName)
    {
        return NULL;
    }


    void DoPaint(HDC hDC, const RECT& rcPaint)
    {
        if (m_rc.left == 0 && m_rc.top == 0 && m_rc.right == 0 && m_rc.bottom == 0)
        {
            m_szText = CRenderEngine::GetTextSize(hDC, m_pManager, g_szText, g_iFont, 0);
            m_rc.left = 800 - m_szText.cx;
            m_rc.top = 50;
            m_rc.right = 800;
            m_rc.bottom = 50 + m_szText.cy;

            m_pLabel = static_cast<CLabelUI*>(m_pManager->FindControl(_T("aaa")));
            m_pLabel1 = static_cast<CLabelUI*>(m_pManager->FindControl(_T("bbb")));
            m_pLabel2 = static_cast<CLabelUI*>(m_pManager->FindControl(_T("ccc")));

            this->OnNotify += MakeDelegate(this, &CTestUI::OnPlay);
            m_pManager->SetTimer(this, 111, 5);
            m_pManager->SetTimer(this, 222, 10);
            m_pManager->SetTimer(this, 333, 20);
        }

        CRenderEngine::DrawText(hDC, m_pManager, m_rc, g_szText, 0xFFFFFFFF, g_iFont, DT_LEFT);
    }

private:
    RECT m_rc;
    SIZE m_szText;
    CLabelUI *m_pLabel;
    CLabelUI *m_pLabel1;
    CLabelUI *m_pLabel2;
};

MainFrame::MainFrame()
{

}

MainFrame::~MainFrame()
{

}

LPCTSTR MainFrame::GetWindowClassName() const
{
    return _T("UIMainFrame");
}

CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
    if (lstrcmpi(pstrClass, _T("Test")) == 0)
        return new CTestUI();

    return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
    WindowImplBase::OnFinalMessage(hWnd);
    delete this;
}

CDuiString MainFrame::GetSkinFile()
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    wsprintf(szBuf, _T("%d"), IDR_SKINXML);
    return szBuf;
}

CDuiString MainFrame::GetSkinFolder()
{
    return _T("");
}

UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
    return UILIB_RESOURCE;
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(m_hWnd);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(m_hWnd) != bZoomed)
    {
        if (!bZoomed)
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));

            if (pControl != NULL)
                pControl->SetVisible(false);

            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl != NULL)
                pControl->SetVisible(true);
        }
        else
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));

            if (pControl != NULL)
                pControl->SetVisible(true);

            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl != NULL)
                pControl->SetVisible(false);
        }
    }
#else
    return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

    return 0;
}

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ERASEBKGND)
        return TRUE;

    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        return TRUE;
    }
    return FALSE;
}

void MainFrame::OnTimer(TNotifyUI& msg)
{

}

void MainFrame::OnExit(TNotifyUI& msg)
{
    Close();
}

void MainFrame::InitWindow()
{

}

void MainFrame::OnPrepare(TNotifyUI& msg)
{

}

void MainFrame::Notify(TNotifyUI& msg)
{
    if (lstrcmpi(msg.sType, _T("menu")) == 0)
    {
        ShowContextMenu(msg);
    }
    else if (lstrcmpi(msg.sType, _T("valuechanged")) == 0)
    {

        CSliderUI *pSlider = static_cast<CSliderUI*>(msg.pSender);
        if (pSlider != NULL)
        {
            if (pSlider->GetName() == _T("alpha_controlor"))
                g_value = pSlider->GetValue();
            else if (pSlider->GetName() == _T("alpha_controlor1"))
                g_value1 = pSlider->GetValue();
            else  if (pSlider->GetName() == _T("alpha_controlor2"))
                g_value2 = pSlider->GetValue();
        }
    
    }
    else if (lstrcmpi(msg.sType, _T("windowinit")) == 0)
    {
        OnPrepare(msg);
    }
    else if (lstrcmpi(msg.sType, _T("click")) == 0)
    {
        if (lstrcmpi(msg.pSender->GetName(), _T("alpha")) == 0)
        {
            m_PaintManager.SetBackgroundTransparent(true);
            m_PaintManager.GetRoot()->SetBkImage(_T("res='115' restype='png' fade='0'"));
            msg.pSender->SetVisible(false);
        }
        else if (lstrcmpi(msg.pSender->GetName(), kCloseButtonControlName) == 0)
        {
            OnExit(msg);
        }
        else if (lstrcmpi(msg.pSender->GetName(), kMinButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
        }
        else if (lstrcmpi(msg.pSender->GetName(), kMaxButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if( pControl ) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if( pControl ) pControl->SetVisible(true);
#else
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
        }
        else if (lstrcmpi(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_RESTORE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if( pControl ) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if( pControl ) pControl->SetVisible(false);
#else
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
        }
        else if (lstrcmpi(msg.pSender->GetName(), _T("btn_menu")) == 0)
        {
            ShowContextMenu(msg);
        }
    }
    else if (lstrcmpi(msg.sType, _T("timer")) == 0)
    {
        return OnTimer(msg);
    }
}

LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 0;
}

void MainFrame::ShowContextMenu(TNotifyUI& msg)
{
    //CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
    //CPoint point = msg.ptMouse;
    //ClientToScreen(m_hWnd, &point);
    //STRINGorID xml(IDR_XML_MENU);
    //pMenu->Init(NULL, xml, _T("xml"), point);
}
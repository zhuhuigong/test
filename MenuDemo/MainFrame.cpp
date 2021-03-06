﻿//
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
#include "UIMenu.h"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

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
    if (uMsg == WM_CUSTOMMENU_CLICK)
    {
        CDuiString sName = *(CDuiString*)wParam;
        if (sName == _T("item1"))
        {
            MessageBox(m_hWnd, _T("点击[我在线上]"), _T("单击菜单项"), MB_OK);
        }
        else if (sName == _T("item8"))
        {
            MessageBox(m_hWnd, _T("点击[添加状态信息]"), _T("单击菜单项"), MB_OK);
        }
        else if (sName == _T("exit"))
        {
            OnExit(TNotifyUI());
        }

        return 0;
    }

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
    else if (lstrcmpi(msg.sType, kWindowInit) == 0)
    {
        OnPrepare(msg);
    }
    else if (lstrcmpi(msg.sType, kClick) == 0)
    {
        if (lstrcmpi(msg.pSender->GetName(), kCloseButtonControlName) == 0)
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
    else if (lstrcmpi(msg.sType, kTimer) == 0)
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
    CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
    CPoint point = msg.ptMouse;
    ClientToScreen(m_hWnd, &point);
    STRINGorID xml(IDR_XML_MENU);
    pMenu->Init(NULL, xml, _T("xml"), point);
}
﻿#include "stdafx.h"
#include "UIEdit.h"

namespace DuiLib
{
    CEditWnd::CEditWnd()
        : m_pOwner(NULL)
        , m_hBkBrush(NULL)
        , m_bInit(false)
    {

    }

    void CEditWnd::Init(CEditUI* pOwner)
    {
        m_pOwner = pOwner;
        RECT rcPos = CalPos();
        UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;

        if (m_pOwner->IsPasswordMode())
            uStyle |= ES_PASSWORD;

        Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

        HFONT hFont = NULL;
        int iFontIndex = m_pOwner->GetFont();
        if (iFontIndex != -1)
            hFont = m_pOwner->GetManager()->GetFont(iFontIndex);

        if (hFont == NULL)
            hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;

        SetWindowFont(m_hWnd, hFont, TRUE);
        Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());

        if (m_pOwner->IsPasswordMode())
            Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());

        Edit_SetText(m_hWnd, m_pOwner->GetText());
        Edit_SetModify(m_hWnd, FALSE);
        SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
        Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
        Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);

        //Styls
        LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
        styleValue |= pOwner->GetWindowStyls();
        ::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
        ::SetFocus(m_hWnd);

        // 设置自动完成
        if (pOwner->IsAutoComplete())
        {
            HMODULE hModule = ::LoadLibrary(_T("Shlwapi.dll"));
            typedef HRESULT(WINAPI *SHAC)(HWND, DWORD);
            SHAC fSHAutoComplete = (SHAC) ::GetProcAddress(hModule, "SHAutoComplete");
            if (fSHAutoComplete)
            {
                fSHAutoComplete(m_hWnd, SHACF_FILESYSTEM);
            }
        }

        m_bInit = true;
    }

    RECT CEditWnd::CalPos()
    {
        CDuiRect rcPos = m_pOwner->GetPos();
        RECT rcInset = m_pOwner->GetTextPadding();
        rcPos.left += rcInset.left;
        rcPos.top += rcInset.top;
        rcPos.right -= rcInset.right;
        rcPos.bottom -= rcInset.bottom;
        LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;

        if (lEditHeight < rcPos.GetHeight())
        {
            rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
            rcPos.bottom = rcPos.top + lEditHeight;
        }

        return rcPos;
    }

    LPCTSTR CEditWnd::GetWindowClassName() const
    {
        return _T("EditWnd");
    }

    LPCTSTR CEditWnd::GetSuperClassName() const
    {
        return WC_EDIT;
    }

    void CEditWnd::OnFinalMessage(HWND /*hWnd*/)
    {
        m_pOwner->Invalidate();

        // Clear reference and die
        if (m_hBkBrush != NULL)
            ::DeleteObject(m_hBkBrush);

        m_pOwner->m_pWindow = NULL;
        delete this;
    }

    LRESULT CEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;

        if (uMsg == WM_KILLFOCUS)
        {
            lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
        }
        else if (uMsg == OCM_COMMAND)
        {
            if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
            {
                lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
            }
            else if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
            {
                RECT rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                ::InvalidateRect(m_hWnd, &rcClient, FALSE);
            }
        }
        else if (uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN)
        {
            m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
        }
        else if (uMsg == OCM_CTLCOLOREDIT || uMsg == OCM_CTLCOLORSTATIC)
        {
            // 背景透明可参考：http://blog.sina.com.cn/s/blog_4c3538470100ezhu.html
            // http://blog.163.com/peng_dhai/blog/static/1776320062011101055938480
            //if (m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF)
            //{
            //    return NULL;
            //}

            ::SetBkMode((HDC)wParam, TRANSPARENT);
            DWORD dwTextColor = m_pOwner->GetTextColor();
            ::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

            if (m_hBkBrush == NULL)
            {
                DWORD clrColor = m_pOwner->GetNativeEditBkColor();
                m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
            }

            return (LRESULT)m_hBkBrush;
        }
        else
        {
            bHandled = FALSE;
        }

        if (!bHandled)
        {
            return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        }

        return lRes;
    }

    LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // 找到原因了，是WM_COMMAND消息处理不当！！！！！
        // http://bbs.csdn.net/topics/390784850?page=1
        // 修复编辑框失去焦点后文本丢失的问题（虽然不知道为什么会丢失！）
        /*UINT len = m_pOwner->GetMaxChar();
        TCHAR *szText = new TCHAR[len];
        ZeroMemory(szText, len * sizeof(TCHAR));
        Edit_GetText(m_hWnd, szText, m_pOwner->GetMaxChar());
        m_pOwner->SetText(szText);
        delete[] szText;*/

        LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        PostMessage(WM_CLOSE);
        return lRes;
    }

    LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (!m_bInit)
            return 0;

        if (m_pOwner == NULL)
            return 0;

        // Copy text back
        int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
        LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));

        ASSERT(pstr);
        if (pstr == NULL)
            return 0;

        ::GetWindowText(m_hWnd, pstr, cchLen);
        m_pOwner->m_sText = pstr;
        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);

        return 0;
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //
    //
    CEditUI::CEditUI()
        : m_pWindow(NULL)
        , m_uMaxChar(255)
        , m_bReadOnly(false)
        , m_bPasswordMode(false)
        , m_bAutoComplete(false)
        , m_cPasswordChar(_T('*'))
        , m_uButtonState(0)
        , m_dwEditbkColor(0xFFFFFFFF)
        , m_iWindowStyls(0)
    {
        SetTextPadding(CDuiRect(4, 3, 4, 3));
        SetBkColor(0xFFFFFFFF);
    }

    LPCTSTR CEditUI::GetClass() const
    {
        return _T("EditUI");
    }

    LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_EDIT) == 0)
            return static_cast<CEditUI*>(this);

        return CLabelUI::GetInterface(pstrName);
    }

    UINT CEditUI::GetControlFlags() const
    {
        if (!IsEnabled())
            return CControlUI::GetControlFlags();

        return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
    }

    void CEditUI::SetCursor(LPCTSTR pStrCursor, bool bModify)
    {
        m_hCursor = LoadCursor(NULL, IDC_IBEAM);

        // 如果xml配置中有cursor属性，则覆盖虚函数SetCursor设置的鼠标光标
        if (bModify)
        {
            CControlUI::SetCursor(pStrCursor);
        }
    }

    void CEditUI::DoEvent(TEventUI& event)
    {
        if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
        {
            if (m_pParent != NULL)
                m_pParent->DoEvent(event);
            else
                CLabelUI::DoEvent(event);

            return;
        }

        if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
        {
            ::SetCursor(m_hCursor);
            return;
        }

        if (event.Type == UIEVENT_WINDOWSIZE)
        {
            if (m_pWindow != NULL)
                m_pManager->SetFocusNeeded(this);
        }

        if (event.Type == UIEVENT_SCROLLWHEEL)
        {
            if (m_pWindow != NULL)
                return;
        }

        if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
        {
            if (m_pWindow) return;
            m_pWindow = new CEditWnd();
            ASSERT(m_pWindow);
            m_pWindow->Init(this);
            Invalidate();
        }

        if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
        {
            Invalidate();
        }

        if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
        {
            if (IsEnabled())
            {
                GetManager()->ReleaseCapture();
                if (IsFocused() && m_pWindow == NULL)
                {
                    m_pWindow = new CEditWnd();
                    ASSERT(m_pWindow);
                    m_pWindow->Init(this);

                    if (PtInRect(&m_rcItem, event.ptMouse))
                    {
                        int nSize = GetWindowTextLength(*m_pWindow);
                        if (nSize == 0)
                            nSize = 1;

                        Edit_SetSel(*m_pWindow, 0, nSize);
                    }
                }
                else if (m_pWindow != NULL)
                {
#if 1
                    int nSize = GetWindowTextLength(*m_pWindow);
                    if (nSize == 0)
                        nSize = 1;

                    Edit_SetSel(*m_pWindow, 0, nSize);
#else
                    POINT pt = event.ptMouse;
                    pt.x -= m_rcItem.left + m_rcTextPadding.left;
                    pt.y -= m_rcItem.top + m_rcTextPadding.top;
                    ::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
                }
            }
            return;
        }

        if (event.Type == UIEVENT_MOUSEMOVE)
        {
            return;
        }

        if (event.Type == UIEVENT_BUTTONUP)
        {
            return;
        }

        if (event.Type == UIEVENT_CONTEXTMENU)
        {
            return;
        }

        if (event.Type == UIEVENT_MOUSEENTER)
        {
            if (IsEnabled())
            {
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
            }
            return;
        }

        if (event.Type == UIEVENT_MOUSELEAVE)
        {
            if (IsEnabled())
            {
                m_uButtonState &= ~UISTATE_HOT;
                Invalidate();
            }
            return;
        }

        CLabelUI::DoEvent(event);
    }

    void CEditUI::SetEnabled(bool bEnable)
    {
        CControlUI::SetEnabled(bEnable);

        if (!IsEnabled())
        {
            m_uButtonState = 0;
        }
    }

    void CEditUI::SetText(LPCTSTR pstrText)
    {
        m_sText = pstrText;

        if (m_pWindow != NULL)
            Edit_SetText(*m_pWindow, m_sText);

        Invalidate();
    }

    void CEditUI::SetMaxChar(UINT uMax)
    {
        m_uMaxChar = uMax;
        if (m_pWindow != NULL)
            Edit_LimitText(*m_pWindow, m_uMaxChar);
    }

    UINT CEditUI::GetMaxChar()
    {
        return m_uMaxChar;
    }

    void CEditUI::SetReadOnly(bool bReadOnly)
    {
        if (m_bReadOnly == bReadOnly)
            return;

        m_bReadOnly = bReadOnly;
        if (m_pWindow != NULL)
            Edit_SetReadOnly(*m_pWindow, m_bReadOnly);

        Invalidate();
    }

    bool CEditUI::IsReadOnly() const
    {
        return m_bReadOnly;
    }

    void CEditUI::SetNumberOnly(bool bNumberOnly)
    {
        if (bNumberOnly)
        {
            m_iWindowStyls |= ES_NUMBER;
        }
        else
        {
            m_iWindowStyls |= ~ES_NUMBER;
        }
    }

    bool CEditUI::IsNumberOnly() const
    {
        return m_iWindowStyls & ES_NUMBER ? true : false;
    }

    int CEditUI::GetWindowStyls() const
    {
        return m_iWindowStyls;
    }

    void CEditUI::SetPasswordMode(bool bPasswordMode)
    {
        if (m_bPasswordMode == bPasswordMode)
            return;

        m_bPasswordMode = bPasswordMode;
        Invalidate();
    }

    bool CEditUI::IsPasswordMode() const
    {
        return m_bPasswordMode;
    }

    void CEditUI::SetAutoComplete(bool bAutoComplte)
    {
        m_bAutoComplete = bAutoComplte;
    }

    bool CEditUI::IsAutoComplete()
    {
        return m_bAutoComplete;
    }

    void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
    {
        if (m_cPasswordChar == cPasswordChar)
            return;

        m_cPasswordChar = cPasswordChar;

        if (m_pWindow != NULL)
            Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);

        Invalidate();
    }

    TCHAR CEditUI::GetPasswordChar() const
    {
        return m_cPasswordChar;
    }

    LPCTSTR CEditUI::GetNormalImage()
    {
        return m_sNormalImage;
    }

    void CEditUI::SetNormalImage(LPCTSTR pStrImage)
    {
        m_sNormalImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CEditUI::GetHotImage()
    {
        return m_sHotImage;
    }

    void CEditUI::SetHotImage(LPCTSTR pStrImage)
    {
        m_sHotImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CEditUI::GetFocusedImage()
    {
        return m_sFocusedImage;
    }

    void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
    {
        m_sFocusedImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CEditUI::GetDisabledImage()
    {
        return m_sDisabledImage;
    }

    void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
    {
        m_sDisabledImage = pStrImage;
        Invalidate();
    }

    void CEditUI::SetNativeEditBkColor(DWORD dwBkColor)
    {
        m_dwEditbkColor = dwBkColor;
    }

    DWORD CEditUI::GetNativeEditBkColor() const
    {
        return m_dwEditbkColor;
    }

    void CEditUI::SetSel(long nStartChar, long nEndChar)
    {
        if (m_pWindow != NULL)
            Edit_SetSel(*m_pWindow, nStartChar, nEndChar);
    }

    void CEditUI::SetSelAll()
    {
        SetSel(0, -1);
    }

    void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
    {
        if (m_pWindow != NULL)
            Edit_ReplaceSel(*m_pWindow, lpszReplace);
    }

    void CEditUI::SetPos(RECT rc)
    {
        CControlUI::SetPos(rc);

        if (m_pWindow != NULL)
        {
            RECT rcPos = m_pWindow->CalPos();
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    void CEditUI::SetVisible(bool bVisible)
    {
        CControlUI::SetVisible(bVisible);

        if (!IsVisible() && m_pWindow != NULL)
            m_pManager->SetFocus(NULL);
    }

    void CEditUI::SetInternVisible(bool bVisible)
    {
        if (!IsVisible() && m_pWindow != NULL)
            m_pManager->SetFocus(NULL);
    }

    SIZE CEditUI::EstimateSize(SIZE szAvailable)
    {
        if (m_cxyFixed.cy == 0)
            return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);

        return CControlUI::EstimateSize(szAvailable);
    }

    void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (lstrcmpi(pstrName, _T("readonly")) == 0)
        {
            SetReadOnly(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("numberonly")) == 0)
        {
            SetNumberOnly(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("password")) == 0)
        {
            SetPasswordMode(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("autocomplete")) == 0)
        {
            SetAutoComplete(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("maxchar")) == 0)
        {
            SetMaxChar(_ttoi(pstrValue));
        }
        else if (lstrcmpi(pstrName, _T("normalimage")) == 0)
        {
            SetNormalImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("hotimage")) == 0)
        {
            SetHotImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("focusedimage")) == 0)
        {
            SetFocusedImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("disabledimage")) == 0)
        {
            SetDisabledImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("nativebkcolor")) == 0)
        {
            DWORD dwColor = 0;
            if (ParseColor(pstrValue, &dwColor))
                SetNativeEditBkColor(dwColor);
        }
        else
        {
            CLabelUI::SetAttribute(pstrName, pstrValue);
        }
    }

    void CEditUI::PaintStatusImage(HDC hDC)
    {
        if (IsFocused())
            m_uButtonState |= UISTATE_FOCUSED;
        else
            m_uButtonState &= ~UISTATE_FOCUSED;

        if (!IsEnabled())
            m_uButtonState |= UISTATE_DISABLED;
        else
            m_uButtonState &= ~UISTATE_DISABLED;

        if ((m_uButtonState & UISTATE_DISABLED) != 0)
        {
            if (!m_sDisabledImage.IsEmpty())
            {
                if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage))
                    m_sDisabledImage.Empty();
                else
                    return;
            }
        }
        else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
        {
            if (!m_sFocusedImage.IsEmpty())
            {
                if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage))
                    m_sFocusedImage.Empty();
                else
                    return;
            }
        }
        else if ((m_uButtonState & UISTATE_HOT) != 0)
        {
            if (!m_sHotImage.IsEmpty())
            {
                if (!DrawImage(hDC, (LPCTSTR)m_sHotImage))
                    m_sHotImage.Empty();
                else
                    return;
            }
        }

        if (!m_sNormalImage.IsEmpty())
        {
            if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage))
                m_sNormalImage.Empty();
            else
                return;
        }
    }

    void CEditUI::PaintText(HDC hDC)
    {
        if (m_dwTextColor == 0)
            m_dwTextColor = m_pManager->GetDefaultFontColor();

        if (m_dwDisabledTextColor == 0)
            m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

        if (m_sText.IsEmpty())
            return;

        CDuiString sText = m_sText;

        if (m_bPasswordMode)
        {
            sText.Empty();
            LPCTSTR p = m_sText.GetData();

            while (*p != _T('\0'))
            {
                sText += m_cPasswordChar;
                p = ::CharNext(p);
            }
        }

        RECT rc = m_rcItem;
        rc.left += m_rcTextPadding.left;
        rc.right -= m_rcTextPadding.right;
        rc.top += m_rcTextPadding.top;
        rc.bottom -= m_rcTextPadding.bottom;

        if (IsEnabled())
        {
            if (m_pManager->IsBackgroundTransparent())
            {
                CRenderEngine::DrawTextEx(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
            }
            else
            {
                CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
            }
        }
        else
        {
            if (m_pManager->IsBackgroundTransparent())
            {
                CRenderEngine::DrawTextEx(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
            }
            else
            {
                CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
            }
        }
    }
}

#include "StdAfx.h"
#include "UIMenu.h"

namespace DuiLib {

    /////////////////////////////////////////////////////////////////////////////////////
    //
    CContextMenuObServerHwnd gContextMenuObServer;


    CMenuUI::CMenuUI()
    {
        if (GetHeader() != NULL)
            GetHeader()->SetVisible(false);
    }

    CMenuUI::~CMenuUI()
    {
        //DUITRACE(TEXT("~CMenuUI"));
    }

    LPCTSTR CMenuUI::GetClass() const
    {
        return _T("MenuUI");
    }

    LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_MENU) == 0)
            return static_cast<CMenuUI*>(this);

        return CListUI::GetInterface(pstrName);
    }

    void CMenuUI::DoEvent(TEventUI& event)
    {
        return __super::DoEvent(event);
    }

    bool CMenuUI::Add(CControlUI* pControl)
    {
        CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
        if (pMenuItem == NULL)
            return false;

        int count = pMenuItem->GetCount();
        for (int i = 0; i < count; ++i)
        {
            CControlUI *pControl = pMenuItem->GetItemAt(i);
            if (pControl != NULL)
            {
                CMenuItemUI *pSubMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
                if (pSubMenuItem != NULL)
                {
                    pSubMenuItem->SetInternVisible(false);
                }
            }
        }

        return CListUI::Add(pControl);
    }

    bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)
    {
        CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
        if (pMenuItem == NULL)
            return false;

        int count = pMenuItem->GetCount();
        for (int i = 0; i < count; ++i)
        {
            CControlUI *pControl = pMenuItem->GetItemAt(i);
            if (pControl != NULL)
            {
                CMenuItemUI *pSubMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
                if (pSubMenuItem != NULL)
                {
                    pSubMenuItem->SetInternVisible(false);
                }
            }
        }

        return CListUI::AddAt(pControl, iIndex);
    }

    int CMenuUI::GetItemIndex(CControlUI* pControl) const
    {
        CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
        if (pMenuItem == NULL)
            return -1;

        return __super::GetItemIndex(pControl);
    }

    bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)
    {
        CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
        if (pMenuItem == NULL)
            return false;

        return __super::SetItemIndex(pControl, iIndex);
    }

    bool CMenuUI::Remove(CControlUI* pControl)
    {
        CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
        if (pMenuItem == NULL)
            return false;

        return __super::Remove(pControl);
    }

    SIZE CMenuUI::EstimateSize(SIZE szAvailable)
    {
        int cxFixed = 0;
        int cyFixed = 0;

        int count = GetCount();
        for (int i = 0; i < count; i++)
        {
            CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(i));

            ASSERT(pControl != NULL);

            if (!pControl->IsVisible())
                continue;

            SIZE sz = pControl->EstimateSize(szAvailable);
            cyFixed += sz.cy;

            if (cxFixed < sz.cx)
                cxFixed = sz.cx;
        }

        //  cyFixed += m_rcInset.bottom;
        //  cyFixed += m_rcInset.top;
        //  cxFixed += m_rcInset.right;
        //  cxFixed += m_rcInset.left;

        return CSize(cxFixed, cyFixed);
    }

    void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        CListUI::SetAttribute(pstrName, pstrValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // 用于创建菜单控件、菜单项控件的回调函数，有了这个，不用在UIDlgBuilder.cpp中加new了！
    class CMenuBuilderCallback : public IDialogBuilderCallback
    {
        CControlUI* CreateControl(LPCTSTR pstrClass)
        {
            if (lstrcmpi(pstrClass, DUI_CTR_MENU) == 0)
            {
                return new CMenuUI();
            }
            else if (lstrcmpi(pstrClass, DUI_CTR_MENUITEM) == 0)
            {
                return new CMenuItemUI();
            }

            return NULL;
        }
    };

    CMenuWnd::CMenuWnd(HWND hParent)
        : m_hParent(hParent)
        , m_pOwner(NULL)
        , m_pLayout()
        , m_xml(_T(""))
    {

    }

    BOOL CMenuWnd::Receive(ContextMenuParam param)
    {
        switch (param.wParam)
        {
        case 1:
            Close();
            break;
        case 2:
            {
                HWND hParent = GetParent(m_hWnd);
                while (hParent != NULL)
                {
                    if (hParent == param.hWnd)
                    {
                        Close();
                        break;
                    }
                    hParent = GetParent(hParent);
                }
            }
            break;
        default:
            break;
        }

        return TRUE;
    }

    void CMenuWnd::Notify(TNotifyUI& msg)
    {
        if (msg.sType == _T("itemclick"))
        {
            DUITRACE(_T("%s Click"), msg.pSender->GetText());

            // 发送消息到主窗口
            if (!msg.pSender->GetName().IsEmpty())
            {
                // new出来的，记得要delete
                CDuiString *pMenuName = new CDuiString(msg.pSender->GetName());
                ::PostMessage(gContextMenuObServer.GetMainHwnd(), WM_CUSTOMMENU_CLICK, (WPARAM)pMenuName, 0);
            }
        }
    }


    void CMenuWnd::Init(CMenuItemUI* pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT point)
    {
        m_BasedPoint = point;
        m_pOwner = pOwner;
        m_pLayout = NULL;

        if (pSkinType != NULL)
            m_sType = pSkinType;

        m_xml = xml;

        // 主窗口调用时的init，pOwner一般都是空，此时设置接收点击菜单项的窗口
        if (m_pOwner == NULL)
        {
            gContextMenuObServer.SetMainHwnd(m_hParent);
        }

        gContextMenuObServer.AddReceiver(this);

        //  创建菜单窗口
        Create((m_pOwner == NULL) ? m_hParent : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());

        ::ShowWindow(m_hWnd, SW_SHOW);

        // HACK: Don't deselect the parent's caption
        /*HWND hWndParent = m_hWnd;
        while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
        ::ShowWindow(m_hWnd, SW_SHOW);
        #if defined(WIN32) && !defined(UNDER_CE)
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
        #endif*/
    }

    LPCTSTR CMenuWnd::GetWindowClassName() const
    {
        return _T("UIMenuWindow");
    }

    void CMenuWnd::OnFinalMessage(HWND hWnd)
    {
        RemoveObserver();

        if (m_pOwner != NULL)
        {
            int count = m_pOwner->GetCount();
            for (int i = 0; i < count; i++)
            {
                CControlUI *pControl = m_pOwner->GetItemAt(i);
                if (pControl != NULL)
                {
                    CMenuItemUI *pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
                    if (pMenuItem != NULL)
                    {
                        pMenuItem->SetOwner(m_pOwner->GetParent());
                        pMenuItem->SetVisible(false);
                        pMenuItem->SetInternVisible(false);
                    }
                }
            }

            m_pOwner->m_pWindow = NULL;
            m_pOwner->m_uButtonState &= ~UISTATE_PUSHED;
            m_pOwner->Invalidate();
        }

        delete this;
    }

    BOOL CMenuWnd::IsRootMenu()
    {
        return (m_pOwner == NULL);
    }

    BOOL CMenuWnd::GetCurrentMonitorWorkRect(LPRECT Rect)
    {
#if defined(WIN32) && !defined(UNDER_CE)
        MONITORINFO oMonitor = { 0 };
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        *Rect = oMonitor.rcWork;
#else
        GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), Rect);
#endif
        return TRUE;
    }

    LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {
            if (!IsRootMenu())
            {
                // 去掉窗口标题栏
                LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
                styleValue &= ~WS_CAPTION;
                ::SetWindowLong(m_hWnd, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
                RECT rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                ::SetWindowPos(m_hWnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
                    rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

                m_pm.Init(m_hWnd);
                m_pm.SetRoundCorner(1, 1);

                // The trick is to add the items to the new container. Their owner gets
                // reassigned by this operation - which is why it is important to reassign
                // the items back to the righfull owner/manager when the window closes.
                // 创建一个新的CMenuUI..要设置其属性.一般菜单的样式要写在Default里面
                // 子菜单不用再解析xml了，可直接使用之前已经解析过的xml
                m_pLayout = new CMenuUI();
                m_pm.UseParentResource(m_pOwner->GetManager());
                m_pLayout->SetManager(&m_pm, NULL, true);
                LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(DUI_CTR_MENU);
                if (pDefaultAttributes != NULL)
                {
                    m_pLayout->ApplyAttributeList(pDefaultAttributes);
                }

                /*m_pLayout->SetBkColor(0xFFFFFFFF);
                m_pLayout->SetBorderColor(0xFF85E4FF);
                m_pLayout->SetBorderSize(0);
                m_pLayout->SetAutoDestroy(false);
                m_pLayout->EnableScrollBar();*/
                m_pLayout->SetAutoDestroy(false);

                // 将菜单项加载进layout中去
                int count = m_pOwner->GetCount();
                for (int i = 0; i < count; i++)
                {
                    CControlUI *pControl = m_pOwner->GetItemAt(i);
                    if (pControl != NULL)
                    {
                        CMenuItemUI *pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
                        if (pMenuItem != NULL)
                            pMenuItem->SetOwner(m_pLayout);

                        m_pLayout->Add(pControl);
                    }
                }

                m_pm.AttachDialog(m_pLayout);
                m_pm.AddNotifier(this);

                // Position the popup window in absolute space
                CDuiRect rcOwner = m_pOwner->GetPos();

                // 因为continer已经减去的内边距我们这里还的加上.
                rcOwner.top -= m_pLayout->GetInset().top;
                rcOwner.bottom += m_pLayout->GetInset().bottom;
                rcOwner.left -= m_pLayout->GetInset().left;
                rcOwner.right += m_pLayout->GetInset().right;

                int cxFixed = 0;
                int cyFixed = 0;

                CDuiRect rcWork;
                GetCurrentMonitorWorkRect(&rcWork);

                SIZE szAvailable = { rcWork.GetWidth(), rcWork.GetHeight() };

                // 获取所有菜单项的高度和和最大宽度
                /*for( int it = 0; it < m_pOwner->GetCount(); it++ ) {
                    if(m_pOwner->GetItemAt(it)->GetInterface(kMenuElementUIInterfaceName) != NULL ){
                    CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
                    SIZE sz = pControl->EstimateSize(szAvailable);
                    cyFixed += sz.cy;

                    if( cxFixed < sz.cx )
                    cxFixed = sz.cx;
                    }
                    }*/

                CSize szAvailable1 = m_pLayout->EstimateSize(szAvailable);
                cyFixed = szAvailable1.cy;
                cxFixed = szAvailable1.cx;

                // 加上内边距
                cyFixed += m_pLayout->GetInset().bottom;
                cyFixed += m_pLayout->GetInset().top;
                cxFixed += m_pLayout->GetInset().left;
                cxFixed += m_pLayout->GetInset().right;

                //cyFixed += 4;
                //cxFixed += 4;

                RECT rcWindow;
                GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

                // 子菜单的top和父菜单相同
                CDuiRect rc = rcOwner;
                rc.top = rcOwner.top;
                rc.bottom = rc.top + cyFixed;

                ::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

                // 设置子菜单的左侧位于父菜单的右侧
                rc.left = rcWindow.right;
                rc.right = rc.left + cxFixed;
                rc.right += 2;

                // 超出显示器的底部。向上移动
                if (rc.bottom > rcWork.bottom)
                {
                    rc.Offset(0, -(rc.GetHeight() - rcOwner.GetHeight()));
                }

                // 超出显示器的右边。向走移动
                if (rc.right > rcWork.right)
                {
                    rc.Offset(-(rcOwner.GetWidth() + rc.GetWidth()), 0);
                }

                // 超出显示器顶部，向下移动
                if (rc.top < rcWork.top)
                {
                    rc.top = rcOwner.top;
                    rc.bottom = rc.top + cyFixed;
                }

                // 超出显示器左边，向右移动
                if (rc.left < rcWork.left)
                {
                    rc.left = rcWindow.right;
                    rc.right = rc.left + cxFixed;
                }

                MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
            }
            else
            {
                // 根菜单创建
                m_pm.Init(m_hWnd);
                m_pm.SetRoundCorner(1, 1);

                CDialogBuilder builder;
                CMenuBuilderCallback menuCallback;

                CControlUI* pRoot = builder.Create(m_xml, m_sType.GetData(), &menuCallback, &m_pm);
                m_pm.AttachDialog(pRoot);
                m_pm.AddNotifier(this);

                CDuiRect rcWork;
                GetCurrentMonitorWorkRect(&rcWork);

                SIZE szAvailable = { rcWork.GetWidth(), rcWork.GetHeight() };
                szAvailable = pRoot->EstimateSize(szAvailable);

                m_pLayout = static_cast<CMenuUI*>(pRoot);
                szAvailable.cy += m_pLayout->GetInset().bottom;
                szAvailable.cy += m_pLayout->GetInset().top;
                szAvailable.cx += m_pLayout->GetInset().left;
                szAvailable.cx += m_pLayout->GetInset().right;

                m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

                SIZE szInit = m_pm.GetInitSize();
                CDuiRect rc;

                rc.left = m_BasedPoint.x;
                rc.top = m_BasedPoint.y;
                rc.right = rc.left + szInit.cx;
                rc.bottom = rc.top + szInit.cy;

                /*
                DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
                int nWidth = rc.GetWidth();
                int nHeight = rc.GetHeight();

                if (dwAlignment & eMenuAlignment_Right)
                {
                rc.right = point.x;
                rc.left = rc.right - nWidth;
                }

                if (dwAlignment & eMenuAlignment_Bottom)
                {
                rc.bottom = point.y;
                rc.top = rc.bottom - nHeight;
                }*/

                SetForegroundWindow(m_hWnd);
                MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
                SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_SHOWWINDOW);
            }

            return 0;
        }
        else if (uMsg == WM_CLOSE)
        {
            if (m_pOwner != NULL)
            {
                // 父菜单获取焦点
                m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
                m_pOwner->SetPos(m_pOwner->GetPos());
                m_pOwner->SetFocus();
            }
        }
        else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_CONTEXTMENU || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK)
        {
            return 0L;
        }
        else if (uMsg == WM_KILLFOCUS)
        {
            HWND hWndFocusd = (HWND)wParam;
            HWND hWndShadow = m_pm.GetShadowWindow();

            // 获得焦点的窗口如果是阴影窗口（或NULL）时不算，此时不销毁菜单窗口！
            if (hWndShadow == NULL || hWndShadow != hWndFocusd)
            {
                BOOL bInMenuWindowList = FALSE;
                ContextMenuParam param;
                param.hWnd = GetHWND();

                ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(gContextMenuObServer);
                ReceiverImplBase<BOOL, ContextMenuParam>* pReceiver = iterator.next();
                while (pReceiver != NULL)
                {
                    CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
                    if (pContextMenu != NULL && pContextMenu->GetHWND() == hWndFocusd)
                    {
                        bInMenuWindowList = TRUE;
                        break;
                    }
                    pReceiver = iterator.next();
                }

                if (!bInMenuWindowList)
                {
                    param.wParam = 1;
                    gContextMenuObServer.RBroadcast(param);
                    return 0;
                }
            }
        }
        else if (uMsg == WM_KEYDOWN)
        {
            if (wParam == VK_ESCAPE)
            {
                Close();
            }
        }
        else if (uMsg == WM_SIZE)
        {
            SIZE szRoundCorner = m_pm.GetRoundCorner();
            if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0))
            {
                CDuiRect rcWnd;
                ::GetWindowRect(*this, &rcWnd);
                rcWnd.Offset(-rcWnd.left, -rcWnd.top);
                rcWnd.right++; rcWnd.bottom++;
                HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
                ::SetWindowRgn(*this, hRgn, TRUE);
                ::DeleteObject(hRgn);
            }
        }

        LRESULT lRes = 0;
        if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
            return lRes;

        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    CMenuItemUI::CMenuItemUI()
        : m_pWindow(NULL)
        , m_bHasSubMenu(FALSE)
        , m_RightWidth(10)
        , m_LeftWidth(0)
        , m_bIsSeparator(FALSE)
        , m_nSeparatorHeight(10)
    {
        //m_bMouseChildEnabled = true;
        //SetMouseChildEnabled(false);
    }

    CMenuItemUI::~CMenuItemUI()
    {

    }

    LPCTSTR CMenuItemUI::GetClass() const
    {
        return _T("MenuItemUI");
    }

    LPVOID CMenuItemUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_MENUITEM) == 0)
            return static_cast<CMenuItemUI*>(this);

        return CListContainerElementUI::GetInterface(pstrName);
    }

    void CMenuItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
    {
        if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem))
            return;

        if (m_cxyBorderRound.cx > 0 && m_cxyBorderRound.cy > 0)
        {
            CRenderClip roundClip;
            CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip); //绘制圆角
        }

        int nItemHeight = m_rcItem.bottom - m_rcItem.top;

        // 绘制左边背景
        if (m_LeftWidth)
        {
            DWORD dwBkColor1 = 0xFF7FAEE4;
            DWORD dwBkColor2 = 0xFFFFFFFF;

            RECT LeftBkRect = m_rcItem;
            LeftBkRect.left = 0;
            LeftBkRect.right = min(m_LeftWidth, 20);

          //  CRenderEngine::DrawGradient(hDC, LeftBkRect, GetAdjustColor(dwBkColor1), GetAdjustColor(dwBkColor2), false, 16);
        }

        if (m_bIsSeparator)
        {
            // 直接画一条线.
            //CDuiRect LineRect = m_rcItem;

            //LineRect.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - 1) / 2;
            //LineRect.bottom = LineRect.top;;

            //CRenderEngine::DrawLine(hDC, LineRect, 1, 0xFFB5B5B5, PS_SOLID);

            DWORD dwBkColor1 = 0x00FFFFFF;
            DWORD dwBkColor2 = 0xFF000000;

            CDuiRect rcItemNew = m_rcItem;

            rcItemNew.left += min(m_LeftWidth, 10);
            rcItemNew.right -= min(m_RightWidth, 10);

            CDuiRect LineRect = rcItemNew;
            LineRect.top = rcItemNew.top + (rcItemNew.bottom - rcItemNew.top - 1) / 2;
            LineRect.bottom = LineRect.top + 1;

            LineRect.right = rcItemNew.left + (rcItemNew.right - rcItemNew.left) / 2;

            CRenderEngine::DrawGradient(hDC, LineRect, GetAdjustColor(dwBkColor1), GetAdjustColor(dwBkColor2), false, 16);

            LineRect.left = LineRect.right;
            LineRect.right = rcItemNew.right;

            CRenderEngine::DrawGradient(hDC, LineRect, GetAdjustColor(dwBkColor2), GetAdjustColor(dwBkColor1), false, 16);

            return;
        }

        // 绘制背景
        CMenuItemUI::DrawItemBk(hDC, m_rcItem);

        // 绘制左边的图标.
        if (!m_strLeftIcon.IsEmpty())
        {
            CDuiString strModify;
            CDuiRect destRect(0, 0, m_LeftWidth, nItemHeight);

            // 调整icon的位置
            if (m_LeftWidth > m_LeftIconSize.cx)
            {
                destRect.left = (m_LeftWidth - m_LeftIconSize.cx) / 2;
                destRect.right = destRect.left + m_LeftIconSize.cx;
            }

            if (nItemHeight > m_LeftIconSize.cy)
            {
                destRect.top += (nItemHeight - m_LeftIconSize.cy) / 2;
                destRect.bottom = destRect.top + m_LeftIconSize.cy;
            }

            strModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), destRect.left, destRect.top, destRect.right, destRect.bottom);
            DrawImage(hDC, (LPCTSTR)m_strLeftIcon, (LPCTSTR)strModify);
        }

        // 绘制右侧箭头
        if (m_bHasSubMenu && !m_strRightArrow.IsEmpty())
        {
            CDuiString strModify;
            CDuiRect destRect(m_rcItem.right - m_RightWidth, 0, m_rcItem.right, nItemHeight);

            // 调整icon的位置
            if (m_RightWidth > m_RightArrowSize.cx)
            {
                destRect.left += (m_RightWidth - m_RightArrowSize.cx) / 2;
                destRect.right = destRect.left + m_RightArrowSize.cx;
            }

            if (nItemHeight > m_RightArrowSize.cy)
            {
                destRect.top += (nItemHeight - m_RightArrowSize.cy) / 2;
                destRect.bottom = destRect.top + m_RightArrowSize.cy;
            }

            strModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), destRect.left, destRect.top, destRect.right, destRect.bottom);
            DrawImage(hDC, (LPCTSTR)m_strRightArrow, (LPCTSTR)strModify);
        }

        // 绘制文本
        CDuiRect textRect = m_rcItem;

        textRect.left += m_LeftWidth;
        textRect.right -= m_RightWidth;

        DrawItemText(hDC, textRect);

        // 子控件的绘制
        int count = GetCount();
        for (int i = 0; i < count; ++i)
        {
            CControlUI *pControl = GetItemAt(i);
            if (pControl != NULL && pControl->GetInterface(DUI_CTR_MENUITEM) == NULL)
                pControl->DoPaint(hDC, rcPaint);
        }
    }

    void CMenuItemUI::SetLeftIconImage(LPCTSTR lpszLeftIcon)
    {
        m_strLeftIcon = lpszLeftIcon;
    }

    void CMenuItemUI::SetRightArrowImage(LPCTSTR lpszIcon)
    {
        m_strRightArrow = lpszIcon;
    }

    void CMenuItemUI::SetRightArrowSize(SIZE Size)
    {
        m_RightArrowSize = Size;
    }

    void CMenuItemUI::SetLeftIconSize(SIZE Size)
    {
        m_LeftIconSize = Size;
    }

    void CMenuItemUI::SetLeftWidth(int nWidth)
    {
        m_LeftWidth = nWidth;
    }

    void CMenuItemUI::SetRightWidth(int nWidth)
    {
        m_RightWidth = nWidth;
    }

    void CMenuItemUI::SetIsSeparator(bool bSeparator)
    {
        m_bIsSeparator = bSeparator;
        SetEnabled(false);
    }

    void CMenuItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (lstrcmpi(pstrName, _T("left")) == 0)
        {
            SetLeftWidth(_ttoi(pstrValue));
        }
        else if (lstrcmpi(pstrName, _T("right")) == 0)
        {
            SetRightWidth(_ttoi(pstrValue));
        }
        else if (lstrcmpi(pstrName, _T("icon")) == 0)
        {
            SetLeftIconImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("iconsize")) == 0)
        {
            SIZE szXY = { 0 };
            LPTSTR pstr = NULL;
            szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
            szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            SetLeftIconSize(szXY);
        }
        else if (lstrcmpi(pstrName, _T("arrowicon")) == 0)
        {
            SetRightArrowImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("separator")) == 0)
        {
            SetIsSeparator(_tcscmp(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("arrowsize")) == 0)
        {
            SIZE szXY = { 0 };
            LPTSTR pstr = NULL;
            szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
            szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            SetRightArrowSize(szXY);
        }
        else
        {
            CListContainerElementUI::SetAttribute(pstrName, pstrValue);
        }
    }

    void CMenuItemUI::DrawItemText(HDC hDC, const RECT& rcItem)
    {
        if (m_sText.IsEmpty())
            return;

        if (m_pOwner == NULL)
            return;

        TListInfoUI* pInfo = m_pOwner->GetListInfo();
        DWORD iTextColor = pInfo->dwTextColor;

        // 热点文本颜色
        if ((m_uButtonState & UISTATE_HOT) != 0)
        {
            iTextColor = pInfo->dwHotTextColor;
        }

        // 选中文本颜色
        if (IsSelected())
        {
            iTextColor = pInfo->dwSelectedTextColor;
        }

        // 禁用文本颜色
        if (!IsEnabled())
        {
            iTextColor = pInfo->dwDisabledTextColor;
        }

        int nLinks = 0;

        // 计算绘制的位置
        RECT rcText = rcItem;
        rcText.left += pInfo->rcTextPadding.left;
        rcText.right -= pInfo->rcTextPadding.right;
        rcText.top += pInfo->rcTextPadding.top;
        rcText.bottom -= pInfo->rcTextPadding.bottom;

        // 绘制文字
        if (pInfo->bShowHtml)
        {
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor,
                NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
        }
        else
        {
            CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor,
                pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
        }
    }


    SIZE CMenuItemUI::EstimateSize(SIZE szAvailable)
    {
        SIZE cXY = { 0 };

        // 获取子控件的最大宽度,并计算所有子控件的高度和
        int count = GetCount();
        for (int i = 0; i < count; i++)
        {
            CControlUI* pControl = GetItemAt(i);

            ASSERT(pControl != NULL);

            if (!pControl->IsVisible())
                continue;

            SIZE sz = pControl->EstimateSize(szAvailable);
            cXY.cy += sz.cy;

            if (cXY.cx < sz.cx)
                cXY.cx = sz.cx;
        }

        if (m_bIsSeparator)
        {
            cXY.cy = m_nSeparatorHeight;
            return cXY;
        }

        // 高度没有设置则,根据文本信息来获取菜单项的高度
        if (cXY.cy == 0)
        {
            TListInfoUI* pInfo = m_pOwner->GetListInfo();

            // 设置绘制颜色
            DWORD iTextColor = pInfo->dwTextColor;
            if ((m_uButtonState & UISTATE_HOT) != 0)
            {
                iTextColor = pInfo->dwHotTextColor;
            }

            if (IsSelected())
            {
                iTextColor = pInfo->dwSelectedTextColor;
            }

            if (!IsEnabled())
            {
                iTextColor = pInfo->dwDisabledTextColor;
            }

            RECT rcText = { 0, 0, max(szAvailable.cx, m_cxyFixed.cx), 9999 };
            rcText.left += pInfo->rcTextPadding.left;

            // 加上leftSize
            rcText.left += m_LeftWidth;

            rcText.right -= pInfo->rcTextPadding.right;

            // 去掉右边的长度.
            rcText.right -= m_RightWidth;

            // 通过DT_CALCRECT获取绘制宽度/高度
            if (pInfo->bShowHtml)
            {
                int nLinks = 0;
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, NULL, NULL, nLinks, DT_CALCRECT | pInfo->uTextStyle);
            }
            else
            {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
            }

            // 设置高度宽度
            cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + m_LeftWidth + m_RightWidth;
            cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        }

        if (cXY.cy < m_RightArrowSize.cy)
        {
            cXY.cy = m_RightArrowSize.cy;
        }

        if (cXY.cy < m_LeftIconSize.cy)
        {
            cXY.cy = m_LeftIconSize.cy;
        }

        // 如果有固定高度则使用固定高度
        if (m_cxyFixed.cy != 0)
            cXY.cy = m_cxyFixed.cy;

        return cXY;
    }

    void CMenuItemUI::DoInit()
    {
        // 控件加载完成.判断其是否有子节点
        int count = GetCount();
        for (int i = 0; i < count; ++i)
        {
            // 将MenuElement中的子控件都设置为可见
            CControlUI *pControl = GetItemAt(i);
            if (pControl->GetInterface(DUI_CTR_MENUITEM) != NULL)
            {
                m_bHasSubMenu = TRUE;
                break;
            }
        }

        // 从默认属性列表中加载右侧箭头的属性
        LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("MenuArrow"));
        if (pDefaultAttributes != NULL)
            ApplyAttributeList(pDefaultAttributes);
    }

    void CMenuItemUI::SetAllSubMenuItemVisible(bool bVisible)
    {
        int count = GetCount();
        for (int i = 0; i < count; ++i)
        {
            CControlUI *pControl = GetItemAt(i);
            if (pControl != NULL)
            {
                CMenuItemUI *pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
                if (pMenuItem != NULL)
                {
                    pMenuItem->SetVisible(bVisible);
                    pMenuItem->SetInternVisible(bVisible);
                }
            }
        }
    }

    void CMenuItemUI::DoEvent(TEventUI& event)
    {
        if (event.Type == UIEVENT_MOUSEENTER)
        {
            CListContainerElementUI::DoEvent(event);

            // 如果子菜单项已经创建了,则不进行处理
            if (m_pWindow != NULL)
                return;

            SetAllSubMenuItemVisible();

            // 如果有子菜单则创建之,否则通知有子菜单项的菜单项关闭子菜单
            if (m_bHasSubMenu)
            {
                m_pOwner->SelectItem(GetIndex(), true);
                CreateMenuWnd();
            }
            else
            {
                // 通知关闭平级菜单上的子菜单
                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = 2;
                gContextMenuObServer.RBroadcast(param);
                m_pOwner->SelectItem(GetIndex(), true);
            }
            return;
        }

        if (event.Type == UIEVENT_BUTTONUP)
        {
            if (IsEnabled())
            {
                CListContainerElementUI::DoEvent(event);

                if (m_pWindow != NULL)
                    return;

                SetAllSubMenuItemVisible();

                if (m_bHasSubMenu)
                {
                    CreateMenuWnd();
                }
                else
                {
                    ContextMenuParam param;
                    param.hWnd = m_pManager->GetPaintWindow();
                    param.wParam = 1;
                    gContextMenuObServer.RBroadcast(param);
                }
            }
            return;
        }

        CListContainerElementUI::DoEvent(event);
    }

    bool CMenuItemUI::Activate()
    {
        if (CListContainerElementUI::Activate() && m_bSelected)
        {
            if (m_pWindow != NULL)
                return true;

            SetAllSubMenuItemVisible();

            if (m_bHasSubMenu)
            {
                CreateMenuWnd();
            }
            else
            {
                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = 1;
                gContextMenuObServer.RBroadcast(param);
            }

            return true;
        }

        return false;
    }

    CMenuWnd* CMenuItemUI::GetMenuWnd()
    {
        return m_pWindow;
    }

    void CMenuItemUI::CreateMenuWnd()
    {
        if (m_pWindow != NULL)
            return;

        m_pWindow = new CMenuWnd(m_pManager->GetPaintWindow());
        ASSERT(m_pWindow != NULL);

        ContextMenuParam param;
        param.hWnd = m_pManager->GetPaintWindow();
        param.wParam = 2;
        gContextMenuObServer.RBroadcast(param);

        m_pWindow->Init(this, _T(""), _T(""), CPoint());
    }


} // namespace DuiLib

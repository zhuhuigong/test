﻿#include "StdAfx.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace DuiLib {

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...)
    {
#ifdef _DEBUG
        TCHAR szBuffer[300] = { 0 };
        va_list args;
        va_start(args, pstrFormat);
        ::wvnsprintf(szBuffer, lengthof(szBuffer) - 2, pstrFormat, args);
        lstrcat(szBuffer, _T("\n"));
        va_end(args);
        ::OutputDebugString(szBuffer);
#endif
    }

    LPCTSTR DUI__TraceMsg(UINT uMsg)
    {
#define MSGDEF(x) if(uMsg==x) return _T(#x)
        MSGDEF(WM_SETCURSOR);
        MSGDEF(WM_NCHITTEST);
        MSGDEF(WM_NCPAINT);
        MSGDEF(WM_PAINT);
        MSGDEF(WM_ERASEBKGND);
        MSGDEF(WM_NCMOUSEMOVE);
        MSGDEF(WM_MOUSEMOVE);
        MSGDEF(WM_MOUSELEAVE);
        MSGDEF(WM_MOUSEHOVER);
        MSGDEF(WM_NOTIFY);
        MSGDEF(WM_COMMAND);
        MSGDEF(WM_MEASUREITEM);
        MSGDEF(WM_DRAWITEM);
        MSGDEF(WM_LBUTTONDOWN);
        MSGDEF(WM_LBUTTONUP);
        MSGDEF(WM_LBUTTONDBLCLK);
        MSGDEF(WM_RBUTTONDOWN);
        MSGDEF(WM_RBUTTONUP);
        MSGDEF(WM_RBUTTONDBLCLK);
        MSGDEF(WM_SETFOCUS);
        MSGDEF(WM_KILLFOCUS);
        MSGDEF(WM_MOVE);
        MSGDEF(WM_SIZE);
        MSGDEF(WM_SIZING);
        MSGDEF(WM_MOVING);
        MSGDEF(WM_GETMINMAXINFO);
        MSGDEF(WM_CAPTURECHANGED);
        MSGDEF(WM_WINDOWPOSCHANGED);
        MSGDEF(WM_WINDOWPOSCHANGING);
        MSGDEF(WM_NCCALCSIZE);
        MSGDEF(WM_NCCREATE);
        MSGDEF(WM_NCDESTROY);
        MSGDEF(WM_TIMER);
        MSGDEF(WM_KEYDOWN);
        MSGDEF(WM_KEYUP);
        MSGDEF(WM_CHAR);
        MSGDEF(WM_SYSKEYDOWN);
        MSGDEF(WM_SYSKEYUP);
        MSGDEF(WM_SYSCOMMAND);
        MSGDEF(WM_SYSCHAR);
        MSGDEF(WM_VSCROLL);
        MSGDEF(WM_HSCROLL);
        MSGDEF(WM_CHAR);
        MSGDEF(WM_SHOWWINDOW);
        MSGDEF(WM_PARENTNOTIFY);
        MSGDEF(WM_CREATE);
        MSGDEF(WM_NCACTIVATE);
        MSGDEF(WM_ACTIVATE);
        MSGDEF(WM_ACTIVATEAPP);
        MSGDEF(WM_CLOSE);
        MSGDEF(WM_DESTROY);
        MSGDEF(WM_GETICON);
        MSGDEF(WM_GETTEXT);
        MSGDEF(WM_GETTEXTLENGTH);
        static TCHAR szMsg[10];
        ::wsprintf(szMsg, _T("0x%04X"), uMsg);
        return szMsg;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    //////////////////////////////////////////////////////////////////////////
    // 这里可以处理消息！！！
    DUI_BASE_BEGIN_MESSAGE_MAP(CNotifyPump)
    DUI_END_MESSAGE_MAP()


    static const DUI_MSGMAP_ENTRY* DuiFindMessageEntry(const DUI_MSGMAP_ENTRY* lpEntry, TNotifyUI& msg)
    {
        CDuiString sMsgType = msg.sType;
        CDuiString sCtrlName = msg.pSender->GetName();
        const DUI_MSGMAP_ENTRY* pMsgTypeEntry = NULL;

        while (lpEntry->nSig != DuiSig_end)
        {
            if (lpEntry->sMsgType == sMsgType)
            {
                if (!lpEntry->sCtrlName.IsEmpty())
                {
                    if (lpEntry->sCtrlName == sCtrlName)
                    {
                        return lpEntry;
                    }
                }
                else
                {
                    pMsgTypeEntry = lpEntry;
                }
            }
            lpEntry++;
        }

        return pMsgTypeEntry;
    }

    bool CNotifyPump::AddVirtualWnd(CDuiString strName, CNotifyPump* pObject)
    {
        if (m_VirtualWndMap.Find(strName) == NULL)
        {
            m_VirtualWndMap.Insert(strName.GetData(), (LPVOID)pObject);
            return true;
        }

        return false;
    }

    bool CNotifyPump::RemoveVirtualWnd(CDuiString strName)
    {
        if (m_VirtualWndMap.Find(strName) != NULL)
        {
            m_VirtualWndMap.Remove(strName);
            return true;
        }

        return false;
    }

    bool CNotifyPump::LoopDispatch(TNotifyUI& msg)
    {
        const DUI_MSGMAP_ENTRY* lpEntry = NULL;
        const DUI_MSGMAP* pMessageMap = NULL;
        bool bLoopDispatch = false;

#ifndef UILIB_STATIC
        for (pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
        for (pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap)
#endif
        {
#ifndef UILIB_STATIC
            ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
            ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif
            if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries, msg)) != NULL)
            {
                bLoopDispatch = true;
                break;
            }
        }

        if (!bLoopDispatch)
            return false;

        union DuiMessageMapFunctions mmf;
        mmf.pfn = lpEntry->pfn;

        bool bRet = false;
        switch (lpEntry->nSig)
        {
        case DuiSig_lwl:
            (this->*mmf.pfn_Notify_lwl)(msg.wParam, msg.lParam);
            bRet = true;
            break;
        case DuiSig_vn:
            (this->*mmf.pfn_Notify_vn)(msg);
            bRet = true;
            break;
        default:
            ASSERT(FALSE);
            break;
        }

        return bRet;
    }

    void CNotifyPump::NotifyPump(TNotifyUI& msg)
    {
        ///遍历虚拟窗口
        if (!msg.sVirtualWnd.IsEmpty())
        {
            for (int i = 0; i < m_VirtualWndMap.GetSize(); i++)
            {
                if (LPCTSTR key = m_VirtualWndMap.GetAt(i))
                {
                    if (lstrcmpi(key, msg.sVirtualWnd.GetData()) == 0)
                    {
                        CNotifyPump* pObject = static_cast<CNotifyPump*>(m_VirtualWndMap.Find(key, false));
                        if (pObject && pObject->LoopDispatch(msg))
                            return;
                    }
                }
            }
        }

        ///
        //遍历主窗口
        LoopDispatch(msg);
    }


    //////////////////////////////////////////////////////////////////////////
    /// duilib窗口基类
    CWindowWnd::CWindowWnd()
        : m_hWnd(NULL)
        , m_OldWndProc(::DefWindowProc)
        , m_bSubclassed(false)
        , m_bSupclassed(false)
    {

    }

    HWND CWindowWnd::GetHWND() const
    {
        return m_hWnd;
    }

    UINT CWindowWnd::GetClassStyle() const
    {
        return 0;
    }

    LPCTSTR CWindowWnd::GetSuperClassName() const
    {
        return NULL;
    }

    CWindowWnd::operator HWND() const
    {
        return m_hWnd;
    }

    HWND CWindowWnd::CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/)
    {
        return Create(hwndParent, pstrWindowName, dwStyle, dwExStyle, 0, 0, 0, 0, NULL);
    }

    HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
    {
        return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
    }

    HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
    {
        // 如果有超类化的类名，则优先注册超类窗口类
        if (GetSuperClassName() != NULL && !RegisterSuperclass())
        {
            return NULL;
        }

        // 没有则注册普通窗口类
        if (GetSuperClassName() == NULL && !RegisterWindowClass())
        {
            return NULL;
        }

        // 开始创建窗口
        m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, CPaintManagerUI::GetInstance(), this);
        ASSERT(m_hWnd != NULL);

        return m_hWnd;
    }

    HWND CWindowWnd::Subclass(HWND hWnd)
    {
        // 仅当没有子类化过时进行子类化
        if (!m_bSubclassed)
        {
            ASSERT(::IsWindow(hWnd));
            ASSERT(m_hWnd == NULL);

            m_hWnd = hWnd;
            ::SetProp(hWnd, SUBCLASS_WINDOW_PROP, (HANDLE) this);

            // 子类化窗口！
            m_OldWndProc = SubclassWindow(hWnd, __SubWindowProc);
            if (m_OldWndProc == NULL)
            {
                return NULL;
            }

            m_bSubclassed = true;
        }

        return m_hWnd;
    }

    void CWindowWnd::Unsubclass()
    {
        // 已经子类化过窗口时才去掉子类化
        if (m_bSubclassed)
        {
            ASSERT(::IsWindow(m_hWnd));
            if (!::IsWindow(m_hWnd))
                return;

            // 去掉子类化
            SubclassWindow(m_hWnd, m_OldWndProc);
            m_OldWndProc = ::DefWindowProc;
            m_bSubclassed = false;
        }
    }

    void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        if (!::IsWindow(m_hWnd))
        {
            return;
        }

        ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
    }

    UINT CWindowWnd::ShowModal()
    {
        ASSERT(::IsWindow(m_hWnd));

        UINT nRet = 0;
        HWND hWndParent = GetWindowOwner(m_hWnd);
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        ::EnableWindow(hWndParent, FALSE);

        MSG msg = { 0 };
        while (::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0))
        {
            if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd)
            {
                nRet = msg.wParam;
                ::EnableWindow(hWndParent, TRUE);
                ::SetFocus(hWndParent);
            }

            if (!CPaintManagerUI::TranslateMessage(&msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }

        ::EnableWindow(hWndParent, TRUE);
        ::SetFocus(hWndParent);

        if (msg.message == WM_QUIT)
        {
            ::PostQuitMessage(msg.wParam);
        }

        return nRet;
    }

    void CWindowWnd::Close(UINT nRet)
    {
        ASSERT(::IsWindow(m_hWnd));
        if (!::IsWindow(m_hWnd))
        {
            return;
        }

        PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
    }

#ifdef UILIB_USE_ATL_CENTERWINDOW
    BOOL CWindowWnd::CenterWindow(HWND hWndCenter /*= NULL*/)
    {
        ASSERT(::IsWindow(m_hWnd));

        // 确定要居中的窗口的owner窗口
        DWORD dwStyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
        if (hWndCenter == NULL)
        {
            // 如果是子窗口，则hWndCenter是它的父窗口，否则是owner窗口
            if (dwStyle & WS_CHILD)
            {
                hWndCenter = ::GetParent(m_hWnd);
            }
            else
            {
                hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
            }
        }

        // 获取窗口相对于它的父窗口的坐标
        RECT rcDlg;
        ::GetWindowRect(m_hWnd, &rcDlg);

        RECT rcArea;
        RECT rcCenter;
        HWND hWndParent;
        if (!(dwStyle & WS_CHILD))
        {
            // 不要对一个hWndCenter不可见或最小化的窗口居中
            if (hWndCenter != NULL)
            {
                DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
                if (!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
                {
                    hWndCenter = NULL;
                }
            }

            // 使用屏幕坐标来居中
#if WINVER < 0x0500
            ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
            HMONITOR hMonitor = NULL;
            if (hWndCenter != NULL)
            {
                hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
            }
            else
            {
                hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
            }
            ATLENSURE_RETURN_VAL(hMonitor != NULL, FALSE);

            MONITORINFO minfo;
            minfo.cbSize = sizeof(MONITORINFO);
            BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);
            ATLENSURE_RETURN_VAL(bResult, FALSE);

            rcArea = minfo.rcWork;
#endif
            if (hWndCenter == NULL)
            {
                rcCenter = rcArea;
            }
            else
            {
                ::GetWindowRect(hWndCenter, &rcCenter);
            }
        }
        else
        {
            // 使用父窗口客户区域坐标居中
            hWndParent = ::GetParent(m_hWnd);
            ASSERT(::IsWindow(hWndParent));

            ::GetClientRect(hWndParent, &rcArea);
            ASSERT(::IsWindow(hWndCenter));
            ::GetClientRect(hWndCenter, &rcCenter);
            ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
        }

        int DlgWidth = rcDlg.right - rcDlg.left;
        int DlgHeight = rcDlg.bottom - rcDlg.top;

        // 计算窗口的左上角坐标（基于rcCenter）
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

        // 如果窗口在屏幕外面，则移动它到屏幕里面
        if (xLeft + DlgWidth > rcArea.right)
        {
            xLeft = rcArea.right - DlgWidth;
        }

        if (xLeft < rcArea.left)
        {
            xLeft = rcArea.left;
        }

        if (yTop + DlgHeight > rcArea.bottom)
        {
            yTop = rcArea.bottom - DlgHeight;
        }

        if (yTop < rcArea.top)
        {
            yTop = rcArea.top;
        }

        // 移动窗口位置在屏幕中间！
        return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
#else
    void CWindowWnd::CenterWindow()
    {
        ASSERT(::IsWindow(m_hWnd));
        ASSERT((GetWindowStyle(m_hWnd) & WS_CHILD) == 0);

        RECT rcDlg = { 0 };
        ::GetWindowRect(m_hWnd, &rcDlg);
        RECT rcArea = { 0 };
        RECT rcCenter = { 0 };
        HWND hWnd = *this;
        HWND hWndParent = ::GetParent(m_hWnd);
        HWND hWndCenter = ::GetWindowOwner(m_hWnd);
        if (hWndCenter != NULL)
            hWnd = hWndCenter;

        // 处理多显示器模式下屏幕居中
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
        rcArea = oMonitor.rcWork;

        if (hWndCenter == NULL)
            rcCenter = rcArea;
        else
            ::GetWindowRect(hWndCenter, &rcCenter);

        int DlgWidth = rcDlg.right - rcDlg.left;
        int DlgHeight = rcDlg.bottom - rcDlg.top;

        // Find dialog's upper left based on rcCenter
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

        // The dialog is outside the screen, move it inside
        if (xLeft < rcArea.left)
            xLeft = rcArea.left;
        else if (xLeft + DlgWidth > rcArea.right)
            xLeft = rcArea.right - DlgWidth;

        if (yTop < rcArea.top)
            yTop = rcArea.top;
        else if (yTop + DlgHeight > rcArea.bottom) 
            yTop = rcArea.bottom - DlgHeight;

        ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
#endif

    void CWindowWnd::SetIcon(UINT nRes)
    {
        HICON hIcon = NULL;

        // 设置大图标
        hIcon = (HICON) ::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
        ASSERT(hIcon);
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);

        // 设置小图标
        hIcon = (HICON) ::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
        ASSERT(hIcon);
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
    }

    bool CWindowWnd::RegisterWindowClass()
    {
        WNDCLASS wc = { 0 };
        wc.style = GetClassStyle();
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hIcon = NULL;
        wc.lpfnWndProc = CWindowWnd::__WndProc;
        wc.hInstance = CPaintManagerUI::GetInstance();
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = GetWindowClassName();

        ATOM ret = ::RegisterClass(&wc);
        ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);

        return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }

    bool CWindowWnd::RegisterSuperclass()
    {
        // 从已存在的窗口中获取窗口类信息，然后修改之，以便超类化！
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wc))
        {
            if (!::GetClassInfoEx(CPaintManagerUI::GetInstance(), GetSuperClassName(), &wc))
            {
                ASSERT(!"Unable to locate window class");
                return NULL;
            }
        }

        m_OldWndProc = wc.lpfnWndProc;
        wc.lpfnWndProc = CWindowWnd::__SupWindowProc;
        wc.hInstance = CPaintManagerUI::GetInstance();
        wc.lpszClassName = GetWindowClassName();

        // 超类化
        m_bSupclassed = true;
        ATOM ret = ::RegisterClassEx(&wc);
        ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);

        return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }

    // 窗口函数，窗口是创建的时候使用的窗口函数
    LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CWindowWnd* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
            pThis->m_hWnd = hWnd;
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
        }
        else
        {
            pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

            if (uMsg == WM_NCDESTROY && pThis != NULL)
            {
                LRESULT lRes = ::DefWindowProc(hWnd, uMsg, wParam, lParam);

                ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
                pThis->m_hWnd = NULL;
                pThis->OnFinalMessage(hWnd);
                return lRes;
            }
        }

        if (pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            // 应该不会来到这里
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    // 子类化已存在的窗口的窗口函数，注意一旦子类化，是收不到WM_NCCREATE、WM_CREATE消息的
    // 因此要想子类化后做初始化操作，重载虚函数OnSubWindowInit即可，其它消息可在虚函数
    // HandleMessage中处理
    LRESULT CALLBACK CWindowWnd::__SubWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static BOOL bInit = TRUE;

        // 获取放在窗口属性__SubWindowProc中的指针，可能得到的是空不成？？
        CWindowWnd* pThis = (CWindowWnd*) ::GetProp(hWnd, SUBCLASS_WINDOW_PROP);

        // 子类化时第一次__SubWindowProc被调用则给执行虚函数OnSubWindowInit的机会
        if (bInit)
        {
            bInit = FALSE;

            if (pThis != NULL)
            {
                pThis->OnSubWindowInit();
            }
        }

        // 窗口收到的最后一条消息，这里直接处理，以免HandleMessage乱处理
        if (uMsg == WM_NCDESTROY && pThis != NULL)
        {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);

            // 去掉窗口子类化
            pThis->Unsubclass();
            // 移除窗口属性
            ::RemoveProp(hWnd, SUBCLASS_WINDOW_PROP);
            // 窗口句柄设置为空
            pThis->m_hWnd = NULL;
            // 执行窗口最后的虚函数，可在此释放资源
            pThis->OnFinalMessage(hWnd);

            return lRes;
        }

        // 消息处理交给虚函数HandleMessage
        if (pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            // 应该不会来到这里
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    // 超类化窗口时使用的窗口函数！！！
    LRESULT CALLBACK CWindowWnd::__SupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CWindowWnd* pThis = NULL;

        // 窗口创建后收到的第一条消息
        if (uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
            ::SetProp(hWnd, SUPCLASS_WINDOW_PROP, (HANDLE)pThis);
            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, SUPCLASS_WINDOW_PROP));

            // 窗口收到的最后一条消息，这里直接处理，以免HandleMessage乱处理
            if (uMsg == WM_NCDESTROY && pThis != NULL)
            {
                LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
                ::RemoveProp(hWnd, SUPCLASS_WINDOW_PROP);
                pThis->m_hWnd = NULL;
                pThis->m_bSupclassed = false;
                pThis->Unsubclass();
                pThis->OnFinalMessage(hWnd);
                return lRes;
            }
        }

        if (pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            // 应该不会来到这里
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
    }

    LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
    }

    void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        RECT rc = { 0 };
        if (!::GetClientRect(m_hWnd, &rc))
        {
            return;
        }

        if (cx != -1) rc.right = cx;
        if (cy != -1) rc.bottom = cy;

        if (!::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)))
        {
            return;
        }

        ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (m_bSubclassed || m_bSupclassed)
        {
            return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
        }
        else
        {
            return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
    }

    void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
    {

    }

    void CWindowWnd::OnSubWindowInit()
    {

    }

} // namespace DuiLib

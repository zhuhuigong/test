#include "stdafx.h"
#include "UIShadow.h"


namespace DuiLib
{
    CShadowWindow::CShadowWindow()
        : m_pShadowUI(NULL)
        , m_hWndOwner(NULL)
        , m_pOldOwnerProc(NULL)
        , m_pManager(NULL)
    {
    
    }

    CShadowWindow::~CShadowWindow()
    {
        if (m_pManager != NULL)
        {
            delete m_pManager;
            m_pManager = NULL;
        }
    }

    LPCTSTR CShadowWindow::GetWindowClassName() const
    {
        return _T("UIShadowWindow");
    }

    void CShadowWindow::OnFinalMessage(HWND hWnd)
    {
        ::SetWindowLongPtr(m_hWndOwner, GWL_WNDPROC, (LONG) m_pOldOwnerProc);

        m_pShadowUI->m_pWindow = NULL;
        delete this;
    }

    void CShadowWindow::Init(CShadowUI* pShadowUI)
    {
        if (m_pShadowUI == NULL)
        {
            ASSERT(pShadowUI != NULL);
            m_pShadowUI = pShadowUI;
            m_hWndOwner = pShadowUI->GetManager()->GetPaintWindow();
            m_pManager = new CPaintManagerUI;

            DWORD dwStyle = WS_POPUP | WS_CLIPSIBLINGS;
            DWORD dwExStyle = WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT /*| WS_EX_LAYERED*/;
            if (m_pShadowUI->IsTraversal())
            {
                dwExStyle |= WS_EX_TRANSPARENT;
            }

            // 创建阴影窗口
            Create(m_hWndOwner, _T(""), dwStyle, dwExStyle);
    
            // 发送WM_ACTIVATEAPP消息给主窗口，这样可以激活它！
            ::PostMessage(m_hWndOwner, WM_ACTIVATEAPP, TRUE, 0);
        }
    }

    HWND CShadowWindow::GetOwnerWindow() const
    {
        return m_hWndOwner;
    }

    CShadowUI* CShadowWindow::GetShadowUI() const
    {
        return m_pShadowUI;
    }

    WNDPROC CShadowWindow::GetOldOwnerProc() const
    {
        return m_pOldOwnerProc;
    }

    CPaintManagerUI* CShadowWindow::GetManager() const
    {
        return m_pManager;
    }

    void CShadowWindow::DrawShadow(BOOL bActive)
    {
        LPCTSTR szActiveImage = m_pShadowUI->GetActiveImage();
        LPCTSTR szDeactiveImage = m_pShadowUI->GetDeactiveImage();

        if (bActive)
        {
            if (*szActiveImage != '\0')
            {
                m_pManager->GetRoot()->SetBkImage(szActiveImage);
            }
        }
        else
        {
            // 窗口非激活时如果指定了非激活图片则用，否则用激活的图片（不变）
            if (*szDeactiveImage != '\0')
            {
                m_pManager->GetRoot()->SetBkImage(szDeactiveImage);
            }
            else
            {
                m_pManager->GetRoot()->SetBkImage(szActiveImage);
            }
        }
    }

    void CShadowWindow::MoveWindowWithOwner(int lParam)
    {
        // 有一些奇怪的参数为0的WM_MOVE消息，不处理，直接返回！
        if (lParam == 0)
            return;

        // 计算阴影窗口的位置（左上角）
        int x = (int)(short) LOWORD(lParam) - m_pShadowUI->GetOffsetX();
        int y = (int)(short) HIWORD(lParam) - m_pShadowUI->GetOffsetY();
        ::SetWindowPos(m_hWnd, m_hWndOwner, x, y, -1, -1, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);

        // 阴影窗口初始宽高，即给定的width和height值
        int w = m_pShadowUI->GetFixedWidth();
        int h = m_pShadowUI->GetFixedHeight();

        // 没有指定width和height属性时，自动计算！
        if (w == 0 && h == 0)
        {
            SIZE sz = m_pShadowUI->GetManager()->GetInitSize();

            // 计算阴影窗口的大小
            int width = sz.cx + m_pShadowUI->GetOffsetX() * 2;
            int height = sz.cy + m_pShadowUI->GetOffsetY() * 2;

            // 代码设置width，height属性
            m_pShadowUI->SetFixedWidth(width);
            m_pShadowUI->SetFixedHeight(height);

            // 设置阴影窗口大小
            ::SetWindowPos(m_hWnd, m_hWndOwner, -1, -1, width, height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE);

            // 显示阴影窗口，注意这会使阴影窗口获得焦点，即其它窗口会失去焦点！！
            ::ShowWindow(m_hWnd, SW_SHOWNA);
        }
    }

    void CShadowWindow::OnOwnerWindowSize(int lParam)
    {
        if (m_hWnd != NULL && IsWindow(m_hWnd) && IsWindowVisible(m_hWnd) &&
            m_hWndOwner != NULL && IsWindow(m_hWndOwner) && IsWindowVisible(m_hWndOwner))
        {
            // 重新计算阴影的大小
            SIZE sz = m_pShadowUI->GetManager()->GetInitSize();

            // 主窗口初始宽高
            int w1 = sz.cx;
            int h1 = sz.cy;

            // 主窗口新的宽高
            int w2 = LOWORD(lParam);
            int h2 = HIWORD(lParam);

            // 阴影窗口初始宽高
            int w = m_pShadowUI->GetFixedWidth();
            int h = m_pShadowUI->GetFixedHeight();
            
            // 根据主窗口宽高的变化（如增高10px），那么阴影窗口也相应增加
            int width = w + (w2 - w1);
            int height = h + (h2 - h1);

            //m_pShadowUI->SetFixedWidth(width);
            //m_pShadowUI->SetFixedHeight(height);

            // 设置阴影窗口新的宽高
            ::SetWindowPos(m_hWnd, m_hWndOwner, -1, -1, width, height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE);
        }
    }

    LRESULT CShadowWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;

        switch (uMsg)
        {
        case WM_CREATE:         lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCHITTEST:      lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
        default:                bHandled = FALSE; break;
        }

        if (bHandled) return lRes;
        if (m_pManager->MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;

        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    LRESULT CShadowWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // 使用xml字符串形式加载皮肤，内容很简单，一个Container控件就够了！Window要有bktrans属性
        CDuiString xml;
        xml.Append(_T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>"));
        xml.Append(_T("<Window size=\"%d,%d\" bktrans=\"true\">"));
        xml.Append(_T(" <Container />"));
        xml.Append(_T("</Window>"));
        xml.Format(xml.GetData(), m_pShadowUI->GetFixedWidth(), m_pShadowUI->GetFixedHeight());

        m_pManager->Init(m_hWnd);

        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create((LPCTSTR)xml, (UINT)0, NULL, m_pManager);
        if (pRoot == NULL)
        {
            MessageBox(m_hWnd, _T("加载资源文件失败"), _T("错误"), MB_OK | MB_ICONERROR);
            ExitProcess(1);
            return 0;
        }

        m_pManager->AttachDialog(pRoot);

        // 为主窗口设置一个属性，内容为阴影窗口的指针，然后子类化主窗口
        ::SetProp(m_hWndOwner, SHADOW_WINDOW_PROP, (HANDLE) this);
        m_pOldOwnerProc = (WNDPROC) ::SetWindowLongPtr(m_hWndOwner, GWL_WNDPROC, (LONG) OwnerProc);
        return 0;
    }

    LRESULT CShadowWindow::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_pShadowUI->IsDrag() ? HTCAPTION : 0;
    }

    LRESULT CALLBACK CShadowWindow::OwnerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CShadowWindow *pThis = (CShadowWindow*) ::GetProp(hWnd, SHADOW_WINDOW_PROP);

        switch (uMsg)
        {
        case WM_SHOWWINDOW:
            ::ShowWindow(pThis->GetHWND(), wParam ? SW_SHOWNA : SW_HIDE);
            break;
        case WM_ACTIVATEAPP:
            pThis->DrawShadow(wParam);
            break;
        case WM_MOVE:
            pThis->MoveWindowWithOwner(lParam);
            break;
        case WM_SIZE:
            pThis->OnOwnerWindowSize(lParam);
            break;
        }

        return ::CallWindowProc(pThis->m_pOldOwnerProc, hWnd, uMsg, wParam, lParam);
    }



    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CShadowUI::CShadowUI() 
        : m_pWindow(NULL)
        , m_bDrag(FALSE)
        , m_bTraversal(FALSE)
    {
        m_ptOffset.x = m_ptOffset.y = 0;
    }

    CShadowUI::~CShadowUI()
    {
        
    }

    LPCTSTR CShadowUI::GetClass() const
    {
        return _T("ShadowUI");
    }

    LPVOID CShadowUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_SHADOW) == 0) 
        {
            return static_cast<CShadowUI*>(this);
        }

        return CControlUI::GetInterface(pstrName);
    }

    void CShadowUI::DoInit()
    {
        CControlUI::DoInit();

        if (m_pWindow == NULL)
        {
            m_pWindow = new CShadowWindow;
            m_pWindow->Init(this);
        }
    }

    void CShadowUI::DoPaint(HDC hDC, const RECT& rcPaint)
    {
        // 重写DoPaint，什么也不做！
    }

    void CShadowUI::SetVisible(bool bVisible)
    {
        CControlUI::SetVisible(bVisible);
        if (m_pWindow != NULL)
        {
            ::ShowWindow(m_pWindow->GetHWND(), bVisible ? SW_SHOWNA : SW_HIDE);
        }
    }

    void CShadowUI::SetEnabled(bool bEnabled)
    {
        CControlUI::SetEnabled(bEnabled);
        if (m_pWindow != NULL)
        {
            ::EnableWindow(m_pWindow->GetHWND(), bEnabled);
        }
    }

    HWND CShadowUI::GetShadowWindow()
    {
        if (m_pWindow != NULL)
            return m_pWindow->GetHWND();

        return NULL;
    }

    CShadowWindow* CShadowUI::GetShadowWindow() const
    {
        return m_pWindow;
    }

    POINT CShadowUI::GetOffset() const
    {
        return m_ptOffset;
    }

    LONG CShadowUI::GetOffsetX() const
    {
        return m_ptOffset.x;
    }

    LONG CShadowUI::GetOffsetY() const
    {
        return m_ptOffset.y;
    }

    void CShadowUI::SetOffset(POINT pt)
    {
        m_ptOffset = pt;
    }

    BOOL CShadowUI::IsDrag() const
    {
        return m_bDrag;
    }

    void CShadowUI::SetDrag(BOOL bDrag)
    {
        m_bDrag = bDrag;
    }

    BOOL CShadowUI::IsTraversal() const
    {
        return m_bTraversal;
    }

    void CShadowUI::SetTraversal(BOOL bTraversal)
    {
        m_bTraversal = bTraversal;
    }

    LPCTSTR CShadowUI::GetActiveImage() const
    {
        return m_sActiveImage;
    }

    void CShadowUI::SetActiveImage(LPCTSTR pStrImage)
    {
        m_sActiveImage = pStrImage;
    }

    LPCTSTR CShadowUI::GetDeactiveImage() const
    {
        return m_sDeactiveImage;
    }

    void CShadowUI::SetDeactiveImage(LPCTSTR pStrImage)
    {
        m_sDeactiveImage = pStrImage;
    }

    void CShadowUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (lstrcmpi(pstrName, _T("offset")) == 0) 
        {
            POINT pt = { 0 };
            LPTSTR pstr = NULL;

            pt.x = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
            pt.y = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);    
            
            SetOffset(pt);
        }
        else if (lstrcmpi(pstrName, _T("drag")) == 0)
        {
            SetDrag(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("traversal")) == 0)
        {
            SetTraversal(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("activeimage")) == 0) 
        {
            SetActiveImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("deactiveimage")) == 0) 
        {
            SetDeactiveImage(pstrValue);
        }
        else
        {
            CControlUI::SetAttribute(pstrName, pstrValue);
        }
    }

}

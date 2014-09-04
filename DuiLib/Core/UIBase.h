#ifndef __UIBASE_H__
#define __UIBASE_H__

#pragma once


namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

// 窗口风格/样式
#define UI_WNDSTYLE_CONTAINER    (0)
#define UI_WNDSTYLE_FRAME        (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD        (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG       (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

// 窗口扩展风格/样式
#define UI_WNDSTYLE_EX_FRAME     (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG    (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

// 窗口类风格/样式
#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

// 窗口Prop
#define SUBCLASS_WINDOW_PROP    _T("#16000")
#define SUPCLASS_WINDOW_PROP    _T("#16001")

/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#ifndef DUITRACE
#define DUITRACE DUI__Trace
#endif
#define DUITRACEMSG DUI__TraceMsg
#else
#ifndef DUITRACE
#define DUITRACE
#endif
#define DUITRACEMSG _T("")
#endif


#ifdef UILIB_USE_ATL_CENTERWINDOW
// 从ATL中复制过来的宏，CenterWindow函数会用到
#ifndef ATLENSURE_RETURN_VAL
#define ATLENSURE_RETURN_VAL(expr, val)        \
do {                                           \
    int __atl_condVal=!!(expr);                \
    ASSERT(__atl_condVal);                     \
    if(!(__atl_condVal)) return val;           \
} while (0) 
#endif // ATLENSURE_RETURN_VAL
#endif


    void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...);
    LPCTSTR UILIB_API DUI__TraceMsg(UINT uMsg);


    /////////////////////////////////////////////////////////////////////////////////////
    // DUI消息处理类
    class UILIB_API CNotifyPump
    {
    public:
        bool AddVirtualWnd(CDuiString strName, CNotifyPump* pObject);
        bool RemoveVirtualWnd(CDuiString strName);
        void NotifyPump(TNotifyUI& msg);
        bool LoopDispatch(TNotifyUI& msg);

        DUI_DECLARE_MESSAGE_MAP()

    private:
        CStdStringPtrMap m_VirtualWndMap;
    };


    /////////////////////////////////////////////////////////////////////////////////////
    // duilib窗口基类，窗口类应从此类派生
    class UILIB_API CWindowWnd
    {
    public:
        CWindowWnd();

        HWND GetHWND() const;
        operator HWND() const;

        bool RegisterWindowClass();
        bool RegisterSuperclass();

        HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
        HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
        HWND CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle = 0, DWORD dwExStyle = 0);
        HWND Subclass(HWND hWnd);
        void Unsubclass();
        void ShowWindow(bool bShow = true, bool bTakeFocus = true);
        UINT ShowModal();
        void Close(UINT nRet = IDOK);
#ifdef UILIB_USE_ATL_CENTERWINDOW
        BOOL CenterWindow(HWND hWndCenter = NULL);
#else
        void CenterWindow();    // 居中，支持扩展屏幕
#endif
        void SetIcon(UINT nRes);

        LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
        LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
        void ResizeClient(int cx = -1, int cy = -1);

    protected:
        virtual LPCTSTR GetWindowClassName() const = 0;
        virtual LPCTSTR GetSuperClassName() const;
        virtual UINT GetClassStyle() const;

        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual void OnSubWindowInit(); // 子类化时的初始化函数，相当于OnCreate！
        virtual void OnFinalMessage(HWND hWnd);

        static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK __SubWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK __SupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:
        HWND m_hWnd;
        WNDPROC m_OldWndProc;
        bool m_bSubclassed;
        bool m_bSupclassed;
    };

} // namespace DuiLib

#endif // __UIBASE_H__

#ifndef __UISHADOW_H__
#define __UISHADOW_H__

#pragma once

// 新增类，为窗口附上一个“阴影”窗口，可配置

namespace DuiLib
{
    // 类前向声明，否则报错
    class CShadowUI;

    class CShadowWindow : public CWindowWnd
    {
    public:
        CShadowWindow();
        ~CShadowWindow();

        LPCTSTR GetWindowClassName() const;
        void OnFinalMessage(HWND hWnd);
        
        void Init(CShadowUI* pShadowUI);

        HWND GetOwnerWindow() const;
        CShadowUI* GetShadowUI() const;
        WNDPROC GetOldOwnerProc() const;
        CPaintManagerUI* GetManager() const;

        void DrawShadow(BOOL bActive);
        void MoveWindowWithOwner(int lParam);
        void OnOwnerWindowSize(int lParam);
    
    protected:
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        static LRESULT CALLBACK OwnerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:
        CPaintManagerUI* m_pManager;
        CShadowUI* m_pShadowUI;
        WNDPROC m_pOldOwnerProc;
        HWND m_hWndOwner;
    };


    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    //
    class UILIB_API CShadowUI : public CControlUI
    {
        friend class CShadowWindow;

    public:
        CShadowUI();
        ~CShadowUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void DoInit();
        void DoPaint(HDC hDC, const RECT& rcPaint);
        void SetVisible(bool bVisible = true);
        void SetEnabled(bool bEnabled = true);
        
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        HWND GetShadowWindow();
        CShadowWindow* GetShadowWindow() const;

        POINT GetOffset() const;
        LONG GetOffsetX() const;
        LONG GetOffsetY() const;
        void SetOffset(POINT pt);

        BOOL IsDrag() const;
        void SetDrag(BOOL bDrag);

        BOOL IsTraversal() const;
        void SetTraversal(BOOL bTraversal);

        LPCTSTR GetActiveImage() const;
        void SetActiveImage(LPCTSTR pStrImage);

        LPCTSTR GetDeactiveImage() const;
        void SetDeactiveImage(LPCTSTR pStrImage);

    protected:
        CShadowWindow* m_pWindow;
        POINT m_ptOffset;
        BOOL m_bDrag;
        BOOL m_bTraversal;
        CDuiString m_sActiveImage;
        CDuiString m_sDeactiveImage;
    };

} // end namespace


#endif  // __UISHADOW_H__

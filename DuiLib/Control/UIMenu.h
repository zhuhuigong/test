#ifndef __UIMENU_H__
#define __UIMENU_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "observer_impl_base.hpp"

#define WM_CUSTOMMENU_CLICK     (WM_USER + 0x100)

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
struct ContextMenuParam
{
    // 1: remove all
    // 2: remove the sub menu
    WPARAM wParam;
    HWND hWnd;
};

enum MenuAlignment
{
    eMenuAlignment_Left =   1 << 1,
    eMenuAlignment_Top =    1 << 2,
    eMenuAlignment_Right =  1 << 3,
    eMenuAlignment_Bottom = 1 << 4,
};

typedef class ObserverImpl<BOOL, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<BOOL, ContextMenuParam> ContextMenuReceiver;

class CContextMenuObServerHwnd : public ContextMenuObserver
{
public:
    CContextMenuObServerHwnd():m_hMainHwnd(NULL){};
    ~CContextMenuObServerHwnd(){};

public:
    HWND GetMainHwnd() {return m_hMainHwnd;}
    VOID SetMainHwnd(HWND hWnd) {m_hMainHwnd = hWnd;}
private:
    HWND m_hMainHwnd;
};

extern CContextMenuObServerHwnd gContextMenuObServer;

// MenuUI
extern const TCHAR* const kMenuUIClassName;// = _T("MenuUI");
extern const TCHAR* const kMenuUIInterfaceName;// = _T("Menu");

class CListUI;
class CMenuUI : public CListUI
{
public:
    CMenuUI();
    ~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    virtual void DoEvent(TEventUI& event);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

    SIZE EstimateSize(SIZE szAvailable);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    bool IsShowShadow();
    void SetShowShadow(bool bShow = true);

private:
    bool m_bShowShadow;
};

/////////////////////////////////////////////////////////////////////////////////////
//

// MenuElementUI
extern const TCHAR* const kMenuElementUIClassName;// = _T("MenuElementUI");
extern const TCHAR* const kMenuElementUIInterfaceName;// = _T("MenuElement);

class CMenuElementUI;
class CMenuWnd : public CWindowWnd, public ContextMenuReceiver, public INotifyUI
{
public:
    CMenuWnd(HWND hParent = NULL);
    void Init(CMenuElementUI* pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT point);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL Receive(ContextMenuParam param);
    BOOL IsRootMenu();
    BOOL GetMonitorWorkRect(LPRECT Rect);
    BOOL GetCurrentMonitorWorkRect(LPRECT Rect);

private:
    void Notify(TNotifyUI& msg);

public:
    HWND m_hParent;
    POINT m_BasedPoint;
    STRINGorID m_xml;
    CDuiString m_sType;
    CPaintManagerUI m_pm;
    CMenuElementUI* m_pOwner;
    CMenuUI* m_pLayout;
   // CWndShadow m_WndShadow;
};

class CListContainerElementUI;
class CMenuElementUI : public CListContainerElementUI
{
    friend CMenuWnd;

public:
    CMenuElementUI();
    ~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoPaint(HDC hDC, const RECT& rcPaint);

    void DrawItemText(HDC hDC, const RECT& rcItem);

    SIZE EstimateSize(SIZE szAvailable);

    bool Activate();

    void DoEvent(TEventUI& event);

    CMenuWnd* GetMenuWnd();

    void CreateMenuWnd();
    void DoInit();
    VOID SetAllSubMenuItemVisible(bool bVisible = true);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetRightWidth(int nWidth);
    void SetLeftWidth(int nWidth);
    void SetLeftIconSize(SIZE Size);
    void SetLeftIconImage(LPCTSTR lpszLeftIcon);
    void SetRightArrowImage(LPCTSTR lpszIcon);
    void SetRightArrowSize(SIZE Size);
    void SetIsSeparator(bool bSeparator = true);
    void Notify(TNotifyUI& msg);

protected:
    CMenuWnd* m_pWindow;

private:
    int m_LeftWidth;
    int m_RightWidth;
    CDuiString m_strLeftIcon;
    CDuiString m_strRightArrow;
    CSize m_LeftIconSize; 
    CSize m_RightArrowSize;
    BOOL m_bHasSubMenu;
    BOOL m_bIsSeparator;

    int m_nSeparatorHeight;
};

} // namespace DuiLib

#endif // __UIMENU_H__

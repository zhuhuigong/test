#ifndef __MAINFORM_H__

//////////////////////////////////////////////////////////////////////////
///

class CPage1 : public CNotifyPump
{
    DUI_DECLARE_MESSAGE_MAP()

public:
    CPage1();

    void SetPaintMagager(CPaintManagerUI* pPaintMgr);
    virtual void OnClick(TNotifyUI& msg);
    virtual void OnSelectChanged(TNotifyUI &msg);
    virtual void OnItemClick(TNotifyUI &msg);

private:
    CPaintManagerUI* m_pPaintManager;
};


//////////////////////////////////////////////////////////////////////////
///

class CPage2 : public CNotifyPump
{
    DUI_DECLARE_MESSAGE_MAP()

public:
    CPage2();

    void SetPaintMagager(CPaintManagerUI* pPaintMgr);
    virtual void OnClick(TNotifyUI& msg);
    virtual void OnSelectChanged(TNotifyUI &msg);
    virtual void OnItemClick(TNotifyUI &msg);

private:
    CPaintManagerUI* m_pPaintManager;
};

//////////////////////////////////////////////////////////////////////////
///

class CMainForm : public WindowImplBase
{
public:
    CMainForm();
    ~CMainForm();

    virtual void OnFinalMessage(HWND);
    virtual CDuiString GetSkinFolder();
    virtual CDuiString GetSkinFile();
    virtual LPCTSTR GetWindowClassName(void) const;
    virtual void Notify(TNotifyUI &msg);
    virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual void InitWindow();
    virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual UILIB_RESOURCETYPE GetResourceType() const;

    DUI_DECLARE_MESSAGE_MAP()
    virtual void OnClick(TNotifyUI& msg);
    virtual void OnSelectChanged(TNotifyUI &msg);
    virtual void OnItemClick(TNotifyUI &msg);

private:
    CButtonUI* m_pCloseBtn;
    CButtonUI* m_pMaxBtn;
    CButtonUI* m_pRestoreBtn;
    CButtonUI* m_pMinBtn;
    CPage1 m_Page1;
    CPage2 m_Page2;
};


#endif
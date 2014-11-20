// Demo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Demo.h"
#include "MainForm.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);

    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr)) return 0;

    CMainForm* pFrame = new CMainForm();
    if (pFrame == NULL) return 0;
    pFrame->Create(NULL, _T("Duilib界面库控件示例"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 600);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
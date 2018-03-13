// xcast-dev.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "xcast-dev.h"
#include "xcast-ui-handler.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern BOOL InitInstance(HINSTANCE, int);



int APIENTRY 
wWinMain(_In_ HINSTANCE hInstance,
         _In_opt_ HINSTANCE hPrevInstance,
         _In_ LPWSTR    lpCmdLine,
         _In_ int       nCmdShow) 
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  
  // 初始化公共空间
  INITCOMMONCONTROLSEX icc;
  icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icc.dwICC = ICC_BAR_CLASSES;
  InitCommonControlsEx(&icc);

  // 执行应用程序初始化:
  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  /* 启动XCAST */
  //ui_init_xcast(true, &main_app);
  
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XCASTDEV));
  MSG msg;
  
  // 主消息循环:
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  ui_init_xcast(false, NULL);

  return (int) msg.wParam;
}

#include "main-view.h"
#include "xcast-ui-handler.h"

typedef struct tab_item_s {
  const TCHAR        *text;
  int32_t             icon;
  HWND                hView;
  HWND               (*CreateTab)(HWND, HINSTANCE);
} tab_item_t;

static HWND CreateTipsView(HWND hTab, HINSTANCE hInst);
void AdjustTabViewSize(HWND hTab);

static tab_item_t tab_items[] = {
  { _T("视频"), 12, NULL, NULL },
  { _T("参数"), 15, NULL, NULL },
  { _T("图表"), 0, NULL, NULL },
  { _T("日志"), 21, NULL, CreateTipsView },
  { _T("Tips"), 21, NULL, CreateTipsView },
};

// Creates a tab control, sized to fit the specified parent window's client
//   area, and adds some tabs. 
// Returns the handle to the tab control. 
// hwndParent - parent window (the application's main window). 
// 
HWND CreateTabView(HWND hwndParent, HINSTANCE hInst) {
  INITCOMMONCONTROLSEX icex;
  HWND hwndTab;
  TCITEM tie;
  int i;
  // Initialize common controls.
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC = ICC_TAB_CLASSES;
  InitCommonControlsEx(&icex);
  // Get the dimensions of the parent window's client area, and
  // create a tab control child window of that size. Note that g_hInst
  // is the global instance handle.
  //hwndTab = CreateWindow(WC_TABCONTROL, L"",
  //                       WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
  //                       0, 0, 0, 0,
  //                       hwndParent, NULL, hInst, NULL);
  hwndTab = CreateWindowEx(0, WC_TABCONTROL, TEXT(""),
    WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_EX_COMPOSITED,
    0, 0, 0, 0,
    hwndParent,
    (HMENU)IDC_TABVIEW,
    hInst,
    NULL);
  if (hwndTab == NULL) {
    return NULL;
  }

  TabCtrl_SetImageList(hwndTab, main_app.hImageList);

  // Add tabs for each day of the week.
  tie.mask = TCIF_TEXT | TCIF_IMAGE;

  for (i = 0; i < sizeof(tab_items) / sizeof(tab_items[0]); i++) {
    tie.iImage = tab_items[i].icon;
    tie.pszText = (TCHAR *)tab_items[i].text;

    if (TabCtrl_InsertItem(hwndTab, i, &tie) == -1) {
      DestroyWindow(hwndTab);
      return NULL;
    }

    if (tab_items[i].CreateTab) {
      tab_items[i].hView = tab_items[i].CreateTab(hwndTab, hInst);
    }
  }

  main_app.hVideoView = CreateWindowEx(WS_EX_CLIENTEDGE,
    _T("Static"), NULL,
    WS_CHILD | WS_VISIBLE,
    0, 0, 0, 0,
    hwndTab, NULL, hInst, NULL);
  main_app.hDCView = GetDC(main_app.hVideoView);
  main_app.hMemDC = CreateCompatibleDC(main_app.hDCView);
  SetBkMode(main_app.hMemDC, TRANSPARENT);
  main_app.hDCCompatible = CreateCompatibleDC(main_app.hDCView);
  tab_items[0].hView = main_app.hVideoView;

  main_app.hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, 
    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
    CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));

  return hwndTab;
}

LRESULT
OnTabviewMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case TCN_SELCHANGING:
      // Return FALSE to allow the selection to change.
      return 0;
    case TCN_SELCHANGE: {
      int32_t iPage = TabCtrl_GetCurSel(hWnd);

      for (int32_t i = 0; i < sizeof(tab_items) / sizeof(tab_items[0]); i++) {
        if (tab_items[i].hView) ShowWindow(tab_items[i].hView, SW_HIDE);
      }

      if (tab_items[iPage].hView) ShowWindow(tab_items[iPage].hView, SW_SHOW);
      if (0 == iPage) {
        InvalidVideoView();
      }

      break;
    }
    default:
      break;
    }

    break;
  default:
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

static HWND 
CreateTipsView(HWND hTab, HINSTANCE hInst)
{
  HWND hwndEdit;
  RECT rcClient;

  TCHAR lpszLatin[] = L"Lorem ipsum dolor sit amet, consectetur \r\n"
    L"adipisicing elit, sed do eiusmod tempor \r\n"
    L"incididunt ut labore et dolore magna \r\n"
    L"aliqua. Ut enim ad minim veniam, quis \r\n"
    L"nostrud exercitation ullamco laboris nisi \r\n"
    L"ut aliquip ex ea commodo consequat. Duis \r\n"
    L"aute irure dolor in reprehenderit in \r\n"
    L"voluptate velit esse cillum dolore eu \r\n"
    L"fugiat nulla pariatur. Excepteur sint \r\n"
    L"occaecat cupidatat non proident, sunt \r\n"
    L"in culpa qui officia deserunt mollit \r\n"
    L"anim id est laborum.";

  GetClientRect(main_app.hMainFrame, &rcClient);
  TabCtrl_AdjustRect(hTab, FALSE, &rcClient);

  hwndEdit = CreateWindowEx(
    0, L"EDIT",   // predefined class 
    NULL,         // no window title 
    WS_CHILD | WS_VSCROLL |
    ES_LEFT | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    rcClient.left, rcClient.top,
    rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,   // set size in WM_SIZE message 
    hTab,         // parent window 
    (HMENU)IDC_TABVIEW_TIPS,   // edit control ID 
    hInst,
    NULL);        // pointer not needed 

  // Add text to the window. 
  SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)lpszLatin);

  return hwndEdit;
}

void 
AdjustTabViewSize(HWND hTab)
{
  HWND hview;
  RECT rcClient;

  for (int32_t i = 0; i < sizeof(tab_items) / sizeof(tab_items[0]); i++) {
    hview = tab_items[i].hView;
    if (hview) {
      GetClientRect(main_app.hMainFrame, &rcClient);
      TabCtrl_AdjustRect(hTab, FALSE, &rcClient);
      MoveWindow(hview, rcClient.left, rcClient.top,
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, FALSE);
    }
  }
}
#include "stdafx.h"
#include "xcast-dev.h"
#include "main-view.h"
#include <string>

void
utf16_to_utf8(const char16_t *wstr, uint32_t len, std::string& str)
{
    uint32_t utf8_len = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)wstr, (int)len, nullptr, 0, nullptr, nullptr);
    if (utf8_len) {
        char *pstr = new char[utf8_len + 1];
        WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)wstr, (int)len, pstr, utf8_len, nullptr, nullptr);
        pstr[utf8_len] = '\0';
        str = pstr;
        delete[] pstr;
    }
}

void
utf8_to_utf16(const char *str, uint32_t len, std::wstring& wstr)
{
    uint32_t utf16_len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)str, (int)len, nullptr, 0);
    if (utf16_len) {
        wchar_t *pwstr = new wchar_t[utf16_len + 1];
        MultiByteToWideChar(CP_UTF8, 0, (LPCCH)str, (int)len, pwstr, utf16_len);
        pwstr[utf16_len] = '\0';
        wstr = pwstr;
        delete[] pwstr;
    }
}

static LRESULT CALLBACK TreeViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

///* 树控件双击事件 */
//static int32_t
//on_start_stream(tree_item_data_t *item_data, HTREEITEM hItem)
//{
//  item_data->start = !item_data->start;
//  ui_start_stream(item_data->id.c_str(), item_data->start, &main_app);
//
//  return XCAST_OK;
//}

static int32_t
on_device_set_default(tree_item_data_t *item_data, HTREEITEM hItem)
{
  return XCAST_OK;
}


static int32_t 
on_media_file_start(tree_item_data_t *item_data, HTREEITEM hItem)
{
  return XCAST_OK;
}

static int32_t 
on_media_file_setting(tree_item_data_t *item_data, HTREEITEM hItem)
{
  return XCAST_OK;
}

static int32_t 
on_media_file_verify(tree_item_data_t *item_data, HTREEITEM hItem)
{
  return XCAST_OK;
}

static int32_t 
on_screen_setting(tree_item_data_t *item_data, HTREEITEM hItem)
{
  return XCAST_OK;
}
//static int32_t on_start_track_all(tree_item_data_t *item_data, HTREEITEM hItem);

static tree_item_data_t tree_items[] = {
  { 0, "stream", "stream", "", "", "", false, NULL, NULL },
  { 0, "device", "device", "", "", "", false, NULL, NULL },
  { 0, "camera", "device.camera", "", "", "", false, NULL, NULL },
  { 0, "mic", "device.mic", "", "", "", false, NULL, NULL },
  { 0, "speaker", "device.speaker", "", "", "", false, NULL, NULL },
 // { 0, "external", "device.external", "", "", "", false, NULL, NULL },
 // { 0, "preference", "preference", "", "", "", false, NULL, NULL },
  //{ 0, "audio", "preference.audio", "", "", "", false, NULL, NULL },
  //{ 0, "video", "preference.video", "", "", "", false, NULL, NULL },
  //{ 0, "net", "preference.net", "", "", "", false, NULL, NULL },
 // { 0, "event", "event", "", "", "", false, NULL, NULL },
  //{ 0, "stream", "event.stream", "", "", "", false, NULL, NULL },
  //{ 21, "xc_stream_added", "event.stream.xc_stream_added", "", "", "", false, NULL, NULL },
  //{ 21, "xc_stream_updated", "event.stream.xc_stream_updated", "", "", "", false, NULL, NULL },
  //{ 21, "xc_stream_removed", "event.stream.xc_stream_removed", "", "", "", false, NULL, NULL },
  //{ 0, "track", "event.track", "", "", "", false, NULL, NULL },
  //{ 21, "xc_track_added", "event.track.xc_track_added", "", "", "", false, NULL, NULL },
  //{ 21, "xc_track_updated", "event.track.xc_track_updated", "", "", "", false, NULL, NULL },
  //{ 21, "xc_track_removed", "event.track.xc_track_removed", "", "", "", false, NULL, NULL },
  //{ 21, "xc_track_media", "event.track.xc_track_media", "", "", "", false, NULL, NULL },
  //{ 0, "device", "event.device", "", "", "", false, NULL, NULL },
  //{ 21, "xc_device_added", "event.device.xc_device_added", "", "", "", false, NULL, NULL },
  //{ 21, "xc_device_updated", "event.device.xc_device_updated", "", "", "", false, NULL, NULL },
  //{ 21, "xc_device_removed", "event.device.xc_device_removed", "", "", "", false, NULL, NULL },
  //{ 21, "xc_device_preprocess", "event.device.xc_device_preprocess", "", "", "", false, NULL, NULL },
  //{ 21, "xc_device_preview", "event.device.xc_device_preview", "", "", "", false, NULL, NULL },
};

void 
SetTreeItemIcon(HWND hTree, HTREEITEM hItem, int32_t icon)
{
  TVITEMW       tvi;

  tvi.hItem = hItem;
  tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  tvi.iImage = icon;
  tvi.iSelectedImage = icon;
  TreeView_SetItem(hTree, &tvi);
}

/* 按照路径查找树控件中的节点和父节点，如果找不到，则返回NULL */
HTREEITEM
GetTreeItem(HWND hTree, LPCSTR pszPath, HTREEITEM *pParent)
{
  wchar_t             text[MAX_PATH] = { 0 };
  //xcast_property_t   *pi;
  std::string         spath(pszPath);
  char               *ch = (char *)spath.c_str();
  char               *pos = ch;
  HTREEITEM           hItem = TreeView_GetRoot(hTree);

  if (pParent) *pParent = NULL;

  while (*pos) {
    if (*pos == '.' || *(pos + 1) == '\0') {
      if (ch == pos) return NULL;
      if (*pos == '.') *pos = 0;

      TVITEMW       tvi;
      std::wstring  name;
      utf8_to_utf16(ch, strlen(ch), name);

      tvi.mask = TVIF_TEXT /*| TVIF_PARAM*/;

      tvi.pszText = text;
      tvi.cchTextMax = MAX_PATH;

      /* 在当前级别查找匹配项 */
      while (hItem) {
        tvi.hItem = hItem;
        SendMessage(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
        //pi = (xcast_property_t *)tvi.lParam;

        if (name == tvi.pszText) {
        //if (name == pi->name) {
          if (*(pos + 1) == '\0') return hItem;     /* 找到匹配项 */
          
          if (pParent) *pParent = hItem;
          hItem = TreeView_GetChild(hTree, hItem);  /* 找到匹配父项，继续查找下一级别 */
          break;
        }

        hItem = TreeView_GetNextSibling(hTree, hItem);
      }

      if (!hItem) break; 
      ch = ++pos;
      continue;
    }

    ++pos;
  }

  return NULL;
}

void
RemoveTreeItem(HWND hTree, HTREEITEM hItem)
{
  HTREEITEM               hChild;
  tree_item_data_t       *item_data;

  hChild = TreeView_GetChild(hTree, hItem);
  while (hChild) {
    item_data = GetTreeItemData(hTree, hChild);
    delete item_data->cap_data;
    delete item_data;
    TreeView_DeleteItem(hTree, hChild);
    hChild = TreeView_GetChild(hTree, hItem);
  }

  item_data = GetTreeItemData(hTree, hItem);
  delete item_data->cap_data;
  delete item_data;
  TreeView_DeleteItem(hTree, hItem);
}

tree_item_data_t *
GetTreeItemData(HWND hTree, HTREEITEM hItem)
{
  TVITEMW                 tvi;

  ZeroMemory(&tvi, sizeof(TVITEMW));
  tvi.mask = TVIF_PARAM;
  tvi.hItem = hItem;
  SendMessage(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);

  return (tree_item_data_t *)tvi.lParam;
}

/* 树视图中插入新节点 */
HTREEITEM
AddTreeItem(HWND hTree, LPCSTR pszFullPath, LPCSTR pszName, tree_item_data_t *item_data)
{
  TVITEMW                 tvi;
  TVINSERTSTRUCTW         tvins;
  HTREEITEM               hParent, hItem;

  hItem = GetTreeItem(hTree, pszFullPath, &hParent);
  if (hItem) return hItem;

  ZeroMemory(&tvi, sizeof(TVITEMW));
  tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
  // Set the text of the item.
  std::wstring wstr;
  utf8_to_utf16(pszName, strlen(pszName), wstr);
  tvi.pszText = (LPWSTR)wstr.c_str();
  tvi.cchTextMax = (int)wstr.size();

  // Assume the item is not a parent item, so give it a document image.
  tvi.iImage = item_data->icon;
  tvi.iSelectedImage = item_data->icon;
  // Save the heading level in the item's application-defined data area.
  tvi.lParam = (LPARAM)item_data;
  tvins.item = tvi;
  tvins.hInsertAfter = TVI_LAST;

  // Set the parent item based on the specified level.
  tvins.hParent = hParent;

  // Add the item to the tree-view control.
  return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEMW, 0, (LPARAM)(LPTVINSERTSTRUCTW)&tvins);
}

/* 根据xcast系统属性在树视图中创建固定节点 */
static BOOL
InitTreeViewItems(HWND hwndTV)
{
  for (int i = 0; i < sizeof(tree_items) / sizeof(tree_item_data_t); i++) {
    //std::string             full_path;
    TVITEMW                 tvi = { 0 };
    TVINSERTSTRUCTW         tvins;
    HTREEITEM               hParent, hItem;
    tree_item_data_t       *item_data = new tree_item_data_t();

    *item_data = tree_items[i];
    //if (!item_data->path.empty()) full_path = item_data->path + ".";
    //full_path.append(item_data->text);
    GetTreeItem(hwndTV, item_data->path.c_str(), &hParent);

    tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    // Set the text of the item.
    std::wstring wstr;
    utf8_to_utf16(item_data->text.c_str(), (uint32_t)item_data->text.size(), wstr);
    tvi.pszText = (LPWSTR)wstr.c_str();
    tvi.cchTextMax = (int)wstr.size();
    // Assume the item is not a parent item, so give it a document image.
    tvi.iImage = item_data->icon;
    tvi.iSelectedImage = item_data->icon;
    // Save the heading level in the item's application-defined data area.
    tvi.lParam = (LPARAM)item_data;
    tvins.item = tvi;
    tvins.hInsertAfter = TVI_LAST;

    // Set the parent item based on the specified level.
    tvins.hParent = hParent ? hParent : TVI_ROOT;

    // Add the item to the tree-view control.
    hItem = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEMW, 0, (LPARAM)(LPTVINSERTSTRUCTW)&tvins);
    if (hItem == NULL) {
      return FALSE;
    }
  }

  return TRUE;
}

HWND 
CreateTreeView()
{
  HWND hTreeView;
  RECT rcClient, rcToolbar, rcStatusbar;

  GetClientRect(main_app.hMainFrame, &rcClient);
  GetClientRect(main_app.hToolbar, &rcToolbar);
  GetClientRect(main_app.hStatusbar, &rcStatusbar);
  hTreeView = CreateWindowEx(0,
    WC_TREEVIEW,
    TEXT("Tree View"),
    WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_FULLROWSELECT | 
    TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_INFOTIP | TVS_SHOWSELALWAYS,
    0,
    rcToolbar.bottom,
    320,
    rcClient.bottom - rcToolbar.bottom - rcStatusbar.bottom,
    main_app.hMainFrame,
    (HMENU)ID_TREEVIEW,
    main_app.hInstance,
    NULL);
  // Associate the image list with the tree-view control.
  TreeView_SetImageList(hTreeView, main_app.hImageList, TVSIL_NORMAL);
  InitTreeViewItems(hTreeView);

  return hTreeView;
}

LRESULT
OnTreeViewMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case TVN_GETINFOTIP: {
      LPNMTVGETINFOTIP        pTip = (LPNMTVGETINFOTIP)lParam;
      TVITEMW                 tvi;
      tree_item_data_t       *item_data;
      std::string             strTip;
      std::wstring            wstrTips;

      // Get the text for the item.
      ZeroMemory(&tvi, sizeof(TVITEMW));
      tvi.mask = TVIF_PARAM;
      tvi.hItem = pTip->hItem;
      SendMessage(hWnd, TVM_GETITEMW, 0, (LPARAM)&tvi);
      item_data = (tree_item_data_t *)tvi.lParam;
      if (!item_data || item_data->text.empty()) break;

      if (!item_data->path.empty()) {
        strTip = "\"" + item_data->path + "\"";
      }
      
      utf8_to_utf16(strTip.c_str(), strTip.size(), wstrTips);
      if (item_data->is_stream && !main_app.tips.empty()) {
        wstrTips.append(_T("\n"));
        wstrTips += main_app.tips;
      }

      memcpy(pTip->pszText, wstrTips.c_str(), min(pTip->cchTextMax, (int)(wstrTips.size() << 1)));
    }
    case TVN_BEGINLABELEDIT:

      return 0; /* '0' allow edit; '1' disallow edit */
    case TVN_ENDLABELEDIT: {
      LPNMTVDISPINFO pDisp = (LPNMTVDISPINFO)lParam;
      TVITEM item = pDisp->item;
      //HWND hEdit = TreeView_GetEditControl(hWnd);
      //EM_SETLIMITTEXT;

      break;
    }
    case NM_DBLCLK: {
      // Get a handle to the selected item
      HTREEITEM         hSelectedItem = TreeView_GetSelection(main_app.hTreeView);
      tree_item_data_t *item_data;

      if (hSelectedItem) {
        item_data = GetTreeItemData(main_app.hTreeView, hSelectedItem);
        if (item_data->db_click) {
          item_data->db_click(item_data, hSelectedItem);
          return 1;
        }
      }

      break;
    }
    //case TVN_SELCHANGING: {
    //  LPNMTREEVIEW    pnmtv = (LPNMTREEVIEW)lParam;

    //  TVITEMA         tvi;
    //  ZeroMemory(&tvi, sizeof(TVITEMA));
    //  tvi.mask = TVIF_IMAGE | TV;
    //  tvi.hItem = pTip->hItem;
    //  SendMessage(hWnd, TVM_GETITEMA, 0, (LPARAM)&tvi);


    //  break;
    //}
    default:
      break;
    }

    break;

  case WM_COMMAND: {
    int wmId = LOWORD(wParam);

    switch (wmId) {
    case IDM_SEL_CAPTURE:

      break;
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    break;
  }

  default:
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

///* 树控件中添加一个媒体流节点 */
//HTREEITEM
//AddStream(HWND hTree, bool add, const char *parent,
//  const char *stream, const char *id)
//{
//  tree_item_data_t *item_data;
//  std::string       path(parent);
//  HTREEITEM         hItem, hParent;
//
//  path += '.';
//  path += stream;
//  if (!add) {
//    /* 移除 */
//    hItem = GetTreeItem(hTree, path.c_str(), NULL);
//    if (hItem) RemoveTreeItem(hTree, hItem);
//    return NULL;
//  }
//
//  hParent = GetTreeItem(hTree, parent, NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  item_data = CreateItemData(1, path.c_str(), stream);
//  item_data->id = id;
//  item_data->db_click = on_start_stream;
//
//  return AddTreeItem(hTree, path.c_str(), stream, item_data);
//}

///* 树控件中添加一个上行节点 */
//HTREEITEM
//AddOutputTrack(HWND hTree, bool add, const char *parent,
//  const char *track_name, const char *id)
//{
//  tree_item_data_t *item_data;
//  xcast_data_t      path;
//  HTREEITEM         hItem, hParent;
//
//  /* 音频轨道输出 */
//  path.format("%s.%s", parent, track_name);
//  if (!add) {
//    /* 移除 */
//    ClearTrackBuffer(id);
//    hItem = GetTreeItem(hTree, path, NULL);
//    if (hItem) RemoveTreeItem(hTree, hItem);
//    return NULL;
//  }
//
//  hParent = GetTreeItem(hTree, parent, NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  item_data = CreateItemData(9, path, track_name);
//  item_data->id = id;
//  item_data->db_click = on_start_track;
//
//  return AddTreeItem(hTree, path, track_name, item_data);
//}
//
///* 树控件中添加一个下行节点 */
//HTREEITEM
//AddInputTrack(HWND hTree, bool add, 
//  const char *parent, uint64_t uin, const char *track_name, const char *id)
//{
//  xcast_data_t      path, name;
//  tree_item_data_t *item_data;
//  HTREEITEM         hItem, hParent;
//
//
//  if (!add) {
//    /* 移除 */
//    path.format("%s-%llu", id, uin);
//    ClearTrackBuffer(path);
//
//    path.format("%s.%llu.%s", parent, uin, track_name);
//    hItem = GetTreeItem(hTree, path, &hParent);
//    if (hItem) {
//      /* 删除空节点 */
//      RemoveTreeItem(hTree, hItem);
//
//      hItem = TreeView_GetChild(hTree, hParent);
//      if (!hItem) {
//        /* 清空节点 */
//        RemoveTreeItem(hTree, hParent);
//      }
//    }
//
//    return NULL;
//  }
//
//  /* 轨道输入 */
//  hParent = GetTreeItem(hTree, parent, NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  path.format("%s.%llu", parent, uin);
//  hParent = GetTreeItem(hTree, path, NULL);
//  if (!hParent) {
//    name.format("%llu", uin);
//    item_data = CreateItemData(14, parent, path);
//    hParent = AddTreeItem(hTree, path, name, item_data);
//  }
//
//  name.format("%s-%llu", id, uin);
//  path.format("%s.%llu.%s", parent, uin, track_name);
//  item_data = CreateItemData(9, path, track_name);
//  item_data->id = name;
//  item_data->db_click = on_start_track;
//  hItem = AddTreeItem(hTree, path, track_name, item_data);
//
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  return hItem;
//}

///* 树控件中添加一个下行节点 */
//HTREEITEM
//AddCameraDevice(HWND hTree, bool add, const char *camera)
//{
//  char              path[MAX_PATH] = { 0 };
//  tree_item_data_t *item_data;
//  HTREEITEM         hItem, hParent;
//  char              gbk_name[256] = { 0 };
//  utf8_to_gbk(camera, gbk_name, sizeof(gbk_name));
//  if (!add) {
//    /* 移除 */
//    snprintf(path, MAX_PATH, "摄像头.%s", gbk_name);
//    hItem = GetTreeItem(hTree, path, &hParent);
//    if (hItem) {
//      /* 删除空节点 */
//      RemoveTreeItem(hTree, hItem);
//
//      hItem = TreeView_GetChild(hTree, hParent);
//      if (!hItem) {
//        /* 清空节点 */
//        RemoveTreeItem(hTree, hParent);
//      }
//    }
//
//    return NULL;
//  }
//
//  /* 轨道输入 */
//  hParent = GetTreeItem(hTree, "摄像头", NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  snprintf(path, MAX_PATH, "摄像头.%s", gbk_name);
//  item_data = CreateItemData(14, "摄像头", gbk_name);
//  item_data->id = gbk_name;
//  item_data->db_click = on_camera_preview;
//  hItem = AddTreeItem(hTree, path, gbk_name, item_data);
//
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  return hItem;
//}
//
//HTREEITEM AddMicDevice(HWND hTree, bool add, const char *mic)
//{
//  char              path[MAX_PATH] = { 0 };
//  tree_item_data_t *item_data;
//  HTREEITEM         hItem, hParent;
//  char              gbk_name[256] = {0};
//  utf8_to_gbk(mic, gbk_name, sizeof(gbk_name));
//  if (!add) {
//    /* 移除 */
//    snprintf(path, MAX_PATH, "麦克风.%s", gbk_name);
//    hItem = GetTreeItem(hTree, path, &hParent);
//    if (hItem) {
//      /* 删除空节点 */
//      RemoveTreeItem(hTree, hItem);
//
//      hItem = TreeView_GetChild(hTree, hParent);
//      if (!hItem) {
//        /* 清空节点 */
//        RemoveTreeItem(hTree, hParent);
//      }
//    }
//
//    return NULL;
//  }
//
//  /* 轨道输入 */
//  hParent = GetTreeItem(hTree, "麦克风", NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  snprintf(path, MAX_PATH, "麦克风.%s", gbk_name);
//  item_data = CreateItemData(14, "麦克风", gbk_name);
//  item_data->id = gbk_name;
//  item_data->db_click = on_mic_start;
//  hItem = AddTreeItem(hTree, path, gbk_name, item_data);
//
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  return hItem;
//}
//
//HTREEITEM AddSpeaerDevice(HWND hTree, bool add, const char *speaker)
//{
//  char              path[MAX_PATH] = { 0 };
//  tree_item_data_t *item_data;
//  HTREEITEM         hItem, hParent;
//  char              gbk_name[256] = { 0 };
//  utf8_to_gbk(speaker, gbk_name, sizeof(gbk_name));
//  if (!add) {
//    /* 移除 */
//    snprintf(path, MAX_PATH, "扬声器.%s", gbk_name);
//    hItem = GetTreeItem(hTree, path, &hParent);
//    if (hItem) {
//      /* 删除空节点 */
//      RemoveTreeItem(hTree, hItem);
//
//      hItem = TreeView_GetChild(hTree, hParent);
//      if (!hItem) {
//        /* 清空节点 */
//        RemoveTreeItem(hTree, hParent);
//      }
//    }
//
//    return NULL;
//  }
//
//  /* 轨道输入 */
//  hParent = GetTreeItem(hTree, "扬声器", NULL);
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  snprintf(path, MAX_PATH, "扬声器.%s", gbk_name);
//  item_data = CreateItemData(14, "扬声器", gbk_name);
//  item_data->id = gbk_name;
//  item_data->db_click = on_speaker_start;
//  hItem = AddTreeItem(hTree, path, gbk_name, item_data);
//
//  TreeView_Expand(hTree, hParent, TVE_EXPAND);
//
//  return hItem;
//}

//
///* 流状态通知： 更新树视图中媒体流节点 */
//int32_t
//on_stream_state(void *user_data, av_data_t *data)
//{
//  const char             *path, *stream;
//  std::string             state;
//  int32_t                 error;
//  HTREEITEM               hItem;
//
//  stream = data->get_str("stream");
//  state = data->get_str("state");
//  error = data->get_int32("err");
//
//  path = xcast_property_path(XCAST_STREAM, stream);
//  if (state == "connecting") {
//    /* 流状态： 连接中 */
//    hItem = AddTreeItem(main_app.hTreeView, path, stream, &xcast_properties[XCAST_STREAM]);
//    SetTreeItemIcon(main_app.hTreeView, hItem, 1);
//    TreeView_Expand(main_app.hTreeView, TreeView_GetParent(main_app.hTreeView, hItem), TVE_EXPAND);
//  } else if (state == "connected") {
//    /* 流状态： 连接成功 */
//    hItem = GetTreeItem(main_app.hTreeView, path, NULL);
//    SetTreeItemIcon(main_app.hTreeView, hItem, 2);
//
//    ///* 启动主路视频输出(开摄像头) */
//    //xcast_set_property(XCAST_STREAM_TRACK_START, stream, "video_out", true);
//  } else if (state == "closed") {
//    /* 流状态： 关闭 */
//    hItem = GetTreeItem(main_app.hTreeView, path, NULL);
//    TreeView_DeleteItem(main_app.hTreeView, hItem);
//  }
//
//  return AV_OK;
//}
//
///* 枚举所有流轨道 */
//void
//print_tracks(const char *stream)
//{
//  av_data_t         data;
//  const char       *path;
//  size_t            n;
//
//  path = av_format_path("stream.%s.track", stream);
//  data = av_get_property(path);
//
//  printf("[AVSDK_CONSOLE] %s track begin:\n", stream);
//  for (n = 0; n < data.size(); ++n) {
//    printf("[AVSDK_CONSOLE]  [%2d] track: %s\n", n + 1, data.str_at(n));
//  }
//
//  printf("[AVSDK_CONSOLE] %s track end:\n", stream);
//}
//
///* 流轨道状态通知： 更新树视图中媒体流轨道节点 */
//int32_t
//on_stream_track(void *user_data, av_data_t *data)
//{
//  const char       *path, *stream, *track;
//  std::string       evt;
//  stream = data->get_str("stream");
//  evt = data->get_str("event");
//
//  //if (evt == "add") {
//  //  track = data->get_str("track");
//
//
//
//
//  //  //if (strncmp(track, "sub_video", 9)) {
//  //  //  av_data_t params;
//  //  //  params.put("start", true);
//  //  //  path = av_format_path("stream.%s.track.%s.start", stream, track);
//  //  //  av_set_property(path, params);
//  //  //}
//  //} else if (0 == strncmp(evt, "remove", 6)) {
//  //  track = data->get_str("track");
//  //  printf("[AVSDK_CONSOLE] %s removed.\n", track);
//  //} else if (0 == strncmp(evt, "update", 6)) {
//  //  av_data_t         ap;
//  //  track = data->get_str("track");
//  //  /* query "start" state of track */
//  //  path = av_format_path("stream.%s.track.%s.start", stream, track);
//  //  ap = av_get_property(path);
//  //  printf("[AVSDK_CONSOLE] %s updated, start=%d.\n", track, ap.bool_val());
//  //}
//
//  return AV_OK;
//}

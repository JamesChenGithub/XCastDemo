#include "main-view.h"
#include "xcast-ui-handler.h"
#include "jansson.h"
#include <time.h>

#include "Live/XCastHelper.h"
#include "Live/XCastData.h"
#include "Live/XCastObserver.h"
#pragma comment(lib, "comctl32.lib")


#define MAX_LOADSTRING 100

// 3780 pixels per meter is equivalent to 96 DPI, typical on desktop monitors.
static const int kPixelsPerMeter = 3780;
// 32 bit RGBA is 4 bytes per pixel.
static const int kBytesPerPixel = 4;
static std::string role = "user";
static int32_t    appid;
static uint64_t   account;
static int32_t    relation_id;
static bool       test_env;
static bool       is_initialized = false;
static bool       is_stream_running = false;
static std::string peer_ip;
static uint16_t      peer_port = 0;
static bool       is_lan_start = false;

static LRESULT CALLBACK MainViewProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK VideoViewProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

static void TrackBufferRefreshLayers(TrackVideoBuffer *buffer, xcast_data &data);

void CalculateUpdateRect();
void RenderBuffer();
extern HWND CreateTabView(HWND hwndParent, HINSTANCE hInst);
extern HWND CreateTreeView();
extern LRESULT OnTreeViewMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT OnTabviewMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern void AdjustTabViewSize(HWND hTab);

int32_t read_file(const char* pFile, char** content);
int32_t write_file(const char* pFile, const char* mode, const char* content, uint32_t content_size);
void save_account();
int32_t read_account();

XCastApp   main_app;

std::shared_ptr<XCastObserver> main_observer;
//#define UDT_TEST    1
//#define SPEED
//#define  AUTH_BUFFER_TEST

xcast_data
xcast_get_start_param(void *user_data)
{
  xcast_data settings;
  //uint64_t     account;
 // account = read_account();
  //settings.put("log_path", "d:\\");
  //settings["log_level"] = 7;
#if defined(UDT_TEST)
  settings["app_id"] = 1400029763;
#elif defined (SPEED)
  settings["app_id"] = 1400036169;
#elif defined(AUTH_BUFFER_TEST)
  settings["app_id"] = 1400053721;
#else
  settings["app_id"] = appid;

#endif
  //account = 144115192392922958l;
  settings["identifier"] = account;
  settings["test_env"] = test_env;

  main_app.uin = account;

  return settings;
}

void
ui_xcast_err(int32_t err, const char *err_msg, void* user_data)
{
    char msg[256];
    snprintf(msg, sizeof(msg), "err(%d) errmsg(%s)", err, err_msg);
    MessageBoxA(NULL, msg, "[ui_xcast_err]", MB_OK);
}

xcast_data 
xcast_get_stream_param(void *user_data)
{
	xcast_data         params, auth_info, track;
	xc_auth_type auth_type = xc_auth_none;

#if defined(UDT_TEST)
  params["relation_id"] = 500012;
#elif defined (SPEED)
  params["relation_id"] = 500012;
#elif defined(AUTH_BUFFER_TEST)
  auth_type = xc_auth_auto;
  params["relation_id"] = 14158;
#else
  params["relation_id"] = relation_id;
#endif
  params["auth_type"] = auth_type;
  auth_info["auth_bits"] = -1;

  if (auth_type == xc_auth_manual) {
    auth_info.put("auth_buffer", (const uint8_t*)"aaa", 3);
  } else if(auth_type == xc_auth_auto) {
    if (appid == 1400046799) {
      auth_info["account_type"] = 18454;
      auth_info["expire_time"] = 1800;
      const  char *secret_key = "bc7b92440f80d6ac";
      auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
    } else if (appid == 1400053721) {
      auth_info["account_type"] = 19937;
      auth_info["expire_time"] = 1800;
      const  char *secret_key = "a6cbaccefd7cde6b";
      auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
    } else if(appid == 1400029761) {
      auth_info["account_type"] = 12348;
      auth_info["expire_time"] = 1800;
      const  char *secret_key = "3c283ebc6403e827";
      auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
    } else if (appid == 491400029763) {
      auth_info["account_type"] = 12346;
      auth_info["expire_time"] = 1800;
      const  char *secret_key = "49b38f929dc3f101";
      auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
    }
  }

  params["auto_recv"] = true;
  params.put("auth_info", auth_info);
  params["videomaxbps"] = 3000;
#if defined(UDT_TEST)
  params["role"] = "90991";
#elif defined(AUTH_BUFFER_TEST)
  params["role"] = "user";
#else
  params["role"] = "user";
#endif

#if defined(XCAST_EXTERNAL_VIDEO)
  /* stream track configuration */
  track["ext-video-capture"] = true;    /* allow video track to use external capture */
#endif

#if defined(XCAST_EXTERNAL_IO)
  /* stream track configuration */
  track["ext-audio-capture"] = true;    /* allow audio track to use external capture */
  track["ext-audio-playback"] = true;   /* allow audio track to use external playback */

  params["track"] = track;
#endif

  //xcast_data host_addr = xcast::get_property("lan-relay.host-addr");

  ///* bind local ip */
  //const char* ip = host_addr["ip"];      
  //uint16_t    port = host_addr["port"];
  //const char* gateway = host_addr["gateway"];
  ///* network card name */
  //const char* name = host_addr["name"];
  ///* network card describe */
  //const char* desc =  host_addr["desc"];
  ///* network card mac address */
  //const char* mac = host_addr["mac"];
  //printf("get host addr:ip(%s) port(%u) gateway(%s) name(%s) desc(%s) mac(%s)\n",
  //      ip, port, gateway, name, desc, mac);

  //int32_t      rt;

  ///* 设置音频输出参数 */
  //xcast_data_t audio_format;
  //audio_format["sample-rate"] = 16000;
  //audio_format["channel"] = 1;
  //audio_format["bits"] = 16;
  //rt = xcast_set_property("preference.audio.output-format", audio_format);

  const char *parmastr = params.dump();
  int i = 0;
  return params;
}

/* 媒体流状态： 连接中 */
void
ui_stream_connecting(const char *stream, void* user_data)
{
  XCastApp         *app = (XCastApp *)user_data;
  HTREEITEM         hItem, hParent;
  tree_item_data_t *item_data;
  char              path[XCAST_MAX_PATH] = { 0 };

  snprintf(path, XCAST_MAX_PATH, "stream.%s", stream);
  hItem = GetTreeItem(app->hTreeView, path, &hParent);
  if (!hItem) {
    item_data = CreateItemData(1, path, stream);
    item_data->is_stream = true;
    hItem = AddTreeItem(app->hTreeView, path, stream, item_data);
    TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
  }
}

/* 媒体流状态： 已连接 */
void
ui_stream_connected(const char *stream, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  HTREEITEM         hItem;

  snprintf(path, XCAST_MAX_PATH, "stream.%s", stream);
  hItem = GetTreeItem(app->hTreeView, path, NULL);
  SetTreeItemIcon(app->hTreeView, hItem, 2);
  TreeView_Expand(app->hTreeView, hItem, TVE_EXPAND);
}

void
ui_stream_closed(const char *stream, int32_t err, const char *err_msg, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  HTREEITEM         hItem;

  snprintf(path, XCAST_MAX_PATH, "stream.%s", stream);
  hItem = GetTreeItem(app->hTreeView, path, NULL);
  if (hItem) {
    RemoveTreeItem(app->hTreeView, hItem);
  }
}

/* 树控件双击事件: start/stop track */
static int32_t
on_start_track(tree_item_data_t *item_data, HTREEITEM hItem)
{
  xcast_data        data;
  char              prop[XCAST_MAX_PATH] = { 0 };

  item_data->start = !item_data->start;
  data["enable"] = item_data->start;
  xcast::set_property(XC_TRACK_ENABLE, item_data->id2.c_str(), item_data->id.c_str(), data);

  return XCAST_OK;
}

/* 树控件双击事件: start/stop track */
static int32_t
on_camera_preview(tree_item_data_t *item_data, HTREEITEM hItem)
{
  item_data->start = !item_data->start;
  xcast::set_property(XC_CAMERA_PREVIEW, item_data->text.c_str(), item_data->start);

  if (!item_data->start) {
    ClearTrackBuffer(item_data->text.c_str());
  }
  
  InvalidVideoView();
  return XCAST_OK;
}

/* 树控件双击事件: start/stop track */
static int32_t
on_screen_preview(tree_item_data_t *item_data, HTREEITEM hItem)
{
  item_data->start = !item_data->start;
  xcast::set_property(XC_SCREEN_CAPTURE_PREVIEW, item_data->start);

  if (!item_data->start) {
    ClearTrackBuffer(item_data->text.c_str());
  }

  InvalidVideoView();
  return XCAST_OK;
}

/* 树控件双击事件: start/stop track */
static int32_t
on_default_speaker(tree_item_data_t *item_data, HTREEITEM hItem)
{
  POINT           pt;
  HMENU           hmenu;   // handle to capture menu  
  xcast_data      speakers, def_speaker;
  const char     *dev;
  uint32_t        n;

  hmenu = CreatePopupMenu();
  def_speaker = xcast::get_property(XC_SPEAKER_DEFAULT);
  speakers = xcast::get_property(XC_SPEAKER);
  if (speakers.size()) {
    for (n = 0; n < speakers.size(); ++n) {
      dev = speakers[n].str_val();
      std::wstring strSpeaker;
      utf8_to_utf16(dev, strlen(dev), strSpeaker);
      if (def_speaker == dev) {
        strSpeaker = L"(默认)" + strSpeaker;
        AppendMenuW(hmenu, MF_STRING | MF_CHECKED, n + 1, strSpeaker.c_str());
      } else {
        AppendMenuW(hmenu, MF_STRING, n + 1, strSpeaker.c_str());
      }
    }
  } else {
    AppendMenuW(hmenu, MF_STRING | MF_CHECKED, 0, L"null");
  }

  GetCursorPos(&pt);
  n = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
    pt.x, pt.y, 0, main_app.hTreeView, NULL);
  DestroyMenu(hmenu);

  if (n && speakers.size() && n - 1 < speakers.size()) {
    xcast::set_property(XC_SPEAKER_DEFAULT, speakers[n - 1].str_val());
  }

  return XCAST_OK;
}

/* popup a capture select menu */
static int32_t
popup_capture_select(tree_item_data_t *item_data, HTREEITEM hItem)
{
  POINT           pt;
  HMENU           hmenu;   // handle to capture menu  
  xcast_data      captures;
  capture_data_t *data = (capture_data_t *)item_data->cap_data;
  const char     *cap;
  uint32_t        n;

  hmenu = CreatePopupMenu();

  if (data->track_type == xc_track_audio) {
    captures = xcast::get_property(XC_MIC);
    for (n = 0; n < captures.size(); ++n) {
      cap = captures[n].str_val();
      std::wstring strCap;
      utf8_to_utf16(cap, strlen(cap), strCap);
      if (data->current_capture == cap) {
        AppendMenuW(hmenu, MF_STRING | MF_CHECKED, n + 1, strCap.c_str());
      } else {
        AppendMenuW(hmenu, MF_STRING, n + 1, strCap.c_str());
      }
    }
  } else if (data->track_type == xc_track_video) {
    if (0 == data->track_index) {
      captures = xcast::get_property(XC_CAMERA);
      for (n = 0; n < captures.size(); ++n) {
        cap = captures[n].str_val();
        std::wstring strCap;
        utf8_to_utf16(cap, strlen(cap), strCap);
        if (data->current_capture == cap) {
          AppendMenuW(hmenu, MF_STRING | MF_CHECKED, n + 1, strCap.c_str());
        } else {
          AppendMenuW(hmenu, MF_STRING, n + 1, strCap.c_str());
        }
      }
    } else {
      AppendMenuW(hmenu, MF_STRING | MF_CHECKED, 0, L"screen-capture");
    }
  }

  GetCursorPos(&pt);
  n = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
    pt.x, pt.y, 0, main_app.hTreeView, NULL);
  DestroyMenu(hmenu);


  //xcast::set_property(XC_TRACK_CAPTURE, data->stream.c_str(), data->track.c_str(), captures[n].str_val());
  if (n && captures.size() && n - 1 < captures.size()) {
    xcast::set_property(XC_TRACK_CAPTURE, data->stream.c_str(), data->track.c_str(), captures[n - 1].str_val());
  }

  return XCAST_OK;
}

/* add/remove track in tree view */
void 
ui_track_add(xcast_data &evt, bool add, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  char              name[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  tree_item_data_t *item_data;
  HTREEITEM         hItem, hParent;
  const char       *track = evt["src"];
  const char       *stream = evt["stream"];
  uint64_t          uin = evt["uin"];
  int32_t           clazz = evt["class"];
  int32_t           index = evt["index"];
  int32_t           dir = evt["direction"];

  if (add) {
#if defined(XCAST_EXTERNAL_VIDEO)
    if (clazz == xc_track_video && dir == xc_track_out && index) {
      /* set 'ext1' as external capture for sub-video-out */
      path.format(XC_TRACK_CAPTURE, stream, track);
      rt = xcast_set_property(path, xcast_data_t("ext1"));
      printf("xcast_set_property '%s'='ext1', rt(%d)\n", path.str_val(), rt);
    }
#endif
   //if (clazz == xc_track_audio && dir == xc_track_out) {
   //  path.format(XC_TRACK_CAPTURE, stream, track);
   //  /* 设置ext1为外部采集,设置之后使用 */
   //  int32_t rt = xcast_set_property(path, xcast_data_t("ext1"));
   //  printf("ui_track_add path(%s) rt(%d)\n", path.str_val(), rt);
   //}
    
//#ifdef _DEBUG
//    if (dir == xc_track_out) {
//      xcast_data_t enable;
//      enable["enable"] = true;
//      path.format(XC_TRACK_ENABLE, stream, track);
//      xcast_set_property(path, enable);
//    }
//#else
//    if (dir == xc_track_in) {
//      xcast_data_t enable;
//      enable["enable"] = true;
//      path.format(XC_TRACK_ENABLE, stream, track);
//      xcast_set_property(path, enable);
//    }
//#endif

    /* create user node under 'stream' */
    snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu", stream, uin);
    hItem = GetTreeItem(app->hTreeView, path, &hParent);
    if (!hItem) {
      snprintf(name, XCAST_MAX_PATH, "%llu", uin);
      item_data = CreateItemData(main_app.uin == uin ? 13 : 14, path, name);
      AddTreeItem(app->hTreeView, path, name, item_data);
      TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
    }

    /* create track node under 'stream.user' */
    snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu.%s", stream, uin, track);
    hItem = GetTreeItem(app->hTreeView, path, &hParent);
    if (!hItem) {
      item_data = CreateItemData(9, path, track);
      item_data->id = track;
      item_data->id2 = stream;
      item_data->db_click = on_start_track;
      AddTreeItem(app->hTreeView, path, track, item_data);
      TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);

      /* create capture node for output track */
      if (xc_track_out == dir) {
        xcast_data        cap;
        const char       *capture = "null";
        capture_data_t   *data;
        hParent = hItem;
        snprintf(path, XCAST_MAX_PATH, XC_TRACK_CAPTURE, stream, track);
        cap = xcast::get_property(path);
        if (cap.size()) capture = cap.str_val();

        snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu.%s.%s", stream, uin, track, capture);
        item_data = CreateItemData(clazz == xc_track_audio ? 5 : 12, path, capture);
        item_data->id = stream;
        item_data->id2 = track;
        data = new capture_data_t();
        data->stream = stream;
        data->track = track;
        data->current_capture = capture;
        data->track_index = evt["index"];
        data->track_type = clazz;
        item_data->cap_data = data;
        item_data->db_click = popup_capture_select;
        AddTreeItem(app->hTreeView, path, capture, item_data);
        TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
      }
    }
  } else {
    snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu.%s", stream, uin, track);
    hItem = GetTreeItem(app->hTreeView, path, &hParent);
    if (hItem) {
      RemoveTreeItem(app->hTreeView, hItem);
      InvalidateRect(main_app.hVideoView, NULL, TRUE);
    }
  }
}

/* 更新轨道状态 */
void
ui_track_update(xcast_data &evt, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  const char       *track = evt["src"];
  const char       *stream = evt["stream"];
  int32_t           clazz = evt["class"];
  int32_t           state = evt["state"];
  int32_t           dir = evt["direction"];
  uint64_t          uin = evt["uin"];
  int32_t           err = evt["err"];
  HTREEITEM         hItem, hParent;

  snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu.%s", stream, uin, track);
  hItem = GetTreeItem(app->hTreeView, path, &hParent);
  if (!hItem) return;

  /* TODO: update track info here */
  tree_item_data_t *item_data = GetTreeItemData(app->hTreeView, hItem);

  if (state == xc_track_running) {
    /* set 'running' icon */
    item_data->start = true;
    SetTreeItemIcon(app->hTreeView, hItem, dir == xc_track_in ? 7 : 8);
  } else if (state == xc_track_stopped) {
    /* set 'stopped' icon */
    item_data->start = false;
    ClearTrackBuffer(track);
    SetTreeItemIcon(app->hTreeView, hItem, 9);
  }

  /* create capture node for output track */
  if ((int32_t)evt["type"] == xc_track_capture_changed && xc_track_out == dir) {
    const char       *capture = "null";
    capture_data_t   *data;
    xcast_data      name;

    hParent = hItem;
    snprintf(path, XCAST_MAX_PATH, XC_TRACK_CAPTURE, stream, track);
    name = xcast::get_property(path);
    if (name.size()) capture = name.str_val();

    hItem = TreeView_GetChild(app->hTreeView, hParent);
    if (hItem) RemoveTreeItem(app->hTreeView, hItem);

    snprintf(path, XCAST_MAX_PATH, "stream.%s.%llu.%s.%s", stream, uin, track, capture);
    item_data = CreateItemData(clazz == xc_track_audio ? 5 : 12, path, capture);
    item_data->id = stream;
    item_data->id2 = track;
    data = new capture_data_t();
    data->stream = stream;
    data->track = track;
    data->current_capture = capture;
    data->track_index = evt["index"];
    data->track_type = clazz;
    item_data->cap_data = data;
    item_data->db_click = popup_capture_select;
    TreeView_SelectItem(app->hTreeView, AddTreeItem(app->hTreeView, path, capture, item_data));

    return;
  }

  TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);

  if (err) ui_xcast_err(err, evt["err-msg"], user_data);
}

/* 媒体流轨道数据通知 */
int32_t
ui_track_media(xcast_data &evt, void *user_data)
{
  TrackVideoBuffer *buffer;
  const char        *src = evt["src"];
  int32_t           type = evt["type"];
  int32_t           clazz = evt["class"];
  int32_t           direction = evt["direction"];

  if (clazz == xc_track_audio) {
    if (direction == xc_track_in) {
      /* 在这里播放音频数据 */
      const char* pcm_data = evt["data"];
      uint32_t     pcm_size = evt["size"];
      uint32_t     sample_rate = evt["sample-rate"];
      uint32_t     channel = evt["channel"];
      uint32_t     bits = evt["bits"];
      uint64_t     uin = evt["uin"];
      printf("ui_track_media: pcm_size(%u) sample_rate(%u) channel(%u) bits(%u) uin(%llu)\n",
            pcm_size, sample_rate, channel, bits, uin);
    }
  } else if (clazz == xc_track_video) {
    int32_t fmt = (int32_t)evt["format"];
    if (fmt == xc_media_argb32) {
      buffer = GetTrackBuffer(evt["src"], evt["width"], evt["height"]);
      memcpy(buffer->data, (const uint8_t *)evt["data"], (uint32_t)evt["size"]);
      InvalidVideoView(&buffer->rcOut);
    } else if (fmt == xc_media_layer) {
      TrackVideoBuffer *buffer = GetTrackBuffer(evt["src"], evt["width"], evt["height"], true);
      TrackBufferRefreshLayers(buffer, evt["data"]);
    }
  }
  return XCAST_OK;
}

int32_t
ui_mic_preprocess(const char *camera, const char *format,
  const uint8_t *frame_data, uint32_t frame_size,
  int32_t width, void *user_data)
{

  return XCAST_OK;
}

/* get device path in tree view and proper icon */
static bool
get_dev_path(const char *dev, int32_t clazz, int32_t state,
  int32_t *icon, char *path)
{
  /* create device node under 'device' */
  switch (clazz) {
  case xc_device_camera:
    *icon = state == xc_device_stopped ? 12 : 17;
    snprintf(path, XCAST_MAX_PATH, "device.camera.%s", dev);
    break;
  case xc_device_screen_capture:
    *icon = state == xc_device_stopped ? 12 : 17;
    snprintf(path, XCAST_MAX_PATH, "device.%s", dev);
    break;
  case xc_device_mic:
    *icon = state == xc_device_stopped ? 5 : 19;
    snprintf(path, XCAST_MAX_PATH, "device.mic.%s", dev);
    break;
  case xc_device_speaker:
    *icon = state == xc_device_stopped ? 18 : 11;
    snprintf(path, XCAST_MAX_PATH, "device.speaker.%s", dev);
    break;
  case xc_device_external:
    *icon = 16;
    snprintf(path, XCAST_MAX_PATH, "device.external.%s", dev);
    break;
  default:
    return false;
  }

  return true;
}

void
ui_device_added(const char *dev, int32_t clazz, bool add, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  tree_item_data_t *item_data;
  HTREEITEM         hItem, hParent;
  int32_t           icon = 0;

  if (!dev || !get_dev_path(dev, clazz, xc_device_stopped, &icon, path)) return;

  hItem = GetTreeItem(app->hTreeView, path, &hParent);
  if (add) {
    if (clazz == xc_device_screen_capture) {
      xcast_data    screen_conf;
      screen_conf["fps"] = 30;
      xcast::set_property(XC_SCREEN_CAPTURE_SETTING, screen_conf);
    }

    if (!hItem) {
      item_data = CreateItemData(icon, path, dev);
      switch (clazz) {
      case xc_device_camera:
        item_data->db_click = on_camera_preview;
        break;
      case xc_device_screen_capture:
        item_data->db_click = on_screen_preview;
        break;
      case xc_device_mic:
        break;
      case xc_device_speaker:
        item_data->db_click = on_default_speaker;
        break;
      case xc_device_external:
        break;
      default:
        break;
      }

      item_data->start = false;
      AddTreeItem(app->hTreeView, path, dev, item_data);
      TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
    }
  } else {
    if (hItem) {
      RemoveTreeItem(app->hTreeView, hItem);
      TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
    }
  }
}

void 
ui_device_update(const char *dev, int32_t clazz, int32_t state,
  int32_t err, const char *err_msg, void* user_data)
{
  char              path[XCAST_MAX_PATH] = { 0 };
  XCastApp         *app = (XCastApp *)user_data;
  tree_item_data_t *item_data;
  HTREEITEM         hItem, hParent;
  int32_t           icon = 0;

  if (!dev || !get_dev_path(dev, clazz, state, &icon, path)) return;

  /* create camera node under 'device.camera' */
  hItem = GetTreeItem(app->hTreeView, path, &hParent);
  if (hItem) {
    item_data = GetTreeItemData(app->hTreeView, hItem);

    if (state == xc_device_running) {
      /* set 'running' icon */
      item_data->start = true;
      SetTreeItemIcon(app->hTreeView, hItem, icon);
    } else if (state == xc_device_stopped) {
      /* set 'stopped' icon */
      item_data->start = false;
      ClearTrackBuffer(dev);
      SetTreeItemIcon(app->hTreeView, hItem, icon);
      InvalidVideoView();
    }

    TreeView_Expand(app->hTreeView, hParent, TVE_EXPAND);
  }
}

int32_t
ui_device_preprocess(xcast_data &evt, void *user_data)
{
  const char       *dev = evt["src"];

  if (!dev) return XCAST_ERR_INVALID_ARGUMENT;

  switch ((int32_t)evt["class"]) {
  case xc_device_mic:
    /* 麦克风数据预处理 */
    //ui_mic_preprocess(evt["src"], evt["format"],
    //  evt["data"], evt["size"], evt["width"], evt["height"], user_data);
    break;
  default:
    break;
  }

  return XCAST_OK;
}

int32_t
ui_device_preview(xcast_data &evt, void *user_data)
{
  const char       *dev = evt["src"];

  if (!dev) return XCAST_ERR_INVALID_ARGUMENT;

  switch ((int32_t)evt["class"]) {
  case xc_device_camera:
  case xc_device_screen_capture: {
    /* 摄像头预览数据渲染 */
    int32_t         width = evt["width"];
    int32_t         height = evt["height"];
    int32_t         format = evt["format"];

    if (format == xc_media_argb32) {
      TrackVideoBuffer *buffer = GetTrackBuffer(dev, width, height);
      memcpy(buffer->data, evt["data"].bytes_val(), width * height * kBytesPerPixel);
      InvalidVideoView(&buffer->rcOut);
    } else if (format == xc_media_layer) {
      TrackVideoBuffer *buffer = GetTrackBuffer(dev, width, height, true);
      TrackBufferRefreshLayers(buffer, evt["data"]);
    }
    break;
  }
  case xc_device_mic:
    break;
  default:
    break;
  }

  return XCAST_OK;
}

tree_item_data_t *
CreateItemData(int32_t icon, const char *path, const char *text)
{
  tree_item_data_t   *item_data = new tree_item_data_t();

  item_data->icon = icon;
  item_data->path = path;
  item_data->text = text;
  item_data->prop = NULL;
  item_data->start = false;
  item_data->db_click = NULL;

  return item_data;
}

/* 创建工具栏窗口 */
static HWND 
CreateToolbar(HWND hwndParent, HINSTANCE hInst)
{
  HWND hToolbar;
  TBBUTTON tbb[7];
  HIMAGELIST hImageList, hHotImageList, hDisableImageList;
  HBITMAP hBitmap;

  // 创建Toolbar控件
  hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, TEXT("toolbar"),
                            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE |
                            TBSTYLE_LIST | TBSTYLE_AUTOSIZE | TBSTYLE_TOOLTIPS,
                            0, 0, 0, 0,
                            hwndParent,
                            (HMENU)IDC_TOOLBAR,
                            hInst,
NULL);

if (!hToolbar) { return NULL; }

SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
// 下面创建三组24x24像素大小的位图图像列表，用于工具栏图标
hImageList = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 3, 1);
hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR));
ImageList_AddMasked(hImageList, hBitmap, RGB(255, 255, 255));
DeleteObject(hBitmap);
SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList); // 正常显示时的图像列表
hHotImageList = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 3, 1);
hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR));
ImageList_AddMasked(hHotImageList, hBitmap, RGB(255, 255, 255));
DeleteObject(hBitmap);
SendMessage(hToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)hHotImageList); // 鼠标悬浮时的图像列表
hDisableImageList = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 3, 1);
hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR));
ImageList_AddMasked(hDisableImageList, hBitmap, RGB(255, 255, 255));
DeleteObject(hBitmap);
SendMessage(hToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hDisableImageList); //当工具栏button失能是的图像列表
ZeroMemory(tbb, sizeof(tbb));
int32_t index = 0;
tbb[index].iBitmap = MAKELONG(3, 0);
tbb[index].fsState = TBSTATE_ENABLED;
tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
tbb[index].idCommand = ID_INIT;
tbb[index].iString = (INT_PTR)TEXT("启动");
index++;

//tbb[index].iBitmap = MAKELONG(4, 0);
//tbb[index].fsState = TBSTATE_ENABLED;
//tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
//tbb[index].idCommand = ID_CLOSESTREAM;
//tbb[index].iString = (INT_PTR)TEXT("停止");
//index++;

tbb[index].iBitmap = MAKELONG(3, 0);
tbb[index].fsState = TBSTATE_ENABLED;
tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
tbb[index].idCommand = ID_STARTSTREAM;
tbb[index].iString = (INT_PTR)TEXT("进房");
index++;

//tbb[index].iBitmap = MAKELONG(4, 0);
//tbb[index].fsState = TBSTATE_ENABLED;
//tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
//tbb[index].idCommand = ID_CLOSESTREAM;
//tbb[index].iString = (INT_PTR)TEXT("退房");
//index++;

tbb[index].iBitmap = MAKELONG(3, 0);
tbb[index].fsState = TBSTATE_ENABLED;
tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
tbb[index].idCommand = ID_STARTLANRELAY;
tbb[index].iString = (INT_PTR)TEXT("开启局域网");
index++;

//tbb[index].iBitmap = MAKELONG(4, 0);
//tbb[index].fsState = TBSTATE_ENABLED;
//tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
//tbb[index].idCommand = ID_STOPLANRELAY;
//tbb[index].iString = (INT_PTR)TEXT("停止局域网");
//index++;

tbb[index].iBitmap = MAKELONG(6, 0);
tbb[index].fsState = TBSTATE_ENABLED;
tbb[index].fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE;
tbb[index].idCommand = IDM_ABOUT;
tbb[index].iString = (INT_PTR)TEXT("关于");
index++;

SendMessage(hToolbar, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM)&tbb); //配置工具栏按钮信息
SendMessage(hToolbar, WM_SIZE, 0, 0);

main_app.hImageList = hImageList;

return hToolbar;
}

/* 创建状态栏窗口 */
static HWND
CreateStatusBar(HWND hwndParent, HINSTANCE hInst)
{
#define PANEL_NUM 3
  int array[PANEL_NUM] = { 120, 120 * 2, -1 };
  //创建Statusbar控件
  HWND hWndStatus = CreateWindowEx(0, STATUSCLASSNAME, TEXT(""),
    WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | WS_VISIBLE,
    0, 0, 0, 0, hwndParent, (HMENU)IDC_STATUSBAR, hInst, NULL);

  if (hWndStatus) {
    SendMessage(hWndStatus, SB_SETPARTS, (WPARAM)PANEL_NUM, (LPARAM)array); //设置面板个数
    SendMessage(hWndStatus, SB_SETTEXT, (LPARAM)1, (WPARAM)TEXT("panel-1")); //设置第二个面板内容
    SendMessage(hWndStatus, SB_SETTEXT, (LPARAM)2, (WPARAM)TEXT("panel-2")); //设置第三个面板内容
  }

#undef PANEL_NUM
  return hWndStatus;
}

// “关于”框的消息处理程序。
static INT_PTR CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);

  switch (message) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }

    break;
  }

  return (INT_PTR)FALSE;
}

// “关于”框的消息处理程序。
static INT_PTR CALLBACK
Role(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
    if (role.length()) {
      std::wstring             szText;

      utf8_to_utf16(role.c_str(), role.length(), szText);

      SetDlgItemText(
        hDlg, IDC_EDIT_ROLE,
        szText.c_str());
    }
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
        WCHAR             szText[MAX_LOADSTRING] = { 0 };
        WCHAR*            ptr = szText;
        int               len;

        len = GetDlgItemText(
          hDlg, IDC_EDIT_ROLE, 
          szText, MAX_LOADSTRING);

        if (len) utf16_to_utf8((char16_t*)ptr, len, role);
      } else if (LOWORD(wParam) == IDCANCEL) {

      }

      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }

    break;
  }

  return (INT_PTR)FALSE;
}

void updateLanRelayButtonState()
{

  HWND hToolbar = FindWindowEx(main_app.hMainFrame, NULL, TOOLBARCLASSNAME, NULL);
  TBBUTTONINFO tbInfo;
  tbInfo.cbSize = sizeof(TBBUTTONINFO);
  tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
  tbInfo.iImage = is_lan_start ? MAKELONG(4, 0) : MAKELONG(3, 0);
  tbInfo.pszText = is_lan_start ? TEXT("停止局域网") : TEXT("启动局域网");
  SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_STARTLANRELAY, (LPARAM)&tbInfo);

}

static INT_PTR CALLBACK
LanRelaySetting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
  {
    WCHAR buffer[256] = {0};
    std::wstring str_ip;
     
    utf8_to_utf16(peer_ip.c_str(), peer_ip.size(), str_ip);
    //swprintf(buffer, wcslen(buffer), L"%s", peer_ip.c_str());
    SetDlgItemText(hDlg, IDC_EDIT_PEER_IP, str_ip.c_str());

    swprintf(buffer, 256, L"%u", peer_port);
    SetDlgItemText(hDlg, IDC_EDIT_PEER_PORT, buffer);

    return (INT_PTR)TRUE;
  }

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
        WCHAR             szText[MAX_LOADSTRING] = { 0 };
        WCHAR*            ptr = szText;
        std::string ip; 
        uint16_t port;

        GetDlgItemText(hDlg, IDC_EDIT_PEER_IP, szText, MAX_LOADSTRING);
        if (!wcslen(szText)) {
          MessageBox(hDlg, L"peer ip is invalid", L"",  MB_OK);
          return TRUE;
        }
        utf16_to_utf8((const char16_t*)szText, wcslen(szText), ip);
   
        GetDlgItemText(hDlg, IDC_EDIT_PEER_PORT, szText, MAX_LOADSTRING);
        port = _wtoi(szText);
        if (port <= 0) {
          MessageBox(hDlg, L"peer port is invalid", L"", MB_OK);
          return TRUE;
        }

        if (peer_ip != ip || peer_port != port) {
          peer_ip = ip;
          peer_port = port;
          save_account();
        }

        if (!is_initialized) {
          MessageBox(hDlg, L"sdk is not initialized", L"", MB_OK);
          return TRUE;
        }

        if (XCAST_OK != ui_start_lan_relay(true, peer_ip.c_str(), peer_port)) {
          MessageBox(hDlg, L"Fail to start lan relay", L"", MB_OK);
          is_lan_start = false;
          return TRUE;
        }

        is_lan_start = true;
        updateLanRelayButtonState();

      } else if (LOWORD(wParam) == IDCANCEL) {

      }

      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }

    break;
  }

  return (INT_PTR)FALSE;
}

static INT_PTR CALLBACK
AppidSetting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
  {
    WCHAR buffer[256] = {0};
    HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO_APPID_LIST);
    swprintf(buffer, 256, L"%u", appid);
    SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)buffer);
    SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);
    
    swprintf(buffer, 256, L"%llu", account);
    SetDlgItemText(hDlg, IDC_EDIT_ACCOUNT, buffer);

    swprintf(buffer, 256, L"%u", relation_id);
    SetDlgItemText(hDlg, IDC_EDIT_RELATION_ID, buffer);

    HWND hWndCheckBox = GetDlgItem(hDlg, IDC_CHECK_TEST);
    SendMessage(hWndCheckBox, BM_SETCHECK, test_env ? BST_CHECKED : BST_UNCHECKED, 0);

    return (INT_PTR)TRUE;
  }

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
        WCHAR             szText[MAX_LOADSTRING] = { 0 };
        WCHAR*            ptr = szText;
        int32_t           nAppid, nRelationId;
        uint64_t          nAccount;
        if (is_initialized) {

        }
        HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO_APPID_LIST);
        DWORD sel = SendMessage(hWndComboBox, CB_GETCURSEL, 0,0);
        SendMessage(hWndComboBox, CB_GETLBTEXT, sel, (LPARAM)szText);
        nAppid = _wtoi(szText);
        if (nAppid <= 0) {
          MessageBox(hDlg, L"", L"appid is invalid", MB_OK);
          return TRUE;
        }

        GetDlgItemText(hDlg, IDC_EDIT_ACCOUNT, szText, MAX_LOADSTRING);
        nAccount = _wtoll(szText);
        if (nAccount <= 0) {
          MessageBox(hDlg, L"", L"account is invalid", MB_OK);
          return TRUE;
        }

        GetDlgItemText(hDlg, IDC_EDIT_RELATION_ID, szText, MAX_LOADSTRING);
        nRelationId = _wtoi(szText);
        if (nRelationId <= 0) {
           MessageBox(hDlg, L"", L"relation_id is invalid", MB_OK);
           return TRUE;
        }

        HWND hWndCheckBox = GetDlgItem(hDlg, IDC_CHECK_TEST);
        test_env = BST_CHECKED == SendMessage(hWndCheckBox, BM_GETCHECK, 0, test_env);

        appid = nAppid;
        account = nAccount;
        relation_id = nRelationId;
        save_account();

        WCHAR szTitle[MAX_LOADSTRING];          
        WCHAR szWindowClass[MAX_LOADSTRING]; 
        LoadStringW(main_app.hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(main_app.hInstance, IDC_XCASTDEV, szWindowClass, MAX_LOADSTRING);
        read_account();
        swprintf(szTitle + wcslen(szTitle), MAX_LOADSTRING - wcslen(szTitle), L"-%u-%u-%llu-%d", appid, relation_id, account,test_env);
        HWND hWnd = FindWindow(szWindowClass, NULL);
        SetWindowText(hWnd, szTitle);

      } else if (LOWORD(wParam) == IDCANCEL) {

      }

      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }

    break;
  }

  return (INT_PTR)FALSE;
}

/* 注册主窗口类 */
static ATOM
MainViewRegisterClass(HINSTANCE hInstance, LPCWSTR szWindowClass)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = 0;
  wcex.lpfnWndProc = MainViewProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XCASTDEV));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_XCASTDEV);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

static void
AutoResizeViews()
{
  RECT rcClient, rcToolbar, rcStatusbar;
  GetClientRect(main_app.hMainFrame, &rcClient);
  GetClientRect(main_app.hToolbar, &rcToolbar);
  GetClientRect(main_app.hStatusbar, &rcStatusbar);
  MoveWindow(main_app.hTreeView, rcClient.left, rcToolbar.bottom,
    240, rcClient.bottom - rcToolbar.bottom - rcStatusbar.bottom, TRUE);
  MoveWindow(main_app.hTabview, 240, rcToolbar.bottom,
    rcClient.right - 240, rcClient.bottom - rcToolbar.bottom - rcStatusbar.bottom, TRUE);
  AdjustTabViewSize(main_app.hTabview);

  TabCtrl_AdjustRect(main_app.hTabview, FALSE, &rcClient);
  MoveWindow(main_app.hVideoView, rcClient.left, rcClient.top, 
    rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE);

  GetClientRect(main_app.hVideoView, &rcClient);
  main_app.cx = rcClient.right - rcClient.left;
  main_app.cy = rcClient.bottom - rcClient.top;

  if (main_app.hBitmapView) DeleteObject(main_app.hBitmapView);
  main_app.hBitmapView = CreateCompatibleBitmap(main_app.hDCView, main_app.cx, main_app.cy);

  InvalidVideoView();
}

/* 创建主窗口 */
BOOL 
InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  WCHAR szTitle[MAX_LOADSTRING] = {0};                  // 标题栏文本
  WCHAR szWindowClass[MAX_LOADSTRING] = {0};            // 主窗口类名
  main_app.hInstance = hInstance; // 将实例句柄存储在全局变量中
  // 初始化全局字符串
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_XCASTDEV, szWindowClass, MAX_LOADSTRING);
  read_account();
  swprintf(szTitle + wcslen(szTitle), MAX_LOADSTRING - wcslen(szTitle), L"-%u-%u-%llu-%d", appid, relation_id, account, test_env);
  MainViewRegisterClass(hInstance, szWindowClass);
  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_EX_COMPOSITED,
                            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
                            
  if (!hWnd) { 
    HLOCAL LocalAddress = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, GetLastError(), 0, (PTSTR)&LocalAddress, 0, NULL);
    WCHAR szText[MAX_LOADSTRING];
    swprintf(szText, wcslen(szText), L"Create Window Fail: errcode(%d), errmsg(%s)", GetLastError(), (PTSTR)LocalAddress);
    MessageBox(NULL, szText, L"ERROR", MB_OK);
    return FALSE;
 }
  
  main_app.hMainFrame = hWnd;
  main_app.hToolbar = CreateToolbar(hWnd, hInstance);
  if (!main_app.hToolbar) { return FALSE; }
  
  main_app.hStatusbar = CreateStatusBar(hWnd, hInstance);
  if (!main_app.hStatusbar) { return FALSE; }
  
  main_app.hTreeView = CreateTreeView();
  if (!main_app.hTreeView) { return FALSE; }

  main_app.hTabview = CreateTabView(hWnd, hInstance);
  if (!main_app.hTabview) { return FALSE; }
  SetWindowLong(main_app.hVideoView, GWL_WNDPROC, (LONG_PTR)&VideoViewProc);

  //main_app.hVideoView = CreateWindowEx(WS_EX_CLIENTEDGE,
  //  _T("Static"), NULL,
  //  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_MULTILINE,
  //  0, 0, 0, 0,
  //  main_app.hTabview, NULL, hInstance, NULL);
  //main_app.hDCView = GetDC(main_app.hVideoView);
  //main_app.hMemDC = CreateCompatibleDC(main_app.hDCView);
  //main_app.hDCCompatible = CreateCompatibleDC(main_app.hDCView);
  
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  main_app.hMainFrame = hWnd;
  AutoResizeViews();
  main_app.tips_on = false;

  return TRUE;
}

static TrackVideoBuffer *
ClearTrackBuffer(const char *track, bool clear_content)
{
  TrackVideoBuffer *buffer;

  for (int i = 0; i < MAX_TRACK_VIDEO; i++)
  {
    buffer = &main_app.trackVideos[i];
    if (!track || buffer->track == track) {
      buffer->track.clear();
      if (clear_content) {
        buffer->width = buffer->height = 0;
        buffer->data = NULL;
        DeleteObject(buffer->hBitmapTarget);
      }
    }
  }

  return buffer;
}

static void 
CreateMemoryBitmap(int width, int height, HBITMAP *hbmp, void **data)
{
  // Dimensions of screen.
  const int bytes_per_row = width * kBytesPerPixel;

  // Create a device independent bitmap (DIB) that is the same size.
  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = kBytesPerPixel * 8;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = bytes_per_row * height;
  bmi.bmiHeader.biXPelsPerMeter = kPixelsPerMeter;
  bmi.bmiHeader.biYPelsPerMeter = kPixelsPerMeter;

  // Create memory for the buffers.
  *hbmp = CreateDIBSection(main_app.hDCView, &bmi, DIB_RGB_COLORS, data, NULL, 0);
}

static void
TrackVideoLayerCreateBitmap(TrackVideoLayer *layer, xcast_data &layer_data)
{
  layer->format = layer_data["format"].uint8_val();
  layer->blend_mode = layer_data["blend-mode"].uint8_val();
  layer->width = layer_data["width"].uint32_val();
  layer->height = layer_data["height"].uint32_val();
  CreateMemoryBitmap(layer->width, layer->height, &layer->hBitmapCache,
    reinterpret_cast<void**>(&layer->data));
}

static void
TrackVideoLayerDestroyBitmap(TrackVideoLayer *layer)
{
  DeleteObject(layer->hBitmapCache);
  layer->format = 0;
  layer->blend_mode = 0;
  layer->width = 0;
  layer->height = 0;
}

static void 
TrackVideoLayerCreate(TrackVideoLayer *layer, xcast_data &layer_data)
{
  memset(layer, 0, sizeof(*layer));
  TrackVideoLayerCreateBitmap(layer, layer_data);
}

static void
TrackVideoLayerDestroy(TrackVideoLayer *layer) 
{
  TrackVideoLayerDestroyBitmap(layer);
  memset(layer, 0, sizeof(*layer));
}

static void
TrackVideoLayerRefresh(TrackVideoLayer *layer, xcast_data &layer_data, 
  int parentWidth, int parentHeight, const RECT *rcBorder)
{
  int parentOutWidth, parentOutHeight;
  const uint8_t *data = layer_data["data"].bytes_val();
  const int32_t format = layer_data["format"].int32_val();
  const int32_t blend_mode = layer_data["blend-mode"].int32_val();
  const uint32_t newWidth = layer_data["width"].uint32_val();
  const uint32_t newHeight = layer_data["height"].uint32_val();
  auto &rcLastOut = layer->rcLastOut;
  auto &rcOut = layer->rcOut;
  if (data) {
    if (layer->format != format ||
      layer->blend_mode != blend_mode ||
      layer->width != newWidth ||
      layer->height != newHeight
      ) {
      TrackVideoLayerDestroyBitmap(layer);
      TrackVideoLayerCreateBitmap(layer, layer_data);
    }
    if (layer->data) {
      memcpy(layer->data, data, layer->width * layer->height * kBytesPerPixel);
    }
  } else {
    layer->left = layer_data["left"].double_val();
    layer->top = layer_data["top"].double_val();
  }

  if (parentWidth != 0 && parentHeight != 0) {
    rcLastOut = rcOut;
    parentOutWidth = rcBorder->right - rcBorder->left;
    parentOutHeight = rcBorder->bottom - rcBorder->top;
    rcOut.left = (int)(layer->left * parentOutWidth / parentWidth) + rcBorder->left;
    rcOut.top = (int)(layer->top * parentOutHeight / parentHeight) + rcBorder->top;
    rcOut.right = rcOut.left + layer->width * parentOutWidth / parentWidth;
    rcOut.bottom = rcOut.top + layer->height * parentOutHeight / parentHeight;
    InvalidVideoView(&layer->rcLastOut);
    InvalidVideoView(&layer->rcOut);
  }
}

static void
TrackVideoLayerDraw(TrackVideoLayer *layer) 
{
  HGDIOBJ oldDC = SelectObject(main_app.hDCCompatible, layer->hBitmapCache);
  auto &rcOut = layer->rcOut;
  if (layer->blend_mode) {
    StretchBlt(main_app.hMemDC, rcOut.left, rcOut.top, rcOut.right - rcOut.left, rcOut.bottom - rcOut.top,
      main_app.hDCCompatible, 0, 0, layer->width, layer->height, SRCINVERT);
  } else {
    BLENDFUNCTION blendFunc = { 0 };
    blendFunc.BlendOp = AC_SRC_OVER;
    blendFunc.BlendFlags = 0;
    blendFunc.AlphaFormat = AC_SRC_ALPHA;
    blendFunc.SourceConstantAlpha = 0xFF;
    AlphaBlend(main_app.hMemDC, rcOut.left, rcOut.top, rcOut.right - rcOut.left, rcOut.bottom - rcOut.top,
      main_app.hDCCompatible, 0, 0, layer->width, layer->height, blendFunc);
  }

  SelectObject(main_app.hDCCompatible, oldDC);
}

static void
TrackBufferRemoveLayers(TrackVideoBuffer *buffer) 
{
  for (size_t i = 0; i < buffer->layers.size(); ++i) {
    TrackVideoLayerDestroy(&buffer->layers[i]);
  }
  buffer->layers.clear();
}

static void
TrackBufferRefreshLayers(TrackVideoBuffer *buffer, xcast_data &data) 
{
  if (buffer->layers.size() > data.size()) {
    const size_t len = buffer->layers.size() - data.size();
    auto last = buffer->layers.end();
    for (size_t i = 0; i < len; ++i) {
      --last;
      TrackVideoLayerDestroy(&(*last));
    }
    buffer->layers.resize(data.size());
  } else if (buffer->layers.size() < data.size()) {
    buffer->layers.resize(data.size());
    for (size_t i = buffer->layers.size(); i < data.size(); ++i) {
      TrackVideoLayerCreate(&(buffer->layers[i]), data.at(i));
    }
  }

  CalculateUpdateRect();
  for (size_t i = 0; i < buffer->layers.size(); ++i) {
    TrackVideoLayerRefresh(&buffer->layers[i], data.at(i), 
      buffer->width, buffer->height, &(buffer->rcOut));
  }
}

static void
TrackBufferDraw(TrackVideoBuffer *buffer, bool bottom)
{
  RECT rcText;
  if (bottom != buffer->bottom || buffer->track.empty()) {
    return;
  }
  HGDIOBJ bitmap_compact_old = SelectObject(main_app.hDCCompatible, buffer->hBitmapTarget);

  BitBlt(main_app.hMemDC, buffer->rcOut.left, buffer->rcOut.top,
    buffer->rcOut.right - buffer->rcOut.left, buffer->rcOut.bottom - buffer->rcOut.top,
    main_app.hDCCompatible, 0, 0, SRCCOPY);

  for (auto &layer: buffer->layers) {
    TrackVideoLayerDraw(&layer);
  }

  FrameRect(main_app.hMemDC, &buffer->rcOut, (HBRUSH)GetStockObject(WHITE_BRUSH));

  SetRect(&rcText, buffer->rcOut.left, buffer->rcOut.top, 240, 32);
  DrawText(main_app.hMemDC, buffer->text.c_str(), -1, &rcText, DT_NOCLIP);
  SelectObject(main_app.hDCCompatible, bitmap_compact_old);
}

/* 创建指定尺寸的轨道视频帧缓冲 */
static TrackVideoBuffer *
TrackBufferResize(TrackVideoBuffer *buffer, int width, int height)
{
  buffer->width = width;
  buffer->height = height;
  buffer->data = NULL;
  if (buffer->hBitmapTarget) DeleteObject(buffer->hBitmapTarget);

  // Create a bitmap to keep the desktop image.
  CreateMemoryBitmap(width, height, &(buffer->hBitmapTarget), &(buffer->data));
  return buffer;
}

/* 从数组获取轨道视频帧缓冲，如果没有则创建之 */
TrackVideoBuffer *
GetTrackBuffer(const char *track, int width, int height, bool bNoResize)
{
  TrackVideoBuffer *buffer;

  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    buffer = &main_app.trackVideos[i];
    if (buffer->track == track) {
      if (!bNoResize && 
        (!buffer->data || buffer->width != width || buffer->height != height)) {
        TrackBufferResize(buffer, width, height);
        CalculateUpdateRect();
      }
      /* 数组中找到了对应的轨道 */
      return buffer;
    }
  }

  /* 数组中没找到对应的轨道，寻找一个空缺创建新的 */
  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    buffer = &main_app.trackVideos[i];
    if (buffer->track.empty()) {
      buffer->track = track;
      utf8_to_utf16(track, buffer->track.size(), buffer->text);
      TrackBufferResize(buffer, width, height);
      CalculateUpdateRect();
      return buffer;
    }
  }

  /* 缓冲区数组已满，丢弃 */
  return NULL;
}

void
ClearTrackBuffer(const char *track)
{
  TrackVideoBuffer *buffer;
  if (!track) return;

  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    buffer = &main_app.trackVideos[i];
    if (buffer->track == track) {
      TrackBufferRemoveLayers(buffer);
      buffer->track.clear();
      buffer->moved = false;
      ZeroMemory(buffer->data, buffer->width * buffer->height * kBytesPerPixel);
      return;
    }
  }
}

void
InvalidVideoView(const RECT *rc)
{
  RECT rcUpdate;
  if (!main_app.hVideoView) {
    return;
  }

  if (rc) {
    memcpy(&rcUpdate, rc, sizeof(rcUpdate));
  } 
  else
  {
    GetClientRect(main_app.hVideoView, &rcUpdate);
  }
  InvalidateRect(main_app.hVideoView, &rcUpdate, FALSE);
}

void
CalculateUpdateRect()
{
  int32_t       cx = 0;
  int32_t       cy = 0;
  int32_t       x = 0;
  int32_t       y = 0;
  int32_t       track = 0;

  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    TrackVideoBuffer* buffer = &main_app.trackVideos[i];
    if (!buffer->track.empty()) {
      if (buffer->track != "screen-capture" &&
        strncmp(buffer->track.c_str(), "sub", 3)) {
        if (cx < buffer->width) cx = buffer->width;
        if (cy < buffer->height) cy = buffer->height;
        buffer->bottom = false;
      } else {
        buffer->bottom = true;
      }

      ++track;
    }
  }

  /* 画面排布 */
  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    TrackVideoBuffer* buffer = &main_app.trackVideos[i];
    if (!buffer->track.empty()) {
      if (buffer->moved) {
        buffer->rcOut.right = buffer->rcOut.left + buffer->width;
        buffer->rcOut.bottom = buffer->rcOut.top + buffer->height;
        continue;
      }

      if (!buffer->bottom) {
        buffer->rcOut.left = x;
        buffer->rcOut.right = x + cx;
        buffer->rcOut.top = y;
        buffer->rcOut.bottom = y + cy;

        if (track <= 2) {
          /* 两路画面排布 */
          if (main_app.cx < main_app.cy) {
            y += cy;
          } else {
            x += cx;
          }
        } else if (track <= 4) {
          /* 四路画面排布 */
          if ((i + 1) % 2) {
            x += cx;
          } else {
            x = 0;  /* 换行 */
            y += cy;
          }
        } else if (track <= 9) {
          /* 九路画面排布 */
          if ((i + 1) % 3) {
            x += cx;
          } else {
            x = 0;  /* 换行 */
            y += cy;
          }
        }
      } else {
        buffer->rcOut.left = 0;
        buffer->rcOut.right = buffer->width;
        buffer->rcOut.top = 0;
        buffer->rcOut.bottom = buffer->height;
      }
    }
  }
}

void 
RenderBuffer()
{
  RECT          rcText;
  HFONT hFontOld = (HFONT)SelectObject(main_app.hMemDC, main_app.hFont);
  SetTextColor(main_app.hMemDC, RGB(0, 220, 0));

  RECT rcClient = { 0, 0, main_app.cx, main_app.cy };
  HGDIOBJ bitmap_memory_old = SelectObject(main_app.hMemDC, main_app.hBitmapView);
  FillRect(main_app.hMemDC, &rcClient, (HBRUSH)GetStockObject(BLACK_BRUSH));
  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    TrackVideoBuffer* buffer = &main_app.trackVideos[i];
    TrackBufferDraw(buffer, true);
  }

  for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
    TrackVideoBuffer* buffer = &main_app.trackVideos[i];
    TrackBufferDraw(buffer, false);
  }

  if (main_app.tips_on) {
    SetRect(&rcText, 0, 0, 640, 480);
    DrawText(main_app.hMemDC, main_app.tips.c_str(), -1, &rcText, DT_NOCLIP);
  }

  // Show source frame in the center of the client area
  BitBlt(main_app.hDCView, 0, 0, main_app.cx, main_app.cy,
    main_app.hMemDC, 0, 0, SRCCOPY);
  SelectObject(main_app.hMemDC, bitmap_memory_old);
  SelectObject(main_app.hMemDC, hFontOld);
}

void xcast_start_stream(HWND hWnd)
{
  if (is_stream_running) {
    ui_start_stream("stream1", false, NULL);
    is_stream_running = false;
  }
  else {
    if (XCAST_OK == ui_start_stream("stream1", true, &main_app)) {
      is_stream_running = true;
    }
  }
  HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
  TBBUTTONINFO tbInfo;
  tbInfo.cbSize = sizeof(TBBUTTONINFO);
  tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
  tbInfo.iImage = is_stream_running ? MAKELONG(4, 0) : MAKELONG(3, 0);
  tbInfo.pszText = is_stream_running ? TEXT("退房") : TEXT("进房");
  SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_STARTSTREAM, (LPARAM)&tbInfo);

  if (!is_stream_running) {
    /* 刷新界面 */
    ClearTrackBuffer(NULL, false);
    InvalidVideoView();
  }
}

void xcast_init(HWND hWnd)
{
  if (is_initialized) {
    if (is_stream_running) {
      xcast_start_stream(hWnd);
    }

    ui_init_xcast(false, &main_app);
    is_initialized = false;

    HTREEITEM         hItem;
    char              path[XCAST_MAX_PATH] = {0};
    snprintf(path, XCAST_MAX_PATH, "stream.%s", "stream1");
    hItem = GetTreeItem(main_app.hTreeView, path, NULL);
    if (hItem) {
      RemoveTreeItem(main_app.hTreeView, hItem);
    }

  } else {
    if (XCAST_OK == ui_init_xcast(true, &main_app)) {
      is_initialized = true;
    }
  }

  HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
  TBBUTTONINFO tbInfo;
  tbInfo.cbSize = sizeof(TBBUTTONINFO);
  tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
  tbInfo.iImage = is_initialized ? MAKELONG(4, 0) : MAKELONG(3, 0);
  tbInfo.pszText = is_initialized ? TEXT("停止") : TEXT("启动");
  SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_INIT, (LPARAM)&tbInfo);
  
  if (!is_initialized) {
    HTREEITEM         hItem;
    char              path[XCAST_MAX_PATH] = {0};
    snprintf(path, XCAST_MAX_PATH, "stream.%s", "stream1");
    hItem = GetTreeItem(main_app.hTreeView, path, NULL);
    if (hItem) {
      RemoveTreeItem(main_app.hTreeView, hItem);
    }
  }
}


/* 主窗口消息处理过程 */
static LRESULT CALLBACK
MainViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_COMMAND: {
    int wmId = LOWORD(wParam);

    // 分析菜单选择:
    switch (wmId) {
    case ID_INIT:
	{
		//xcast_init(hWnd);
		//break;

		/*tencent::xcast_data settings;
		settings["app_id"] = 1400036169;
		settings["identifier"] = 12345654;
		settings["test_env"] = false;
		int32_t rt = tencent::xcast::startup(settings);*/

		main_observer.reset(new XCastObserver());

		if (!is_initialized)
		{

			XCastStartParam *param = new XCastStartParam;
			std::unique_ptr<XCastStartParam> up(param);
			up->identifier = 67890;
			up->isTestEvn = false;
			up->sdkappid = 1400036169;
			up->accounttype = 14180;

			XCastHelper::getInstance()->startContext(std::move(up), [&](int code, const char *err) {
				is_initialized = code == 0;
				new_ui_init_xcast(is_initialized, &main_app);

				HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
				TBBUTTONINFO tbInfo;
				tbInfo.cbSize = sizeof(TBBUTTONINFO);
				tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
				tbInfo.iImage = is_initialized ? MAKELONG(4, 0) : MAKELONG(3, 0);
				tbInfo.pszText = is_initialized ? TEXT("停止") : TEXT("启动");
				SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_INIT, (LPARAM)&tbInfo);

				if (!is_initialized) {
					HTREEITEM         hItem;
					char              path[XCAST_MAX_PATH] = { 0 };
					snprintf(path, XCAST_MAX_PATH, "stream.%s", "stream1");
					hItem = GetTreeItem(main_app.hTreeView, path, NULL);
					if (hItem) {
						RemoveTreeItem(main_app.hTreeView, hItem);
					}
				}

				if (is_initialized)
				{ 
					XCastHelper::getInstance()->setGlobalHandler(main_observer);
				}
			});
		}
		else
		{
			XCastHelper::getInstance()->stopContext([&](int32_t errcode, const char *err){
				is_stream_running = false;
				is_initialized = false;
				//new_ui_init_xcast(is_initialized, &main_app);

				HTREEITEM         hItem;
				char              path[XCAST_MAX_PATH] = { 0 };
				snprintf(path, XCAST_MAX_PATH, "stream.%s", "stream1");
				hItem = GetTreeItem(main_app.hTreeView, path, NULL);
				if (hItem) {
					RemoveTreeItem(main_app.hTreeView, hItem);
				}


				HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
				TBBUTTONINFO tbInfo;
				tbInfo.cbSize = sizeof(TBBUTTONINFO);
				tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
				tbInfo.iImage = is_initialized ? MAKELONG(4, 0) : MAKELONG(3, 0);
				tbInfo.pszText = is_initialized ? TEXT("停止") : TEXT("启动");
				SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_INIT, (LPARAM)&tbInfo);

				if (!is_initialized) {
					HTREEITEM         hItem;
					char              path[XCAST_MAX_PATH] = { 0 };
					snprintf(path, XCAST_MAX_PATH, "stream.%s", "stream1");
					hItem = GetTreeItem(main_app.hTreeView, path, NULL);
					if (hItem) {
						RemoveTreeItem(main_app.hTreeView, hItem);
					}
				}
			});
		}
	}
      
      break;
    case ID_STARTSTREAM:
	{
		/*xcast_start_stream(hWnd);
		break;*/

		if (!is_stream_running)
		{
			std::unique_ptr<XCastStreamParam> param(new XCastStreamParam);

			param->role = "user";
			param->roomid = 1000;
			param->auto_recv = true;
			param->streamID = "stream1";

			param->auth_info.auth_bits = -1;
			param->auth_info.auth_type = XCastAuth_None;

			XCastHelper::getInstance()->enterRoom(std::move(param), main_observer, [&](int code, const char *err) {
				is_stream_running = code == XCAST_OK;
				if (is_stream_running)
				{
					HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
					TBBUTTONINFO tbInfo;
					tbInfo.cbSize = sizeof(TBBUTTONINFO);
					tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
					tbInfo.iImage = is_stream_running ? MAKELONG(4, 0) : MAKELONG(3, 0);
					tbInfo.pszText = is_stream_running ? TEXT("退房") : TEXT("进房");
					SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_STARTSTREAM, (LPARAM)&tbInfo);

					if (!is_stream_running) {
						/* 刷新界面 */
						ClearTrackBuffer(NULL, false);
						InvalidVideoView();
					}
				}
				else
				{
					ui_xcast_err(code, err, NULL);
				}

			});
		}
		else
		{
			XCastHelper::getInstance()->exitRoom([&](int32_t code, const char *err) {
				is_stream_running = code == XCAST_OK ? false : true;
				if (!is_stream_running)
				{
					HWND hToolbar = FindWindowEx(hWnd, NULL, TOOLBARCLASSNAME, NULL);
					TBBUTTONINFO tbInfo;
					tbInfo.cbSize = sizeof(TBBUTTONINFO);
					tbInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;
					tbInfo.iImage = is_stream_running ? MAKELONG(4, 0) : MAKELONG(3, 0);
					tbInfo.pszText = is_stream_running ? TEXT("退房") : TEXT("进房");
					SendMessage(hToolbar, TB_SETBUTTONINFO, (WPARAM)ID_STARTSTREAM, (LPARAM)&tbInfo);

					if (!is_stream_running) {
						/* 刷新界面 */
						ClearTrackBuffer(NULL, false);
						InvalidVideoView();
					}
				}
				else
				{
					ui_xcast_err(code, err, NULL);
				}
			});
		}
		


	}
      break;
       break;
    case ID_STARTLANRELAY:
      if (is_lan_start) {
        ui_start_lan_relay(false, "", 0);
        is_lan_start = false;
        updateLanRelayButtonState();
      } else {
        DialogBox(main_app.hInstance, MAKEINTRESOURCE(IDD_LAN_RELAY), hWnd, LanRelaySetting);
      }
       break;
    case IDM_ABOUT:
      DialogBox(main_app.hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case ID_32859:
      DialogBox(main_app.hInstance, MAKEINTRESOURCE(IDD_ROLE), hWnd, Role);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    case IDM_SEL_CAPTURE:
    case ID_SDK_APP32856:
      if (is_initialized) {
        MessageBox(hWnd, L"停止后才能设置", L"", MB_OK);
      } else {
        DialogBox(main_app.hInstance, MAKEINTRESOURCE(IDD_APPID_SETTING), hWnd, AppidSetting);
      }
      break;
    case ID_TIPS_ON:
      main_app.tips_on = !main_app.tips_on;
      break;
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    break;
  }
  case WM_SIZE: {
    SendMessage(main_app.hToolbar, TB_AUTOSIZE, 0, 0);
    MoveWindow(main_app.hStatusbar, 0, 0, 0, 0, TRUE);
    AutoResizeViews();
    break;
  }
  case WM_NOTIFY:
    switch (LOWORD(wParam)) {
    case ID_TREEVIEW:
      return OnTreeViewMsg(main_app.hTreeView, msg, wParam, lParam);
    case IDC_TABVIEW:
      return OnTabviewMsg(main_app.hTabview, msg, wParam, lParam);
    default:
      break;
    }

    break;
  case WM_DESTROY:
    if (main_app.hBitmapView) {
      DeleteObject(main_app.hBitmapView);
      main_app.hBitmapView = NULL;
    }

    /* 清理缓冲区 */
    for (int i = 0; i < MAX_TRACK_VIDEO; i++) {
      TrackVideoBuffer *buffer = &main_app.trackVideos[i];
      buffer->track.clear();
      buffer->width = buffer->height = 0;
      buffer->data = NULL;
      if (buffer->hBitmapTarget) DeleteObject(buffer->hBitmapTarget);
      buffer->hBitmapTarget = NULL;
    }

    DeleteObject(main_app.hFont);
    DeleteDC(main_app.hMemDC);
    DeleteDC(main_app.hDCCompatible);
    ReleaseDC(main_app.hVideoView, main_app.hDCView);
    main_app.hMemDC = main_app.hDCCompatible = NULL;

    PostQuitMessage(0);
    break;

  default:
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

/* VideoView消息处理过程 */
static LRESULT CALLBACK
VideoViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
  static std::string drag;
  static POINT pt_last = { 0, 0 };
  static int32_t captured = -1;
  switch (msg) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RenderBuffer();
    EndPaint(hWnd, &ps);
    return TRUE;
  }
  case WM_ERASEBKGND:
    break;

  case WM_LBUTTONDOWN: {
    POINT pt;
    RECT rcClient;

    GetClientRect(main_app.hMainFrame, &rcClient);
    TabCtrl_AdjustRect(main_app.hTabview, FALSE, &rcClient);
    GetCursorPos(&pt);
    ScreenToClient(main_app.hVideoView, &pt);
    if (PtInRect(&rcClient, pt)) {
      /* 画面排布 */
      for (int i = MAX_TRACK_VIDEO - 1; i >= 0; i--) {
        TrackVideoBuffer* buffer = &main_app.trackVideos[i];
        if (!buffer->track.empty() && !buffer->bottom) {
          if (PtInRect(&buffer->rcOut, pt)) {
            drag = buffer->track;
            captured = i;
            pt_last = pt;
            SetCapture(hWnd);
            break;
          }
        }
      }

      if (captured > -1) return TRUE;

      /* 画面排布 */
      for (int i = MAX_TRACK_VIDEO - 1; i >= 0; i--) {
        TrackVideoBuffer* buffer = &main_app.trackVideos[i];
        if (!buffer->track.empty() && buffer->bottom) {
          if (PtInRect(&buffer->rcOut, pt)) {
            drag = buffer->track;
            captured = i;
            pt_last = pt;
            SetCapture(hWnd);
            break;
          }
        }
      }
    }
    return TRUE;
  }
  case WM_MOUSEMOVE:
    if (captured > -1) {
      POINT pt;
      RECT rcClient;

      GetClientRect(main_app.hMainFrame, &rcClient);
      TabCtrl_AdjustRect(main_app.hTabview, FALSE, &rcClient);

      if (captured < MAX_TRACK_VIDEO) {
        TrackVideoBuffer* buffer = &main_app.trackVideos[captured];
        if (buffer->track == drag) {
          buffer->moved = true;
          GetCursorPos(&pt);
          ScreenToClient(main_app.hVideoView, &pt);
          if (pt.x < rcClient.right) {
            buffer->rcOut.left += pt.x - pt_last.x;
            buffer->rcOut.right += pt.x - pt_last.x;
            pt_last.x = pt.x;
          }

          if (pt.y < rcClient.bottom) {
            buffer->rcOut.top += pt.y - pt_last.y;
            buffer->rcOut.bottom += pt.y - pt_last.y;
            pt_last.y = pt.y;
          }
          CalculateUpdateRect();
          InvalidVideoView();
          return TRUE;
        }
      }
    }
    break;
  case WM_LBUTTONUP:
    if (captured > -1) {
      captured = -1;
      ReleaseCapture();
    }
    return TRUE;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

int32_t read_file(const char* pFile, char** content)
{

  int32_t retval = -1;
  FILE* fp = NULL;
  char *pBuf = NULL;
  size_t length = 0;
  do {

#ifdef _WIN32
    fopen_s(&fp, pFile, "rb");
#else
    fp = fopen(pFile, "rb");
#endif
    if (!fp) {
      retval = -1;
      break;
    }

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (length <= 0) return -1;
    pBuf = new char[length + 1];
    pBuf[length] = '\0';
    if (length != fread(pBuf, 1, length, fp)) {
      retval = -2;
      break;
    }
    retval = 0;
  } while (0);

  if (fp) {
    fclose(fp);
  }

  if (retval == 0) {
    *content = pBuf;
    return length;
  }
  else {
    *content = NULL;
    delete pBuf;
  }
  return retval;
}

int32_t write_file(const char* pFile, const char* mode, const char* content, uint32_t content_size)
{
  int32_t retval = -1;
  FILE* fp = NULL;

#ifdef _WIN32
  fopen_s(&fp, pFile, "wb");
#else
  fp = fopen(pFile, "rb");
#endif
  if (!fp) {
    return -1;
  }

  if (content_size != fwrite(content, 1, content_size, fp)) {
    retval = -2;
  }
  else {
    retval = 0;
  }

  fclose(fp);

  return retval;
}

void save_account()
{
  json_t *json = json_object();
   
  json_t *base_info = json_object();
  json_t *value = json_integer(appid);
  json_object_set(base_info, "appid", value);
  json_decref(value);

  value = json_integer(account);
  json_object_set(base_info, "account", value);
  json_decref(value);

  value = json_integer(relation_id);
  json_object_set(base_info, "relation_id", value);
  json_decref(value);

  value = json_boolean(test_env);
  json_object_set(base_info, "test_env", value);
  json_decref(value);
   
  json_object_set(json, "base", base_info);
  json_decref(base_info);

  json_t  *lan_relay = json_object();
  value = json_string(peer_ip.c_str());
  json_object_set(lan_relay, "peer_ip", value);
  json_decref(value);

  value = json_integer(peer_port);
  json_object_set(lan_relay, "peer_port", value);
  json_decref(value);

  json_object_set(json, "lan", lan_relay);
  json_decref(lan_relay);

  json_dump_file(json, "settings.txt",0);
  json_decref(json);

}

int32_t read_account()
{
  json_error_t err;
  json_t *json = json_load_file("settings.txt", 0, &err);
  if (!json) {
    appid = 1400036169;
    srand((unsigned int)time(NULL));
    account = 0xFFFFFFFF - rand();
    relation_id = 200019;
    test_env = false;
    save_account();
  } else {
    json_t *base = json_object_get(json, "base");
    if (base) {
      json_t *value = json_object_get(base, "appid");
      appid = (int32_t)json_integer_value(value);
      if (!appid) appid = 1400036169;
      account = json_integer_value(json_object_get(base, "account"));
      if (!account) {
        srand((unsigned int)time(NULL));
        account = 0xFFFFFFFF - rand();
      }
      relation_id = (int32_t)json_integer_value(json_object_get(base, "relation_id"));
      if (!relation_id) relation_id = 200019;
      test_env = json_boolean_value(json_object_get(base, "test_env"));
    } else {
      appid = 1400036169;
      srand((unsigned int)time(NULL));
      account = 0xFFFFFFFF - rand();
      relation_id = 200019;
      test_env = false;
    }
    json_t *lan = json_object_get(json, "lan");
    if (lan) {
      json_t *value = json_object_get(lan, "peer_ip");
      if (value) {
        peer_ip = json_string_value(value);
      }
      value = json_object_get(lan, "peer_port");
      if (value) {
        peer_port = (uint16_t)json_integer_value(value);
      }
    }
  }
  json_decref(json);
  return 0;
}

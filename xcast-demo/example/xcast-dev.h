#pragma once

#include "stdafx.h"
#include "resource.h"
#include "xcast.hh"
#include <string>
#include <vector>

using namespace tencent;

#define       MAX_TRACK_VIDEO       9

typedef struct {
  uint8_t       format;
  uint8_t       blend_mode;
  double        left;
  double        top;
  uint32_t      width;
  uint32_t      height;
  RECT          rcOut;
  RECT          rcLastOut;
  uint8_t      *data;
  HBITMAP       hBitmapCache;
} TrackVideoLayer;

/* 轨道视频缓冲 */
typedef struct {
  std::string                   track;
  std::wstring                  text;
  uint64_t                      uin;
  int                           width, height;
  void                         *data;
  HBITMAP                       hBitmapTarget;
  RECT                          rcOut;
  bool                          bottom, moved;
  std::vector<TrackVideoLayer>  layers;
} TrackVideoBuffer;

typedef struct {
  uint64_t                uin;
  HINSTANCE               hInstance;
  HWND                    hMainFrame;           /* 主窗口 */
  HWND                    hToolbar;             /* 工具栏 */
  HWND                    hTabview;             /* Tab视图 */
  HWND                    hTreeView;            /* 左侧树视图 */
  HWND                    hVideoView;           /* 视频显示视图 */
  HWND                    hStatusbar;           /* 状态栏 */
  HIMAGELIST              hImageList;

  HFONT                   hFont;
  HDC                     hDCView;
  HDC                     hMemDC;
  HDC                     hDCCompatible;
  HBITMAP                 hBitmapView;
  int                     cx, cy;

  bool                    tips_on;
  std::wstring            tips;

  TrackVideoBuffer        trackVideos[MAX_TRACK_VIDEO];
} XCastApp;

#define ICON_FOLDER                   1
#define ICON_FOLDER_EXPAND            2

extern XCastApp           main_app;

#pragma once

#include "stdafx.h"
#include "xcast-dev.h"
#include "xcast-ui-handler.h"

extern  int32_t    appid;
extern  uint64_t   account;
extern  int32_t    relation_id;

void utf16_to_utf8(const char16_t *wstr, uint32_t len, std::string& str);
void utf8_to_utf16(const char *str, uint32_t len, std::wstring& wstr);

typedef struct tree_item_data_s tree_item_data_t;
typedef int32_t (*tree_item_pt)(tree_item_data_t *, HTREEITEM);   /* 树控件事件 */

typedef struct capture_data_s {
  int32_t       track_type;
  int32_t       track_index;
  std::string   stream;
  std::string   track;
  std::string   current_capture;
} capture_data_t;

struct tree_item_data_s {
  int32_t                   icon;       /* icon of item */
  std::string               text;       /* display text of item */
  std::string               path;       /* parent path of item in tree view */
  std::string               id;
  std::string               id2;
  const char               *prop;       /* property path of item in xcast */
  bool                      start;
  tree_item_pt              db_click;   /* double click event handle */
  tree_item_pt              right_click;   /* double click event handle */
  capture_data_t           *cap_data;

  bool                      is_stream;
};

tree_item_data_t *CreateItemData(int32_t icon, const char *path, const char *text);

/* 从数组获取轨道视频帧缓冲，如果没有则创建之 */
TrackVideoBuffer *GetTrackBuffer(const char *track, int width, int height, bool bNoResize = false);
void ClearTrackBuffer(const char *track);

/* XCAST属性树视图中插入新节点 */
HTREEITEM AddTreeItem(HWND hTree, 
  LPCSTR pszFullPath, LPCSTR pszName, tree_item_data_t *item_data);

/* 从XCAST属性树视图中删除指定节点 */
void RemoveTreeItem(HWND hTree, HTREEITEM hItem);

tree_item_data_t *GetTreeItemData(HWND hTree, HTREEITEM hItem);

/* 按照路径查找XCAST属性树控件中的节点和父节点，如果找不到，则返回NULL */
HTREEITEM GetTreeItem(HWND hTree, LPCSTR pszPath, HTREEITEM *pParent);

/* 设置XCAST属性树视图中节点图标 */
void SetTreeItemIcon(HWND hTree, HTREEITEM hItem, int32_t icon);

//HTREEITEM AddOutputTrack(HWND hTree, 
//  bool add, const char *parent, const char *track_name, const char *id);
//HTREEITEM AddInputTrack(HWND hTree, 
//  bool add, const char *parent, uint64_t uin, const char *track_name, const char *id);
//HTREEITEM AddCameraDevice(HWND hTree, bool add, const char *camera);
//HTREEITEM AddMicDevice(HWND hTree, bool add, const char *mic);
//HTREEITEM AddSpeaerDevice(HWND hTree, bool add, const char *speaker);
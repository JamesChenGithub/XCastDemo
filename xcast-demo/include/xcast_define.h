/*
** Copyright (c) 2017 The xcast project. All rights reserved.
*/
#ifndef XCAST_DEFINE_H
#define XCAST_DEFINE_H

typedef enum xc_auth_type_s {
  xc_auth_none = 0,
  xc_auth_manual,
  xc_auth_auto,
} xc_auth_type_t;

/* xcast system events */
typedef enum xc_system_event_s {
  xc_system_normal = 1,
  xc_system_warning,
  xc_system_error,
  xc_system_fatal,
} xc_system_event_t;

/* xcast stream events */
typedef enum xc_stream_event_e {
  xc_stream_added = 1,                /* new stream added */
  xc_stream_updated,                  /* stream state updated */
  xc_stream_removed,                  /* stream removed */
} xc_stream_event;

/* xcast stream events */
typedef enum xc_stream_state_e {
  xc_stream_connecting = 1,           /* stream connecting */
  xc_stream_connected,                /* stream connected */
  xc_stream_closed,                   /* stream connected */
} xc_stream_state;

/* xcast stream track events */
typedef enum xc_track_event_e {
  xc_track_added = 1,                 /* new track added */
  xc_track_updated,                   /* track state updated */
  xc_track_removed,                   /* track removed */
  xc_track_capture_changed,           /* track capture changed */
  xc_track_media,                     /* track media data coming */
} xc_track_event;

/* xcast stream track running state */
typedef enum xc_track_state_e {
  xc_track_stopped = 1,               /* track stopped */
  xc_track_running,                   /* track is running */
} xc_track_state;

/* xcast track types */
typedef enum xc_track_type_e {
  xc_track_msg = 1,                   /* msg track */
  xc_track_audio,                     /* audio track */
  xc_track_video,                     /* video track */
} xc_track_type;

/* xcast track types */
typedef enum xc_track_direction_e {
  xc_track_out = 1,                   /* send media data to remote host */
  xc_track_in,                        /* receive media data from remote host */
} xc_track_direction;

/* xcast media types */
typedef enum xc_media_format_e {
  xc_media_argb32 = 0,                /* argb32 video format */
  xc_media_i420,                      /* i420 video format */
  xc_media_aac,                       /* aac audio format */
  xc_media_pcm,                       /* pcm audio format */
  xc_media_layer = 0xFF,              /* layer format*/
} xc_media_format;

/* xcast media source types */
typedef enum xc_media_source_e {
  xc_media_source_unknown = 0,        /* unknown source type */
  xc_media_source_camera,             /* camera */
  xc_media_source_screen_capture,     /* screen capture */
  xc_media_source_media_player,       /* media player */
  xc_media_source_ppt,                /* ppt */
} xc_media_source;

/* xcast device running state */
typedef enum xc_device_type_e {
  xc_device_unknown = 0,              /* unknown device type */
  xc_device_camera,                   /* camera device */
  xc_device_screen_capture,           /* screnn capture */
  xc_device_player,                   /* media player */
  xc_device_mic,                      /* microphone device */
  xc_device_speaker,                  /* speaker device */
  xc_device_accompany,                /* audio acompany device */
  xc_device_external,                 /* external device type */
} xc_device_type;

/* xcast stream track events */
typedef enum xc_device_event_e {
  xc_device_added = 1,                /* new device added */
  xc_device_updated,                  /* device state updated */
  xc_device_removed,                  /* device removed */
  xc_device_preprocess,               /* device preprocess data coming */
  xc_device_preview,                  /* device preview data coming */
} xc_device_event;

/* xcast device running state */
typedef enum xc_device_state_e {
  xc_device_stopped = 1,
  xc_device_running,
} xc_device_state;

/* xcast事件列表 */

/*
* xcast events notify path defines.
* register callbacks through 'xcast_handle_event()' can receive
* notifies from xcast, the notified event value is a 'xcast_variant_t' of 'vdict' type.
* // xcast支持的事件描述, ver 000.000.1
* // 注意：xcast的事件回调使用xcast_variant_t(vdict)类型的参数值。
* // 事件描述遵循以下格式(带"*"项表示必需项)：
* //   "事件路径":{
* //     // 事件源
* //     "*src":"vstring",
* //     // 事件类型
* //     "*type":"vstring",
* //     // 事件状态
* //     "state":"vstring",
* //     // 错误代码
* //     "err":"vint32",
* //     // 错误信息
* //     "err-msg":"vstring",
* //     // 事件自定义数据
* //     ...
* //   }
* //
* // 其中，"[]"表示数值可选，"{}"表示数值为字典vdict类型。
* //
*/

/*
* xcast支持的系统事件
* "event.system":{
*   // 事件源:
*   "*src":["system"],
*   // 事件类型: 警告，错误，致命
*   "*type":[xc_system_event_t],
*   // 事件状态
*   "state":["running","closed"],
*   // 错误代码
*   "err":"vint32",
*   // 错误信息
*   "err-msg":"vstring"
* },
*/
#define XC_EVENT_SYSTEM                     "event.system"  

/*
* xcast支持的媒体流事件
* "event.stream":{
*   // 事件源:媒体流名称
*   "*src":"vstring",
*   // 事件类型: 新增，更新，删除
*   "*type":[xc_stream_event],
*   // 事件状态
*   "state":[xc_stream_state],
*   // 错误代码
*   "err":"vint32",
*   // 错误信息
*   "err-msg":"vstring"
* },
*/
#define XC_EVENT_STREAM                     "event.stream"  

/*
* xcast支持的媒体流轨道事件
* "event.track":{
*   // 事件源: 轨道名 媒体流名称
*   "*src":"vstring",
*   "*stream":"vstring",
*   // 事件类型: 新增，更新，删除,媒体数据
*   "*type":[xc_track_event],
*   // 轨道类别: xc_track_audio,xc_track_video
*   "*class":[xc_track_type],
*   // 轨道方向: 上行,下行
*   "*direction":[xc_track_direction],
*   // 用户uin
*   "*uin":"vuint64",
*   // 轨道编号
*   "*index":"vuint32",
*   // 轨道状态
*   "state":[xc_track_state],
*   // 错误代码
*   "err":"vint32",
*   // 错误信息
*   "err-msg":"vstring",
*   // 轨道数据格式
*   "format":[xc_media_format],
*   // 轨道数据缓存
*   "data":"vbytes",
*   // 轨道数据缓存长度
*   "size":"vuint32",
*   // 轨道数据宽度
*   "width":"vuint32",
*   // 轨道数据高度
*   "height":"vuint32",
*   // 轨道数据旋转
*   "rotate":[0,90,180,270]
*   // 轨道数据视频源:摄像头，屏幕分享，媒体文件，PPT，未知源
*   "media-src":[xc_media_source]
* },
*/
#define XC_EVENT_TRACK                      "event.track"

/*
* xcast支持的设备事件
* "event.device":{
*   // 事件源: 设备名
*   "*src":"vstring",
*   // 事件类型: 新增，更新，删除,预处理数据,预览数据
*   "*type":[xc_device_added,xc_device_updated,
      xc_device_removed,xc_device_preprocess,xc_device_preview],
*   // 设备类型: xc_device_type
*   "*class":[xc_device_camera,xc_device_screen_capture,xc_device_player
*     xc_device_mic,xc_device_speaker,xc_device_external],
*   // 事件状态: 运行，停止
*   "state":[xc_device_running,xc_device_stopped],
*   // 错误代码
*   "err":"vint32",
*   // 错误信息
*   "err-msg":"vstring",
*   // 数据格式
*   "format":[xc_media_argb32,xc_media_i420,xc_media_aac],
*   // 数据缓存
*   "data":"vbytes",
*   // 数据缓存长度
*   "size":"vuint32",
*   // 数据宽度
*   "width":"vint32",
*   // 数据高度
*   "height":"vint32",
*   // 数据旋转
*   "rotate":[0,90,180,270],
*   // 音频设备：音量值，取值范围[0,100]
*   "volume":"vuint32",
*   // 音频设备：动态音量值，取值范围[0,100]
*   "dynamic-volume":"vuint32",
*   // 播放器路径
*   "player-path":"vstring",
*   // 音频文件路径
*   "file-path":"vstring",
*   // 播放进度
*   "current-pos":"vint64",
*   // 最大进度
*   "max-pos":"vint64",
*   // 伴奏源: 默认值,系统伴奏,应用伴奏(播放器进程)
*   "accompany-source":["none","system","process"],
*   // 左边界
*   "screen-left":"vint32",
*   // 上边界
*   "screen-top":"vint32",
*   // 右边界
*   "screen-right":"vint32",
*   // 下边界
*   "screen-bottom":"vint32",
*   // 捕获帧率
*   "screen-fps":"vuint32",
*   // 窗口句柄, 仅支持win
*   "screen-hwnd":"vint32"
* },
*/
#define XC_EVENT_DEVICE                     "event.device"

/*
* // <TODO>音频参数：获取通话中实时音频/视频质量相关信息
* "event.tips":{
*   // 事件源: "audio","video"
*   "*src":["audio","video"],
*   "*tips":"vstring"
* }
*/
#define XC_EVENT_STATISTIC_TIPS             "event.tips"

/*
* // xcast支持的属性描述, ver 000.000.1
* // 注意：xcast的属性操作只接收/返回xcast_variant_t类型的参数/返回值。
* // 属性描述遵循以下格式：
* //   "属性路径":{
* //     "属性get":{
* //       // 返回值：带"*"项表示必需项，"null"表示不需要
* //       "return":[null, {其它值}]
* //     },
* //     "属性set":{
* //       // 参数：带"*"项表示必需项，"null"表示不需要
* //       "params":[null, {其它值}],
* //     }                                                                                                                                               
* // 其中，"[]"表示数值可选，"{}"表示数值为字典vdict类型。
* //
*/

/* xcast支持的媒体流属性 */

/*
* // 媒体流属性: 获取xcast中媒体流列表
* "stream":{
*   "get":{
*     "return":[
*       // "null"表示不存在
*       null,
*       // 所有媒体流名的字符串数组
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_STREAM                           "stream"

/*
* // 媒体流属性: 获取指定媒体流状态
* "stream.%s.state":{
*   "get":{
*     "return":[
*       // "null"表示不存在
*       null,
*       // 媒体流状态：
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_STREAM_STATE                     "stream.%s.state"

/*
* // 媒体流属性: 获取指定媒体流包含的轨道列表
* "stream.%s.track":{
*   "get":{
*     "return":[
*       // "null"表示不存在
*       null,
*       // 所有轨道名的字符串数组
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_STREAM_TRACK                     "stream.%s.track"

/*
* // 媒体流属性: 启动/停止媒体流轨道
* "stream.%s.%s.enable":{
*   "set":{
*     "params":{
*       // true启动,false停止
*       "*enable":"vbool",
*       // 请求video下行的时候可以指定请求画面大小
*       "size":["small","big"]
*     }
*   }
* },
*/
#define XC_TRACK_ENABLE                     "stream.%s.%s.enable"

/*
* // 媒体流属性: 查询媒体流轨道状态
* "stream.%s.%s.state":{
*   "get":{
*     // xc_track_stopped停止,xc_track_running运行
*     "return":[xc_track_stopped,xc_track_running]
*   }
* },
*/
#define XC_TRACK_STATE                      "stream.%s.%s.state"

/*
* // 媒体流属性: 设置媒体流轨道上行音频/视频源
* "stream.%s.%s.capture":{
*   "get":{
*     "return":[
*       // 无视频源
*       null,
*       // 音频/视频源名(摄像头或麦克风)
*       "vstring"
*     ]
*   },
*   "set":{
*     // 视频源名(摄像头名)
*     "params":"vstring"
*   }
* },
*/
#define XC_TRACK_CAPTURE                    "stream.%s.%s.capture"

/* xcast支持的摄像头属性 */

/*
* // 摄像头属性: 获取系统中所有摄像头的列表
* "device.camera":{
*   "get":{
*     // 返回所有摄像头名的字符串数组
*     "return":["vstring"]
*   }
* },
*/
#define XC_CAMERA                           "device.camera"

/*
* // 摄像头属性: 获取/设置默认摄像头
* "device.camera.default":{
*   "get":{
*     "return":[
*       // 无摄像头
*       null,
*       // 默认摄像头名
*       "vstring"
*     ]
*   },
*   "set":{
*     // 摄像头名
*     "params":"vstring"
*   }
* },
*/
#define XC_CAMERA_DEFAULT                   "device.camera.default"

/*
* // 摄像头属性: 启动/关闭指定摄像头的预览
* "device.camera.%s.preview":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_CAMERA_PREVIEW                   "device.camera.%s.preview"

/*
* // 摄像头属性: 获取指定摄像头状态
* "device.camera.%s.state":{
*   "get":{
*     "return":[
*       // 无摄像头
*       null,
*       // 摄像头状态：xc_device_stopped，xc_device_running
*       [xc_device_stopped，xc_device_running]
*     ]
*   }
* },
*/
#define XC_CAMERA_STATE                     "device.camera.%s.state"

/*
* // <TODO>摄像头属性: 打开/关闭摄像头预处理
* "device.camera.%s.preprocess":{
*   "get":{
*     "return":[
*       // 无摄像头
*       null,
*       // 摄像头预处理状态：true打开，false关闭
*       "vbool"
*     ]
*   },
*   "set":{
*     // 摄像头名
*     "params":"vbool"
*   }
* },
*/
#define XC_CAMERA_PREPROCESS                "device.camera.%s.preprocess"

/* xcast支持的麦克风属性 */

/*
* // 麦克风属性: 获取系统中所有麦克风的列表
* "device.mic":{
*   "get":{
*     "return":[
*       // "null"表示不存在
*       null,
*       // 所有麦克风名的字符串数组
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_MIC                              "device.mic"

/*
* // 麦克风属性: 获取/设置默认麦克风
* "device.mic.default":{
*   "get":{
*     "return":[
*       // 无麦克风
*       null,
*       // 默认麦克风名
*       "vstring"
*     ]
*   },
*   "set":{
*     // 麦克风名
*     "params":"vstring"
*   }
* },
*/
#define XC_MIC_DEFAULT                      "device.mic.default"

/*
* // 麦克风属性: 获取指定麦克风状态
* "device.mic.%s.state":{
*   "get":{
*     "return":[
*       // 无麦克风
*       null,
*       // 麦克风状态：xc_device_stopped，xc_device_running
*       [xc_device_stopped，xc_device_running]
*     ]
*   }
* },
*/
#define XC_MIC_STATE                        "device.mic.%s.state"

/*
* // 麦克风属性: 启动/关闭指定麦克风本地回放
* "device.mic.%s.loopback":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_MIC_LOOPBACK                     "device.mic.%s.loopback"

/*
* // 麦克风属性: 音量获取和设置
* "device.mic.%s.volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   },
*   "set":{
*     "params":"vuint32"
*   }
* },
*/
#define XC_MIC_VOLUME                       "device.mic.%s.volume"

/* xcast支持的扬声器属性 */

/*
* // 扬声器属性: 获取系统中所有扬声器的列表
* "device.speaker":{
*   "get":{
*     "return": [
*       // "null"表示不存在
*       null,
*       // 所有扬声器名的字符串数组
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_SPEAKER                          "device.speaker"

/*
* // 扬声器属性: 获取/设置默认扬声器
* "device.speaker.default":{
*   "get":{
*     "return":[
*       // 无扬声器
*       null,
*       // 默认扬声器名
*       "vstring"
*     ]
*   },
*   "set":{
*     // 扬声器名
*     "params":"vstring",
*     "return":null
*   }
* },
*/
#define XC_SPEAKER_DEFAULT                  "device.speaker.default"

/*
* // 扬声器属性: 打开/关闭扬声器
* "device.speaker.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   }
* },
*/
#define XC_SPEAKER_ENABLE                   "device.speaker.%s.enable"

/*
 * // 扬声器属性: 打开/关闭耳机模式
 * "device.speaker.earphone-mode":{
 *   "get":{
 *     "return":"vbool"
 *   },
 *   "set":{
 *     "params":"vbool"
 *   }
 * },
 */
#define XC_SPEAKER_EARPHONE_MODE            "device.speaker.%s.earphone-mode"

/*
* // 扬声器属性: 音量获取和设置
* "device.speaker.volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   },
*   "set":{
*     "params":"vuint32"
*   }
* },
*/
#define XC_SPEAKER_VOLUME                   "device.speaker.%s.volume"

/* xcast支持的外部设备属性 */

/*
* // 外部设备属性: 获取系统中所有外部设备的列表
* "device.external":{
*   "get":{
*     "return": [
*       // "null"表示不存在
*       null,
*       // 所有外部设备名的字符串数组
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_DEVICE_EXTERNAL                  "device.external"

/*
* // 外部设备属性: 外部设备输入
* "device.external.%s.input":{
*   "set":{
*     "params":{
*       // 媒体格式
*       "*format":[xc_media_format],
*       // 媒体数据缓存
*       "*data":"vbytes",
*       // 媒体数据缓存长度
*       "*size":"vuint32",
*       // 媒体数据宽度
*       "*width":"vint32",
*       // 媒体数据高度
*       "height":"vint32",
*       // 媒体数据旋转
*       "rotate":[0,90,180,270],
*       // 媒体数据采样率
*       "sample-rate":"vuint32",
*       // 媒体数据通道：[1, 2]
*       "channel":[1, 2],
*       // 媒体数据比特位：[8, 16]
*       "bits":[8, 16]
*     }
*   }
* },
*/
#define XC_DEVICE_EXTERNAL_INPUT            "device.external.%s.input"

/*
 * 外部设备视频源类型
 * "device.external.%s.type":{
 *   "set":{
 *     "params":[
 *       // 外部设备状态：xc_media_source_screen_capture etc.
 *       [xc_media_source]
 *     ]
 *   }
 * },
 */
#define XC_DEVICE_EXTERNAL_TYPE            "device.external.%s.type"
/*
* // 外部设备属性: 获取指定外部设备状态
* "device.external.%s.state":{
*   "get":{
*     "return":[
*       // 无外部设备
*       null,
*       // 外部设备状态：xc_device_stopped，xc_device_running
*       [xc_device_state]
*     ]
*   }
* },
*/
#define XC_DEVICE_EXTERNAL_STATE            "device.external.%s.state"

/* xcast支持的视频播放器属性 */

/*
* // <TODO>视频播放器属性: 启动/关闭视频播放，请配合"device.media-file.setting"使用。
* // 支持格式：*.aac,*.ac3,*.amr,*.ape,*.mp3,*.flac,*.midi,*.wav,*.wma,*.ogg,
* //   *.amv,*.mkv,*.mod,*.mts,*.ogm,*.f4v,*.flv,*.hlv,*.asf,*.avi,*.wm,*.wmp,
* //   *.wmv,*.ram,*.rm,*.rmvb,*.rpm,*.rt,*.smi,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,
* //   *.m2v,*.mp2v, *.tp,*.tpr,*.ts,*.m4b,*.m4p,*.m4v,*.mp4,*.mpeg4,*.3g2,*.3gp,
* //   *.3gp2,*.3gpp,*.mov,*.pva,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,*.mp2v,*.pss,
* //   *.pva,*.ifo,*.vob,*.divx,*.evo,*.ivm,*.mkv,*.mod,*.mts,*.ogm,*.scm,*.tod,*.vp6,
* //   *.webm,*.xlmv。
* "device.media-file.enable":{
*   "get":{
*     "params":null,
*     "return":[
*       // 无摄像头
*       null,
*       // 视频播放状态：true启动，false关闭
*       "vbool"
*     ]
*   },
*   "set":{
*     // true启动，false关闭
*     "*params":"vbool",
*     // 通过注册事件获取视频播放捕获状态通知
*     "return":null
*   }
* },
*/
#define XC_MEDIA_FILE_ENABLE                "device.media-file.enable"

/*
* // <TODO>视频播放属性: 设置/获取视频播放的参数
* "device.media-file.setting":{
*   "get":{
*     "return":{
*       "file":"vstring",                     // 视频文件路径
*       "state":["init","playing","paused"],  // 播放状态：初始化，播放中，暂停
*       "pos":"vint64",                       // 播放进度
*       "max-pos":"vint64"                    // 最大进度
*     }
*   },
*   "set":{
*     "params":{
*       "file":"vstring",                     // 视频文件路径
*       "restart":"vbool",                    // 重新开始播放
*       "state":["init","playing","paused"],  // 播放状态：初始化，播放中，暂停
*       "pos":"vint64",                       // 播放进度
*       "max-pos":"vint64"                    // 最大进度
*     }
*   }
* },
*/
#define XC_MEDIA_FILE_SETTING               "device.media-file.setting"

/*
* // <TODO>视频播放属性:检查视频文件格式合法性
* "device.media-file.verify":{
*   "get":{
*     "params":{
*       "file":"vstring",                     // 视频文件路径
*       "loop-back":"vbool"                   // 循环播放
*     },
*     "return":{
*       "valid":"vbool",                      // 检查结果，true有效，false无效
*       "audio-only":"vbool"                  // 是否纯音频
*     }
*   }
* },
*/
#define XC_MEDIA_FILE_VERIFY                "device.media-file.verify"

/*
* // 屏幕捕获属性: 启动/关闭指定屏幕捕获预览
* "device.screen-capture.preview":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_SCREEN_CAPTURE_PREVIEW           "device.screen-capture.preview"

/*
* // 屏幕捕获属性: 设置/获取屏幕捕获的参数
* "device.screen-capture.setting":{
*   "get":{
*     "return":{
*       "left":"vint32",
*       "top":"vint32",
*       "right":"vint32",
*       "bottom":"vint32",
*       "fps":"vuint32",    // 捕获帧率: [1-20]
*       "hwnd":"vint32"     // 窗口句柄, 仅支持win
*     }
*   },
*   "set":{
*     "params":{
*       "left":"vint32",
*       "top":"vint32",
*       "right":"vint32",
*       "bottom":"vint32",
*       "fps":"vuint32",    // 捕获帧率: [1-20]
*       "hwnd":"vint32"     // 窗口句柄, 仅支持win
*     }
*   }
* },
*/
#define XC_SCREEN_CAPTURE_SETTING           "device.screen-capture.setting"

/*
* // <TODO>麦克风属性: 动态音量
* "device.mic.%s.dynamic-volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_MIC_DYNAMIC_VOLUME               "device.mic.%s.dynamic-volume"

/*
* // 麦克风属性: 外部音频输入
* "device.mic.external.input":{
*   "set":{
*     "params":{
*       // 音频格式
*       "*format":["aac"],
*       // 音频数据缓存
*       "*data":"vbytes",
*       // 音频数据缓存长度
*       "*size":"vuint32",
*       // 数据宽度
*       "*width":"vint32"
*     }
*   }
* },
*/
#define XC_MIC_EXTERNAL_INPUT               "device.mic.external.input"

/*
* // <TODO>麦克风属性: 输入额外的音频数据，与本地发送的音频数据混音后发送出去
* "device.mic.%s.mix":{
*   "set":{
*     "params":{
*       // 音频格式
*       "*format":["aac"],
*       // 音频数据缓存
*       "*data":"vbytes",
*       // 音频数据缓存长度
*       "*size":"vuint32",
*       // 数据宽度
*       "*width":"vint32"
*     }
*   }
* },
*/
#define XC_MIC_MIX                          "device.mic.%s.mix"

/*
* // <TODO>麦克风属性: 发送混音输入，实时性更高
* "device.mic.%s.mix-sync":{
*   "set":{
*     "params":{
*       // 音频格式
*       "*format":["aac"],
*       // 音频数据缓存
*       "*data":"vbytes",
*       // 音频数据缓存长度
*       "*size":"vuint32",
*       // 数据宽度
*       "*width":"vint32"
*     },
*     "return":null
*   }
* },
*/
#define XC_MIC_MIX_SYNC                     "device.mic.%s.mix-sync"

/*
* // <TODO>麦克风属性: 麦克风音频数据预处理,请结合麦克风事件使用
* "device.mic.%s.dispose":{
*   "set":{
*     "params":{
*       // true打开,false关闭
*       "*enable":"vbool",
*       // 麦克风名
*       "name":[
*         // 默认麦克风
*         null,
*         // 指定麦克风
*         "vstring"
*       ]
*     }
*   }
* },
*/
#define XC_MIC_DISPOSE                      "device.mic.dispose"

/*
* // <TODO>扬声器属性: 动态音量
* "device.speaker.dynamic-volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_SPEAKER_DYNAMIC_VOLUME           "device.speaker.dynamic-volume"

/*
* // <TODO>扬声器属性: 输入额外的音频数据，与本地播放的音频数据混音后给扬声器播放出来
* "device.speaker.mix":{
*   "set":{
*     "params":{
*       // 音频格式
*       "*format":["aac"],
*       // 音频数据缓存
*       "*data":"vbytes",
*       // 音频数据缓存长度
*       "*size":"vuint32",
*       // 数据宽度
*       "*width":"vint32"
*     }
*   }
* },
*/
#define XC_SPEAKER_MIX                      "device.speaker.mix"

/* xcast支持的伴奏器属性 */

/*
* // <TODO>伴奏属性: 启动/关闭伴奏
* "device.accompany.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     // 扬声器名
*     "params":"vbool"
*   }
* },
*/
#define XC_ACCOMPANY_ENABLE                 "device.accompany.enable"

/*
* // <TODO>伴奏属性: 音量
* "device.accompany.volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   },
*   "set":{
*     // 音量值，取值范围[0,100]
*     "params":"vuint32"
*   }
* },
*/
#define XC_ACCOMPANY_VOLUME                 "device.accompany.volume"

/*
* // <TODO>伴奏属性: 动态音量
* "device.accompany.dynamic-volume":{
*   "get":{
*     // 音量值，取值范围[0,100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_ACCOMPANY_DYNAMIC_VOLUME         "device.accompany.dynamic-volume"

/*
* // <TODO>伴奏属性: 设置/获取伴奏配置
* "device.accompany.setting":{
*   "get":{
*     "return":{
*       // 播放器路径
*       "player":"vstring",
*       // 音频文件路径
*       "file":"vstring",
*       // 伴奏源: 默认值,系统伴奏,应用伴奏(播放器进程)
*       "source":["none","system","process"]
*     }
*   },
*   "set":{
*     "params":{
*       // 播放器路径
*       "*player":"vstring",
*       // 音频文件路径
*       "*file":"vstring",
*       // 伴奏源: 默认值,系统伴奏,应用伴奏(播放器进程)
*       "*source":["none","system","process"]
*     }
*   }
* },
*/
#define XC_ACCOMPANY_SETTING                "device.accompany.setting"

/* xcast支持的系统参数属性 */

/*
* // <TODO>系统参数：业务类型
* "preference.business":{
*   "get":{
*     // 返回一种类型，默认为"opensdk"
*     "return":["unknown","discuss","group","edu","qcall","opensdk","edu_k12"]
*   },
*   "set":{
*     // 设置一种类型，默认为"opensdk"
*     "*params":["unknown","discuss","group","edu","qcall","opensdk","edu_k12"]
*   }
* },
*/
#define XC_PREFERENCE_BUSINESS              "preference.business"

/*
* // <TODO>系统参数：关系链类型
* "preference.relation":{
*   "get":{
*     // 返回一种类型，默认为"opensdk"
*     "return":["unknown","group","discuss","buddy","temp","opensdk"]
*   },
*   "set":{
*     // 设置一种类型，默认为"opensdk"
*     "*params":["unknown","group","discuss","buddy","temp","opensdk"]
*   }
* },
*/
#define XC_PREFERENCE_RELATION              "preference.relation"

/*
* // <TODO>系统参数：服务器上指定媒体流不存在时自动创建媒体流
* "preference.auto-create":{
*   "get":{
*     // true打开,false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开,false关闭
*     "*params":"vbool"
*   }
* },
*/
#define XC_AUTO_STREAM                      "preference.auto-stream"

/*
* // <TODO>系统参数：扩展字段(未公开)
* "preference.extra-data":{
*   "get":{
*     "return":"vbytes"
*   },
*   "set":{
*     "*params":"vbytes"
*   }
* },
*/
#define XC_PREFERENCE_EXTRA_DATA            "preference.extra-data"

/*
* // <TODO>系统参数：获取xcast支持的所有spear角色列表
* "preference.spear.roles":{
*   "get":{
*     "return":["vstring"]
*   }
* },
*/
#define XC_PREFERENCE_SPEAR_ROLES           "preference.spear.roles"

/*
* // <TODO>系统参数：设置/获取当前spear角色名
* "preference.role":{
*   "get":{
*     // 当前角色名
*     "return":"vstring"
*   },
*   "set":{
*     // 有效的角色名，存在于"preference.spear.roles"当中
*     "*params":"vstring",
*     "return":null
*   }
* },
*/
#define XC_PREFERENCE_ROLE                  "preference.role"

/*
* // <TODO>系统参数：获取当前spear角色类型
* "preference.role-type":{
*   "get":{
*     "params":null,
*     "return":"vint32"
*   }
* },
*/
#define XC_PREFERENCE_ROLE_TYPE             "preference.role-type"

/*
* // <TODO>系统参数：设置/获取当前系统中外部音频/视频设备数量
* "preference.external-device":{
*   "get":{
*     // 系统中存在的设备数量，默认值为8
*     "return":"vint32"
*   },
*   "set":{
*     "*params":"vint32"
*   }
* },
*/
#define XC_PREFERENCE_EXTERNAL_DEVICE       "preference.external-device"

/*
* // <TODO>系统参数：设置/获取当前摄像头预览配置
* "preference.camera-preview":{
*   "get":{
*     "return":{
*       "*format":["argb32", "i420"],
*       "*width":"vuint32",
*       "*height":"vuint32"
*     }
*   },
*   "set":{
*     "*params":{
*       "*format":["argb32","i420"],
*       "*width":"vuint32",
*       "*height":"vuint32"
*     }
*   }
* },
*/
#define XC_PREFERENCE_CAMERA_PREVIEW       "preference.camera-preview"

/* xcast支持的音频参数属性 */

/*
* // <TODO>音频参数：音频场景
* "preference.audio.scene":{
*   "get":{
*     // 当前音频场景
*     "return":["voicechat","playrecord","playback"]
*   },
*   "set":{
*     // 音频场景
*     "*params":["voicechat","playrecord","playback"]
*   }
* },
*/
#define XC_PREFERENCE_AUDIO_SCENE           "preference.audio.scene"

/*
* // <TODO>音频参数：终端专用，高音质开关
* "preference.audio.hd":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
* },
*/
#define XC_PREFERENCE_AUDIO_HD              "preference.audio.hd"

/*
* // <TODO>音频参数：媒体流连接成功后自动打开音频上行
* "preference.audio.auto-out":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
* },
*/
#define XC_PREFERENCE_AUDIO_AUTO_MIC        "preference.audio.auto-out"

/*
* // <TODO>音频参数：媒体流连接成功后自动打开音频下行
* "preference.audio.auto-in":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
* },
*/
#define XC_PREFERENCE_AUDIO_AUTO_SPEAKER    "preference.audio.auto-in"

/*
* // <TODO>音频参数：背景音乐模式
* "preference.audio.background-mode":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
* },
*/
#define XC_PREFERENCE_AUDIO_BACKGROUND_MODE "preference.audio.background-mode"

/*
* // <TODO>音频参数：设置音源音频格式
* "preference.audio.format":{
*   "get":{
*     // 音频类型
*     "params":[
*       "mic",          // 本地麦克风
*       "mix-out",      // 麦克风混音器
*       "net-out",      // 网络输出
*       "mix-in",       // 扬声器混音器
*       "speaker",      // 本地扬声器
*       "net-in",       // 网络输入
*       "mic-dispose",  // 麦克风预处理
*       "mix-out-sync", // 麦克风混音器(同步)
*       "aac-raw"       // AAC数据(未公开)
*     ],
*     "return":[
*       null,
*       {
*         "sample-rate":"vuint32",
*         "channel-num":"vuint32",
*         "bits":"vuint32",
*         // 返回音源列表中的某一个
*         "source":[
*           "mic",          // 本地麦克风
*           "mix-out",      // 麦克风混音器
*           "net-out",      // 网络输出
*           "mix-in",       // 扬声器混音器
*           "speaker",      // 本地扬声器
*           "net-in",       // 网络输入
*           "mic-dispose",  // 麦克风预处理
*           "mix-out-sync", // 麦克风混音器(同步)
*           "aac-raw"       // AAC数据(未公开)
*         ]
*       }
*     ]
*   },
*   "set":{
*     "params":{
*       // 请求音源列表中的某一个
*       "*source":[
*         "mic",          // 本地麦克风
*         "mix-out",      // 麦克风混音器
*         "net-out",      // 网络输出
*         "mix-in",       // 扬声器混音器
*         "speaker",      // 本地扬声器
*         "net-in",       // 网络输入
*         "mic-dispose",  // 麦克风预处理
*         "mix-out-sync", // 麦克风混音器(同步)
*         "aac-raw"       // AAC数据(未公开)
*       ],
*       "*sample-rate":"vuint32",
*       "*channel-num":"vuint32",
*       "*bits":"vuint32"
*     },
*     "return":null
*   }
*/
#define XC_PREFERENCE_AUDIO_FORMAT          "preference.audio.format"

/*
* // <TODO>音频参数：设置音源音量
* "preference.audio.volume":{
*   "get":{
*     // 音频类型
*     "params":[
*       "mic",          // 本地麦克风
*       "mix-out",      // 麦克风混音器
*       "net-out",      // 网络输出
*       "mix-in",       // 扬声器混音器
*       "speaker",      // 本地扬声器
*       "net-in",       // 网络输入
*       "mic-dispose",  // 麦克风预处理
*       "mix-out-sync", // 麦克风混音器(同步)
*       "aac-raw"       // AAC数据(未公开)
*     ],
*     "return":[
*       null,
*       "vfloat"
*     ]
*   },
*   "set":{
*     "params":{
*       // 请求音源列表中的某一个
*       "*source":[
*         "mic",          // 本地麦克风
*         "mix-out",      // 麦克风混音器
*         "net-out",      // 网络输出
*         "mix-in",       // 扬声器混音器
*         "speaker",      // 本地扬声器
*         "net-in",       // 网络输入
*         "mic-dispose",  // 麦克风预处理
*         "mix-out-sync", // 麦克风混音器(同步)
*         "aac-raw"       // AAC数据(未公开)
*       ],
*       "*volume":"vfloat"
*     },
*     "return":null
*   }
*/
#define XC_PREFERENCE_AUDIO_VOLUME          "preference.audio.volume"

/*
* // <TODO>音频参数：设置音源DB音量
* "preference.audio.db-volume":{
*   "get":{
*     // 音频类型
*     "params":[
*       "mic",          // 本地麦克风
*       "mix-out",      // 麦克风混音器
*       "net-out",      // 网络输出
*       "mix-in",       // 扬声器混音器
*       "speaker",      // 本地扬声器
*       "net-in",       // 网络输入
*       "mic-dispose",  // 麦克风预处理
*       "mix-out-sync", // 麦克风混音器(同步)
*       "aac-raw"       // AAC数据(未公开)
*     ],
*     "return":[
*       null,
*       "vint32"
*     ]
*   },
*   "set":{
*     "params":{
*       // 请求音源列表中的某一个
*       "*source":[
*         "mic",          // 本地麦克风
*         "mix-out",      // 麦克风混音器
*         "net-out",      // 网络输出
*         "mix-in",       // 扬声器混音器
*         "speaker",      // 本地扬声器
*         "net-in",       // 网络输入
*         "mic-dispose",  // 麦克风预处理
*         "mix-out-sync", // 麦克风混音器(同步)
*         "aac-raw"       // AAC数据(未公开)
*       ],
*       "*volume":"vint32"
*     },
*     "return":null
*   }
*/
#define XC_PREFERENCE_AUDIO_DB_VOLUME       "preference.audio.db-volume"

/*
* // <TODO>音频参数：打开/关闭音频spatialize,仅支持win
* "preference.audio.spatialize":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     "params":{
*       // true打开，false关闭
*       "*enable":"vbool",
*       "*uin":"vuint64",
*       "*azimuth":"vfloat",
*       "*elevation":"vfloat",
*       "*distance-cm":"vfloat"
*     }
*   }
*/
#define XC_PREFERENCE_AUDIO_SPATIALIZE      "preference.audio.spatialize"

/* xcast支持的视频参数属性 */

/*
* // <TODO>视频参数：视频水印
* "preference.video.watermark":{
*   "set":{
*     "params":{
*       // 设置水印的目标视频编码分辨率类型
*       "*type":[
*         "320*240",    // 目标编码分辨率为320*240
*         "480*360",    // 目标编码分辨率为480*360
*         "640*480",    // 目标编码分辨率为640*480
*         "640*368",    // 目标编码分辨率为640*368
*         "960*540",    // 目标编码分辨率为960*540
*         "1280*720",   // 目标编码分辨率为1280*720
*         "192*144",    // 目标编码分辨率为192*144
*         "320*180"     // 目标编码分辨率为192*144
*       ],
*       // argb格式水印数据缓存
*       "*argb-data":"vbytes",
*       // 水印宽度
*       "*width":"vint32",
*       // 水印高度
*       "*height":"vint32"
*     }
*   }
*/
#define XC_PREFERENCE_VIDEO_WATERMARK       "preference.video.watermark"

/*
* // <TODO>视频参数：美颜程度
* "preference.video.skin-smooth":{
*   "get":{
*     // 取值范围在0-9之间，0表示美颜关闭
*     "return":"vint32"
*   },
*   "set":{
*     // 取值范围在0-9之间，0表示美颜关闭
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_SKIN_SMOOTH     "preference.video.skin-smooth"

/* xcast支持的音频spear属性 */

/*
* // <TODO>(当前角色)音频spear配置：aec
* "preference.audio.aec":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
*/
#define XC_PREFERENCE_AUDIO_AEC             "preference.audio.aec"

/*
* // <TODO>(当前角色)音频spear配置：agc
* "preference.audio.agc":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
*/
#define XC_PREFERENCE_AUDIO_AGC             "preference.audio.agc"

/*
* // <TODO>(当前角色)音频spear配置：ans
* "preference.audio.ans":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
*/
#define XC_PREFERENCE_AUDIO_ANS             "preference.audio.ans"

/*
* // <TODO>(当前角色)音频spear配置：冗余抗丢包
* "preference.audio.anti_dropout":{
*   "get":{
*     // -1自适应, 0关，1开
*     "return":[-1,0,1]
*   },
*   "set":{
*     // -1自适应, 0关，1开
*     "*params":[-1,0,1]
*   }
*/
#define XC_PREFERENCE_AUDIO_ANTI_DROPOUT    "preference.audio.anti_dropout"

/*
* // <TODO>(当前角色)音频spear配置：音频场景
* "preference.audio.au_scheme":{
*   "get":{
*     // 2种场景: 1主播，2观众
*     "return":[1,2]
*   },
*   "set":{
*     // 2种场景: 1主播，2观众
*     "*params":[1,2]
*   }
*/
#define XC_PREFERENCE_AUDIO_AU_SCHEME       "preference.audio.au_scheme"

/*
* // <TODO>(当前角色)音频spear配置：通道数
* "preference.audio.channel":{
*   "get":{
*     // 1单通道，2双通道
*     "return":"vint32"
*   },
*   "set":{
*     // 1单通道，2双通道
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_CHANNEL         "preference.audio.channel"

/*
* // <TODO>(当前角色)音频spear配置：音频编码协议
* "preference.audio.codec_prof":{
*   "get":{
*     // 固定为4106(acc)
*     "return":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_CODEC_PROF      "preference.audio.codec_prof"

/*
* // <TODO>(当前角色)音频spear配置：帧长
* "preference.audio.frame":{
*   "get":{
*     // -1自适应, 自定义2个档位: 40, 80
*     "return":[-1,40,80]
*   },
*   "set":{
*     // -1自适应, 自定义2个档位: 40, 80
*     "*params":[-1,40,80]
*   }
*/
#define XC_PREFERENCE_AUDIO_FRAME           "preference.audio.frame"

/*
* // <TODO>(当前角色)音频spear配置：编码码率
* "preference.audio.kbps":{
*   "get":{
*     // -1自适应, 最小值10, 最大值64
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值10, 最大值64
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_KBPS            "preference.audio.kbps"

/*
* // <TODO>(当前角色)音频spear配置：采样率
* "preference.audio.sample_rate":{
*   "get":{
*     // 固定为48000
*     "return":[48000]
*   }
*/
#define XC_PREFERENCE_AUDIO_SAMPLE_RATE     "preference.audio.sample_rate"

/*
* // <TODO>(当前角色))音频spear配置：静音检测
* "preference.audio.silence_detect":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
*/
#define XC_PREFERENCE_AUDIO_SILENCE_DETECT  "preference.audio.silence_detect"

/* xcast支持的视频spear属性 */

/*
* // <TODO>(当前角色)视频spear配置：视频编码协议
* "preference.video.codec_prof":{
*   "get":{
*     // -1自适应, 5(h264)
*     "return":[-1,5]
*   },
*   "set":{
*     // -1自适应, 5(h264)
*     "*params":[-1,5]
*   }
*/
#define XC_PREFERENCE_VIDEO_CODEC_PROF      "preference.video.codec_prof"

/*
* // <TODO>(当前角色)视频spear配置：固定图像高
* "preference.video.format_fix_height":{
*   "get":{
*     "params": null,
*     // 和WEB流控配置可选分辨率对齐, 其他分辨率不支持
*     "return":[368]
*   }
*/
#define XC_PREFERENCE_VIDEO_FORMAT_FIX_HEIGHT "preference.video.format_fix_height"

/*
* // <TODO>(当前角色)视频spear配置：固定图像宽
* "preference.video.format_fix_width":{
*   "get":{
*     // 和WEB流控配置可选分辨率对齐, 其他分辨率不支持
*     "return":[640]
*   }
*/
#define XC_PREFERENCE_VIDEO_FORMAT_FIX_WIDTH  "preference.video.format_fix_width"

/*
* // <TODO>(当前角色)视频spear配置：最大图像高
* "preference.video.format_max_height":{
*   "get":{
*     // 和WEB流控配置可选分辨率对齐, 其他分辨率不支持
*     "return":[-1]
*   }
*/
#define XC_PREFERENCE_VIDEO_FORMAT_MAX_HEIGHT "preference.video.format_max_height"

/*
* // <TODO>(当前角色)视频spear配置：最大图像宽
* "preference.video.format_max_width":{
*   "get":{
*     // 和WEB流控配置可选分辨率对齐, 其他分辨率不支持
*     "return":[-1]
*   }
*/
#define XC_PREFERENCE_VIDEO_FORMAT_MAX_WIDTH  "preference.video.format_max_width"

/*
* // <TODO>(当前角色)视频spear配置：编码帧率
* "preference.video.fps":{
*   "get":{
*     // -1自适应, 最小值10, 最大值30
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值10, 最大值30
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_FPS               "preference.video.fps"

/*
* // <TODO>(当前角色)视频spear配置：自适应流控
* "preference.video.live_adapt":{
*   "get":{
*     // -1自适应, 0关，1开
*     "return":[-1,0,1]
*   },
*   "set":{
*     // -1自适应, 0关，1开
*     "*params":[-1,0,1]
*   }
*/
#define XC_PREFERENCE_VIDEO_LIVE_ADAPT        "preference.video.live_adapt"

/*
* // <TODO>(当前角色)视频spear配置：帧序列时间
* "preference.video.fqueue_time":{
*   "get":{
*     // -1自适应, 最小值1, 最大值5
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值1, 最大值5
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_FQUEUE_TIME       "preference.video.fqueue_time"

/*
* // <TODO>(当前角色)视频spear配置：最大码率
* "preference.video.maxkbps":{
*   "get":{
*     // 最小值30, 最大值1500
*     "return":"vint32"
*   },
*   "set":{
*     // 最小值30, 最大值1500
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_MAXKBPS           "preference.video.maxkbps"

/*
* // <TODO>(当前角色)视频spear配置：最小码率
* "preference.video.minkbps":{
*   "get":{
*     // 最小值30, 最大值1500
*     "return":"vint32"
*   },
*   "set":{
*     // 最小值30, 最大值1500
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_MINKBPS           "preference.video.minkbps"

/*
* // <TODO>(当前角色)视频spear配置：最大QP
* "preference.video.maxqp":{
*   "get":{
*     // -1自适应, 最小值10, 最大值51
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值10, 最大值51
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_MAXQP             "preference.video.maxqp"

/*
* // <TODO>(当前角色)视频spear配置：最小QP
* "preference.video.minqp":{
*   "get":{
*     // -1自适应, 最小值10, 最大值51
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值10, 最大值51
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_MINQP             "preference.video.minqp"

/*
* // <TODO>(当前角色)视频spear配置：qclear
* "preference.video.qclear":{
*   "get":{
*     // -1自适应, 0关，1开
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 0关，1开
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_QCLEAR            "preference.video.qclear"

/*
* // <TODO>(当前角色)视频spear配置：冗余抗丢包
* "preference.video.anti_dropout":{
*   "get":{
*     // -1自适应, 0关，1开
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 0关，1开
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_ANTI_DROPOUT      "preference.video.anti_dropout"

/*
* // <TODO>(当前角色)视频spear配置：小画面上传
* "preference.video.small_video_upload":{
*   "get":{
*     // 未公开
*     "return":[0]
*   }
*/
#define XC_PREFERENCE_VIDEO_SMALL_VIDEO_UPLOAD  "preference.video.small_video_upload"

/* xcast支持的网络spear属性 */

/*
* // <TODO>(当前角色)网络spear配置：重传抗丢包
* "preference.net.rc_anti_dropout":{
*   "get":{
*     // -1自适应, 0关，1开
*     "return":[-1,0,1]
*   },
*   "set":{
*     // -1自适应, 0关，1开
*     "*params":[-1,0,1]
*   }
*/
#define XC_PREFERENCE_NET_RC_ANTI_DROPOUT       "preference.net.rc_anti_dropout"

/*
* // <TODO>(当前角色)网络spear配置：重传初始延迟
* "preference.net.rc_init_delay":{
*   "get":{
*     // -1自适应, 最小值0, 最大值10000
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值0, 最大值10000
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_NET_RC_INIT_DELAY         "preference.net.rc_init_delay"

/*
* // <TODO>(当前角色)网络spear配置：重传最大延迟
* "preference.net.rc_max_delay":{
*   "get":{
*     // -1自适应, 最小值0, 最大值10000
*     "return":"vint32"
*   },
*   "set":{
*     // -1自适应, 最小值0, 最大值10000
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_NET_RC_MAX_DELAY          "preference.net.rc_max_delay"

/*
* // <TODO>(当前角色)网络spear配置：多次请求拉接口机iplist
* "preference.net.multi-request":{
*   "get":{
*     // true打开，false关闭
*     "return":"vbool"
*   },
*   "set":{
*     // true打开，false关闭
*     "*params":"vbool"
*   }
*/
#define XC_PREFERENCE_NET_MULTI_REQUEST         "preference.net.multi-request"

/*
* // 用户配置：最大视频码率
* "preference.custom.video.videomaxbps":{
*   "get":{
*     // 最小值0, 最大值10000
*     "return":"vint32"
*   },
*   "set":{
*     // 最小值0, 最大值10000
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_CUSTOM_VIDEO_MAXBPS       "preference.custom.videomaxbps"

#define XC_LOG_WARN         3
#define XC_LOG_INFO         4
#define XC_LOG_DEBUG        5
#define XC_LOG_VERBOSE      6

#endif /* XCAST_DEFINE_H */

/*
** Copyright (c) 2014-2017 The xCast project. All rights reserved.
*/
#ifndef XCAST_DEFINE_H
#define XCAST_DEFINE_H

typedef enum xc_auth_type_e {
  xc_auth_none = 0,
  xc_auth_manual,
  xc_auth_auto,
} xc_auth_type;

/* xcast system events */
typedef enum xc_system_event_e {
  xc_system_normal = 1,
  xc_system_warning,
  xc_system_error,
  xc_system_fatal,
} xc_system_event;

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

/* xcast event list */

/*
* xcast events notify path defines.
* register callbacks through 'xcast_handle_event()' can receive
* notifies from xcast, the notified event value is a 'xcast_variant_t' of 'vdict' type.
* // xcast event discription, ver 000.000.1
* // attention: a 'xcast_variant_t(vdict)' type is used in xcast event callback as parameter.
* // the event has the following format(item with "*" is a required item):
* //   "event path":{
* //     // from where the event come from
* //     "*src":"vstring",
* //     // type of the event
* //     "*type":"vstring",
* //     // event state
* //     "state":"vstring",
* //     // error code
* //     "err":"vint32",
* //     // error message
* //     "err-msg":"vstring",
* //     // other custom event items
* //     ...
* //   }
* //
* // "[]" means the value is an array or option list, "{}" means value has a 'vdict' type.
* //
*/

/*
* xcast system event
* "event.system":{
*   // from where the event come from:
*   "*src":["system"],
*   // type of the event: warning,error,fatal
*   "*type":[xc_system_event],
*   // error code
*   "err":"vint32",
*   // error message
*   "err-msg":"vstring"
* },
*/
#define XC_EVENT_SYSTEM                     "event.system"

/*
* xcast stream event
* "event.stream":{
*   // from where the event come from: name of the stream
*   "*src":"vstring",
*   // type of the event, one of 'xc_stream_event'
*   "*type":[xc_stream_event],
*   // event state, one of 'xc_stream_state'
*   "state":[xc_stream_state],
*   // error code
*   "err":"vint32",
*   // error message
*   "err-msg":"vstring"
* },
*/
#define XC_EVENT_STREAM                     "event.stream"

/*
* xcast stream track event
* "event.track":{
*   // from where the event come from: track name
*   "*src":"vstring",
*   // which stream the track belong: stream name
*   "*stream":"vstring",
*   // type of the event, one of 'xc_track_event'
*   "*type":[xc_track_event],
*   // track class, one of 'xc_track_type'
*   "*class":[xc_track_type],
*   // direction of the track, one of 'xc_track_direction'
*   "*direction":[xc_track_direction],
*   // which user the track belong
*   "*uin":"vuint64",
*   // index of the track, start from 0
*   "*index":"vuint32",
*   // track state, one of 'xc_track_state'
*   "state":[xc_track_state],
*   // error code
*   "err":"vint32",
*   // error message
*   "err-msg":"vstring",
*   // track media data format, one of 'xc_media_format'
*   "format":[xc_media_format],
*   // track media raw data
*   "data":"vbytes",
*   // media data size
*   "size":"vuint32",
*   // media data width
*   "width":"vuint32",
*   // media data height
*   "height":"vuint32",
*   // rotation of the media data
*   "rotate":[0,90,180,270]
*   // media source of the track, one of 'xc_media_source'
*   "media-src":[xc_media_source]
* },
*/
#define XC_EVENT_TRACK                      "event.track"

/*
* xcast device event
* "event.device":{
*   // from where the event come from: device name
*   "*src":"vstring",
*   // type of the event, one of 'xc_device_event'
*   "*type":[xc_device_event],
*   // type of the device, one of 'xc_device_type'
*   "*class":[xc_device_type],
*   // device state, one of 'xc_device_state'
*   "state":[xc_device_state],
*   // error code
*   "err":"vint32",
*   // error message
*   "err-msg":"vstring",
*   // device data format
*   "format":[xc_media_argb32,xc_media_i420,xc_media_aac],
*   // device data
*   "data":"vbytes",
*   // size of device data
*   "size":"vuint32",
*   // width of device data
*   "width":"vint32",
*   // height of device data
*   "height":"vint32",
*   // rotate of device data
*   "rotate":[0,90,180,270],
*   // audio device only, volume value of [0,100]
*   "volume":"vuint32",
*   // audio device only, dynamic volume value of [0,100]
*   "dynamic-volume":"vuint32",
*   // media file player only, player path
*   "player-path":"vstring",
*   // media file player only, file path
*   "file-path":"vstring",
*   // media file player only, current position
*   "current-pos":"vint64",
*   // media file player only, max position
*   "max-pos":"vint64",
*   // accompany only, source of accompany
*   "accompany-source":["none","system","process"],
*   // screen capture only, left margin of capture rect
*   "screen-left":"vint32",
*   // screen capture only, top margin of capture rect
*   "screen-top":"vint32",
*   // screen capture only, right margin of capture rect
*   "screen-right":"vint32",
*   // screen capture only, bottom margin of capture rect
*   "screen-bottom":"vint32",
*   // screen capture only, capture fps
*   "screen-fps":"vuint32",
*   // windows screen capture only, capture window handler
*   "screen-hwnd":"vint32"
* },
*/
#define XC_EVENT_DEVICE                     "event.device"

/*
* // xcast tips event
* "event.tips":{
*   // from where the event come from: "audio","video"
*   "*src":["audio","video"],
*   "*tips":"vstring"
* }
*/
#define XC_EVENT_STATISTIC_TIPS             "event.tips"

/*
* // xcast properties description, ver 000.000.1
* // attention: only 'xcast_variant_t' type is supported as property value in xcast.
* // property has the following format:
* //   "property path":{
* //     "property get":{
* //       // return value: item with "*" is required, while "null" means not exist.
* //       "return":[null, {other value}]
* //     },
* //     "property set":{
* //       // param value: item with "*" is required, while "null" means not exist.
* //       "params":[null, {other value}],
* //     }
* // "[]" means the value is an array or option list, "{}" means value has a 'vdict' type.
* //
*/

/* xcast stream property */

/*
* // stream property: get running stream list in xcast
* "stream":{
*   "get":{
*     "return":[
*       // "null" means not exist
*       null,
*       // array of stream names
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_STREAM                           "stream"

/*
* // stream property: query stream state with the given name
* "stream.%s.state":{
*   "get":{
*     "return":[
*       // "null" means not exist
*       null,
*       // stream state, one of 'xc_stream_state'
*       [xc_stream_state]
*     ]
*   }
* },
*/
#define XC_STREAM_STATE                     "stream.%s.state"

/*
* // stream property: query track list in a given stream
* "stream.%s.track":{
*   "get":{
*     "return":[
*       // "null" means not exist
*       null,
*       // array of track names in the stream
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_STREAM_TRACK                     "stream.%s.track"

/*
* // track property: start/stop named track in a given stream
* "stream.%s.%s.enable":{
*   "set":{
*     "params":{
*       // while 'true' is for enable, and 'false' is for disable
*       "*enable":"vbool",
*       // optional, specify video size for video track only
*       "size":["small","big"]
*     }
*   }
* },
*/
#define XC_TRACK_ENABLE                     "stream.%s.%s.enable"

/*
* // track property: query named track state
* "stream.%s.%s.state":{
*   "get":{
*     // return track state, one of 'xc_track_state'
*     "return":[xc_track_state]
*   }
* },
*/
#define XC_TRACK_STATE                      "stream.%s.%s.state"

/*
* // track property: set capture for a given track in stream
* "stream.%s.%s.capture":{
*   "get":{
*     "return":[
*       // no capture found
*       null,
*       // capture name, such as a mic or a camera
*       "vstring"
*     ]
*   },
*   "set":{
*     // capture name to be set
*     "params":"vstring"
*   }
* },
*/
#define XC_TRACK_CAPTURE                    "stream.%s.%s.capture"

/* xcast camera property */

/*
* // camera property: get available camera list in system
* "device.camera":{
*   "get":{
*     // array of camera names
*     "return":["vstring"]
*   }
* },
*/
#define XC_CAMERA                           "device.camera"

/*
* // camera property: enable/disable all cameras in system
* "device.camera.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_CAMERA_ENABLE                    "device.camera.enable"

/*
* // camera property: get/set default camera for xcast, the default camera
* // is used when camera not specified for a track.
* "device.camera.default":{
*   "get":{
*     "return":[
*       // no default camera
*       null,
*       // default camera name
*       "vstring"
*     ]
*   },
*   "set":{
*     // camera to be set
*     "params":"vstring"
*   }
* },
*/
#define XC_CAMERA_DEFAULT                   "device.camera.default"

/*
* // camera property: start/stop camera preview, the preivew data
* // is received through device preview event when preview on
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
* // camera property: query camera state
* "device.camera.%s.state":{
*   "get":{
*     "return":[
*       // no camera exist
*       null,
*       // camera state, one of 'xc_device_state'
*       [xc_device_state]
*     ]
*   }
* },
*/
#define XC_CAMERA_STATE                     "device.camera.%s.state"

/*
* // camera property: enable/disable camera data preprocess, the camera data
* // is received through device event for preprocess when preprocess on
* "device.camera.%s.preprocess":{
*   "get":{
*     "return":[
*       // no camera exist
*       null,
*       // camera preprocess state
*       "vbool"
*     ]
*   },
*   "set":{
*     // camera preprocess state
*     "params":"vbool"
*   }
* },
*/
#define XC_CAMERA_PREPROCESS                "device.camera.%s.preprocess"

/* xcast mic property */

/*
* // mic property: get available mic list in system
* "device.mic":{
*   "get":{
*     "return":[
*       // "null" means not exist
*       null,
*       // array of mic names
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_MIC                              "device.mic"

/*
* // mic property: enable/disable all mics in xcast
* "device.mic.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_MIC_ENABLE                       "device.mic.enable"

/*
* // mic property: get/set default mic for xcast, the default mic
* // is used when mic not specified for a track.
* "device.mic.default":{
*   "get":{
*     "return":[
*       // no mic exist
*       null,
*       // default mic name
*       "vstring"
*     ]
*   },
*   "set":{
*     // mic name to set
*     "params":"vstring"
*   }
* },
*/
#define XC_MIC_DEFAULT                      "device.mic.default"

/*
* // mic property: query state of the given mic
* "device.mic.%s.state":{
*   "get":{
*     "return":[
*       // no mic exist
*       null,
*       // mic state, one of 'xc_device_state'
*       [xc_device_state]
*     ]
*   }
* },
*/
#define XC_MIC_STATE                        "device.mic.%s.state"

/*
* // mic property: start/stop mic preview, audio captured by this mic
* // is played by the working speaker when preview on
* "device.mic.%s.preview":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_MIC_PREVIEW                      "device.mic.%s.preview"

/*
* // mic property: get/set mic volume
* "device.mic.%s.volume":{
*   "get":{
*     // volume value of [0,100], default is 100
*     "return":"vuint32"
*   },
*   "set":{
*     "params":"vuint32"
*   }
* },
*/
#define XC_MIC_VOLUME                       "device.mic.%s.volume"

/*
* // mic property: get mic dynamic volume
* "device.mic.%s.dynamic-volume":{
*   "get":{
*     // volume value of [0~100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_MIC_DYNAMIC_VOLUME               "device.mic.%s.dynamic-volume"

/* xcast speaker property */

/*
* // speaker property: get available speakers in system
* "device.speaker":{
*   "get":{
*     "return": [
*       // no speaker exist
*       null,
*       // array of speaker names
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_SPEAKER                          "device.speaker"

/*
* // speaker property: enable/disable all speakers in system
* "device.speaker.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   },
* },
*/
#define XC_SPEAKER_ENABLE                   "device.speaker.enable"

/*
* // speaker property: get/set default speaker for xcast, the default speaker
* // is used as the audio output in xcast.
* "device.speaker.default":{
*   "get":{
*     "return":[
*       // no speaker exist
*       null,
*       // default speaker name
*       "vstring"
*     ]
*   },
*   "set":{
*     // speaker name to set
*     "params":"vstring"
*   }
* },
*/
#define XC_SPEAKER_DEFAULT                  "device.speaker.default"

/*
* // speaker property: enable/disable earphone mode in xcast
* "device.speaker.earphone-mode":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   }
* },
*/
#define XC_SPEAKER_EARPHONE_MODE            "device.speaker.earphone-mode"

/*
* // speaker property: query state of the given speaker
* "device.speaker.%s.state":{
*   "get":{
*     "return":[
*       // no speaker exist
*       null,
*       // speaker state, one of 'xc_device_state'
*       [xc_device_state]
*     ]
*   }
* },
*/
#define XC_SPEAKER_STATE                    "device.speaker.%s.state"

/*
* // speaker property: start/stop speaker preview, will start speaker as the working
* // one in xcast when preview on, the speaker is stopped when preview is off.
* "device.speaker.%s.preview":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   }
* },
*/
#define XC_SPEAKER_PREVIEW                  "device.speaker.%s.preview"

/*
* // speaker property: get/set speaker volume
* "device.speaker.volume":{
*   "get":{
*     // volume value of [0~100], default is 100
*     "return":"vuint32"
*   },
*   "set":{
*     "params":"vuint32"
*   }
* },
*/
#define XC_SPEAKER_VOLUME                   "device.speaker.%s.volume"

/*
* // speaker property: get speaker dynamic volume
* "device.speaker.%s.dynamic-volume":{
*   "get":{
*     // volume value of [0~100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_SPEAKER_DYNAMIC_VOLUME           "device.speaker.%s.dynamic-volume"

/* xcast external device property */

/*
* // external device property: get available external device list in xcast
* "device.external":{
*   "get":{
*     "return": [
*       // no external device exist
*       null,
*       // array of device names
*       ["vstring"]
*     ]
*   }
* },
*/
#define XC_DEVICE_EXTERNAL                  "device.external"

/*
* // external device property: input external data into xcast
* "device.external.%s.input":{
*   "set":{
*     "params":{
*       // data format
*       "*format":[xc_media_format],
*       // raw data
*       "*data":"vbytes",
*       // data size
*       "*size":"vuint32",
*       // data width
*       "*width":"vint32",
*       // data height
*       "height":"vint32",
*       // rotate
*       "rotate":[0,90,180,270],
*       // sample rate of raw data
*       "sample-rate":"vuint32",
*       // channel count, value of [1, 2]
*       "channel":[1, 2],
*       // data bits of [8, 16]
*       "bits":[8, 16]
*     }
*   }
* },
*/
#define XC_DEVICE_EXTERNAL_INPUT            "device.external.%s.input"

/*
* external device property: set external device source type
* "device.external.%s.type":{
*   "set":{
*     "params":[
*       // source type, one of 'xc_media_source'
*       [xc_media_source]
*     ]
*   }
* },
*/
#define XC_DEVICE_EXTERNAL_TYPE            "device.external.%s.type"

/*
* // external device property: query external device state
* "device.external.%s.state":{
*   "get":{
*     "return":[
*       // no device exist
*       null,
*       // device state, one of 'xc_device_state'
*       [xc_device_state]
*     ]
*   }
* },
*/
#define XC_DEVICE_EXTERNAL_STATE            "device.external.%s.state"

/* xcast media file play property */

/*
* // <TODO>media file property: enable/disable media file play, use with
* // "device.media-file.setting" property.
* // format supported: *.aac,*.ac3,*.amr,*.ape,*.mp3,*.flac,*.midi,*.wav,*.wma,*.ogg,
* //   *.amv,*.mkv,*.mod,*.mts,*.ogm,*.f4v,*.flv,*.hlv,*.asf,*.avi,*.wm,*.wmp,
* //   *.wmv,*.ram,*.rm,*.rmvb,*.rpm,*.rt,*.smi,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,
* //   *.m2v,*.mp2v, *.tp,*.tpr,*.ts,*.m4b,*.m4p,*.m4v,*.mp4,*.mpeg4,*.3g2,*.3gp,
* //   *.3gp2,*.3gpp,*.mov,*.pva,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,*.mp2v,*.pss,
* //   *.pva,*.ifo,*.vob,*.divx,*.evo,*.ivm,*.mkv,*.mod,*.mts,*.ogm,*.scm,*.tod,*.vp6,
* //   *.webm,*.xlmv
* "device.media-file.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   }
* },
*/
#define XC_MEDIA_FILE_ENABLE                "device.media-file.enable"

/*
* // <TODO>media file property: media file play setting
* "device.media-file.setting":{
*   "get":{
*     "return":{
*       "file":"vstring",                     // media file path
*       "state":["init","playing","paused"],  // play state
*       "pos":"vint64",                       // current play position
*       "max-pos":"vint64"                    // max position
*     }
*   },
*   "set":{
*     "params":{
*       "file":"vstring",                     // media file path
*       "restart":"vbool",                    // restart play
*       "state":["init","playing","paused"],  // play state
*       "pos":"vint64",                       // current play position
*       "max-pos":"vint64"                    // max position
*     }
*   }
* },
*/
#define XC_MEDIA_FILE_SETTING               "device.media-file.setting"

/*
* // <TODO>media file property: media file check
* "device.media-file.verify":{
*   "get":{
*     "params":{
*       "file":"vstring",                     // media file path
*       "loop-back":"vbool"                   // loop back mode
*     },
*     "return":{
*       "valid":"vbool",                      // 'true' when valid, otherwise 'false'
*       "audio-only":"vbool"                  // 'true' when the file only contain audio
*     }
*   }
* },
*/
#define XC_MEDIA_FILE_VERIFY                "device.media-file.verify"

/*
* // screen capture: enable/disable screen preview
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
* // screen capture: capture setting
* "device.screen-capture.setting":{
*   "get":{
*     "return":{
*       "left":"vint32",
*       "top":"vint32",
*       "right":"vint32",
*       "bottom":"vint32",
*       "fps":"vuint32",    // capture fps, value of [1~20]
*       "hwnd":"vint32"     // handle of capture window, windows only
*     }
*   },
*   "set":{
*     "params":{
*       "left":"vint32",
*       "top":"vint32",
*       "right":"vint32",
*       "bottom":"vint32",
*       "fps":"vuint32",    // capture fps, value of [1~20]
*       "hwnd":"vint32"     // handle of capture window, windows only
*     }
*   }
* },
*/
#define XC_SCREEN_CAPTURE_SETTING           "device.screen-capture.setting"

/* xcast accompany property */

/*
* // <TODO>accompany property: enable/disable accompany
* "device.accompany.enable":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "params":"vbool"
*   }
* },
*/
#define XC_ACCOMPANY_ENABLE                 "device.accompany.enable"

/*
* // <TODO>accompany property: volume
* "device.accompany.volume":{
*   "get":{
*     // volume value of [0~100]
*     "return":"vuint32"
*   },
*   "set":{
*     // volume value of [0~100]
*     "params":"vuint32"
*   }
* },
*/
#define XC_ACCOMPANY_VOLUME                 "device.accompany.volume"

/*
* // <TODO>accompany property: dynamic volume
* "device.accompany.dynamic-volume":{
*   "get":{
*     // volume value of [0~100]
*     "return":"vuint32"
*   }
* },
*/
#define XC_ACCOMPANY_DYNAMIC_VOLUME         "device.accompany.dynamic-volume"

/*
* // <TODO>accompany property: get/set accompany setting
* "device.accompany.setting":{
*   "get":{
*     "return":{
*       // player path
*       "player":"vstring",
*       // audio file path
*       "file":"vstring",
*       // accompany source
*       "source":["none","system","process"]
*     }
*   },
*   "set":{
*     "params":{
*       // player path
*       "*player":"vstring",
*       // audio file path
*       "*file":"vstring",
*       // accompany source
*       "*source":["none","system","process"]
*     }
*   }
* },
*/
#define XC_ACCOMPANY_SETTING                "device.accompany.setting"

/* xcast preference */

/*
* // preference: auto create stream on server when stream not exist
* "preference.auto-stream":{
*   "get":{
*     "return":"vbool"
*   },
*   "set":{
*     "*params":"vbool"
*   }
* },
*/
#define XC_PREFERENCE_AUTO_STREAM           "preference.auto-stream"

/*
* // preference: get spear roles supported by xcast
* "preference.spear.roles":{
*   "get":{
*     // array of spear role name
*     "return":["vstring"]
*   }
* },
*/
#define XC_PREFERENCE_SPEAR_ROLES           "preference.spear.roles"

/*
* // preference: get current spear role name
* "preference.role":{
*   "get":{
*     // role name
*     "return":"vstring"
*   },
*   "set":{
*     // valid role name, one of "preference.spear.roles"
*     "*params":"vstring",
*     "return":null
*   }
* },
*/
#define XC_PREFERENCE_ROLE                  "preference.role"

/*
* // preference: get current spear role type
* "preference.role-type":{
*   "get":{
*     "params":null,
*     "return":"vint32"
*   }
* },
*/
#define XC_PREFERENCE_ROLE_TYPE             "preference.role-type"

/*
* // preference: get/set external device count supported by xcast
* "preference.external-device":{
*   "get":{
*     // device count, default is 4.
*     "return":"vint32"
*   },
*   "set":{
*     "*params":"vint32"
*   }
* },
*/
#define XC_PREFERENCE_EXTERNAL_DEVICE       "preference.external-device"

/*
* // <TODO>preference: get/set camera preview setting
* "preference.camera-preview":{
*   "get":{
*     "return":{
*       "*format":[xc_media_argb32, xc_media_i420],
*       "*width":"vuint32",
*       "*height":"vuint32"
*     }
*   },
*   "set":{
*     "*params":{
*       "*format":[xc_media_argb32, xc_media_i420],
*       "*width":"vuint32",
*       "*height":"vuint32"
*     }
*   }
* },
*/
#define XC_PREFERENCE_CAMERA_PREVIEW       "preference.camera-preview"

/* xcast video preference */

/*
* // <TODO>video preference: set video watermark param
* "preference.video.watermark":{
*   "set":{
*     "params":{
*       // watermark resolution
*       "*type":[
*         "320*240",    // resolution: 320*240
*         "480*360",    // resolution: 480*360
*         "640*480",    // resolution: 640*480
*         "640*368",    // resolution: 640*368
*         "960*540",    // resolution: 960*540
*         "1280*720",   // resolution: 1280*720
*         "192*144",    // resolution: 192*144
*         "320*180"     // resolution: 192*144
*       ],
*       // argb format watermark data
*       "*argb-data":"vbytes",
*       // watermark width
*       "*width":"vint32",
*       // watermark height
*       "*height":"vint32"
*     }
*   }
*/
#define XC_PREFERENCE_VIDEO_WATERMARK       "preference.video.watermark"

/*
* // <TODO>video preference: skin smooth
* "preference.video.skin-smooth":{
*   "get":{
*     // value of [0~9], '0' means off
*     "return":"vint32"
*   },
*   "set":{
*     // value of [0~9], '0' means off
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_VIDEO_SKIN_SMOOTH     "preference.video.skin-smooth"

/* xcast audio preference */

/*
* // audio preference: force client aec
* "preference.audio.force-aec":{
*   "get":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "return":"vint32"
*   },
*   "set":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_AEC             "preference.audio.force-aec"

/*
* // audio preference: force client agc
* "preference.audio.force-agc":{
*   "get":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "return":"vint32"
*   },
*   "set":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_AGC             "preference.audio.force-agc"

/*
* // audio preference: force client ans
* "preference.audio.force-ans":{
*   "get":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "return":"vint32"
*   },
*   "set":{
*     // value of [-1(adaptive),0(off),1(on)]
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_AUDIO_ANS             "preference.audio.force-ans"

/*
* // custom preference: max video bps
* "preference.custom.videomaxbps":{
*   "get":{
*     // value of [0~10000]
*     "return":"vint32"
*   },
*   "set":{
*     // value of [0~10000]
*     "*params":"vint32"
*   }
*/
#define XC_PREFERENCE_CUSTOM_VIDEO_MAXBPS   "preference.custom.videomaxbps"

#define XC_LOG_WARN         3
#define XC_LOG_INFO         4
#define XC_LOG_DEBUG        5
#define XC_LOG_VERBOSE      6

#endif /* XCAST_DEFINE_H */
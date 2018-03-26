#include "xcast-ui-handler.h"
#include "include/xcast_variant.h"
#include "Live/XCastData.h"
#include "main-view.h"


extern void utf8_to_utf16(const char *str, uint32_t len, std::wstring& wstr);

int32_t new_video_preview_event(void *user_data, const XCastVideoFrame *frame)
{
	if (frame)
	{
		const char       *dev = frame->deviceSrc.c_str();

		if (!dev) return XCAST_ERR_INVALID_ARGUMENT;

		switch (frame->media_source) {
		case XCastMediaSource_Camera:
		case XCastMediaSource_Screen_Capture: {
			/* 摄像头预览数据渲染 */
			int32_t         width = frame->width;
			int32_t         height = frame->height;
			XCastMediaFormat format = frame->media_format;
			if (format == XCastMedia_argb32) 
			{
				TrackVideoBuffer *buffer = GetTrackBuffer(dev, width, height);
				memcpy(buffer->data, frame->data, frame->size);
				InvalidVideoView(&buffer->rcOut);
			}
			break;
		}
		case xc_device_mic:
			break;
		default:
			break;
		}
	}

	return XCAST_OK;
}

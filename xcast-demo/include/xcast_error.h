/* 
** Copyright (c) 2014 The xcast project. All rights reserved.
*/
#ifndef XCAST_ERROR_H_
#define XCAST_ERROR_H_

#include "xcast_base.h"

#define XCAST_ERROR(label, value) XCAST_ERR_##label = value,

typedef enum xcast_error_e {
  XCAST_OK                  = 0,

  /* A generic failure occurred. */
  XCAST_ERROR(FAILED, -1)
  
  XCAST_ERROR(CONFIG_NOT_FOUND, -3)

  /* 签名超时 */
  XCAST_ERROR(KEY_EXPIRED, -8)

  /* 找不到房间 */
  XCAST_ERROR(ROOM_NOT_FOUND, -10)

  /* 找不到用户 */
  XCAST_ERROR(ENDPOINT_NOT_FOUND, -11)

  /* 签名无效 */
  XCAST_ERROR(KEY_INVALID, -12)

  /* 远程用户关闭 */
  XCAST_ERROR(ENDPOINT_CLOSE, -13)

  /* 成员列表不需要更新 */
  XCAST_ERROR(ENDPOINT_LIST_UPTODATE, -15)

  XCAST_ERROR(PROTO_FAILED, -50)

  /* An asynchronous operation is not yet complete.  This usually does not
  indicate a fatal error.  Typically this error will be generated as a
  notification to wait for some external notification that the operation
  finally completed. */
  XCAST_ERROR(PENDING, -100)

  /* An operation is already in progress. */
  XCAST_ERROR(BUSY, -101)

  /* An object is already exists. */
  XCAST_ERROR(ALREADY_EXISTS, -103)

  /* An argument to the function is incorrect. */
  XCAST_ERROR(INVALID_ARGUMENT, -104)

  /* The handle or file descriptor is invalid. */
  XCAST_ERROR(INVALID_HANDLE, -105)

  /* An object cannot be found. */
  XCAST_ERROR(NOT_FOUND, -106)

  /* An operation timed out. */
  XCAST_ERROR(TIMED_OUT, -107)

  /* The operation failed because of context not started. */
  XCAST_ERROR(CONTEXT_NOT_STARTED, -110)

  /* The operation failed because of unimplemented functionality. */
  XCAST_ERROR(NOT_IMPLEMENTED, -111)

  /* There were not enough resources to complete the operation. */
  XCAST_ERROR(INSUFFICIENT_RESOURCES, -112)

  /* Memory allocation failed. */
  XCAST_ERROR(OUT_OF_MEMORY, -113)

  /* Operation not supported. */
  XCAST_ERROR(NOT_SUPPORTED, -114)

  /* 群语音、群视频被禁用，或者用户被禁言. */
  XCAST_ERROR(ENTER_BAN, -115)

  /* 群视频抢座失败，因为群视频座位已满. */
  XCAST_ERROR(ENTER_VIDEO_FULL, -116)

  /* Get interface server address failed */
  XCAST_ERROR(INTERFACE_SERVER_NOT_EXIST, -117)

  /* A previous operation is already in progress */
  XCAST_ERROR(OPERATION_IN_PROGRESS, -118)

  /* Not a valid room */
  XCAST_ERROR(INVALID_ROOM, -119)

  /* Init opensdk fail */
  XCAST_ERROR(INITSDKFAIL, -120)

  /* Server kicked out as another client connected. */
  XCAST_ERROR(SERVER_KICK_OUT, -150)

  /* The connection closed. */
  XCAST_ERROR(CONNECTION_CLOSED, -151)

  /* Failed to connect. */
  XCAST_ERROR(CONNECTION_FAILED, -152)

  /* App server failed to connect. */
  XCAST_ERROR(APP_SERVER_FAILED, -153)

  /* An error occured in device start. */
  XCAST_ERROR(DEVICE_START_FAILED, -200)
  
  XCAST_ERROR(PERMISSION_DENIED, -201)

} xcast_error;


#endif /* XCAST_ERROR_H_ */

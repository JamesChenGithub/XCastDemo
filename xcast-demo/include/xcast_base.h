/*
** Copyright (c) 2014-2017 The xcast project. All rights reserved.
*/
#ifndef XCAST_BASIC_TYPES_H_
#define XCAST_BASIC_TYPES_H_

#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#if defined(ANDROID)
#define OS_ANDROID
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if defined(TARGET_OS_OSX)
#define OS_OSX
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define OS_IOS
#endif
#elif defined(__linux__)
#define OS_LINUX
#include <unistd.h>
#if defined(__GLIBC__) && !defined(__UCLIBC__)
#define LIBC_GLIBC
#endif
#elif defined (_WINDOWS) || defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
#define OS_WIN
#define HAVE_STRUCT_TIMESPEC
#else
#error Please add support for your platform in build_config.h
#endif

#if defined(OS_ANDROID)
#define OS_POSIX
#endif

#if defined(OS_IOS) || defined(OS_OSX)
#define OS_POSIX
#endif

#if defined(OS_WIN)
#include <Windows.h>
#endif

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(XCAST_IMPLEMENTATION)
#define xcast_export __declspec(dllexport)
#else
#define xcast_export __declspec(dllimport)
#endif /* defined(XCAST_IMPLEMENTATION) */

#else /* defined(WIN32) */
#if defined(XCAST_IMPLEMENTATION)
#define xcast_export __attribute__((visibility("default")))
#else
#define xcast_export
#endif /* defined(XCAST_IMPLEMENTATION) */
#endif

#else /* defined(COMPONENT_BUILD) */
#define xcast_export
#endif

#ifndef xcast_inline
#if defined(__GNUC__)
#define xcast_inline __inline__
#elif defined(_MSC_VER) || defined(__BORLANDC__) || \
      defined(__DMC__) || defined(__SC__) || \
      defined(__WATCOMC__) || defined(__LCC__) || \
      defined(__DECC)
#define xcast_inline __inline
#ifndef __inline__
#define __inline__ __inline
#endif
#else
#define xcast_inline inline
#ifndef __inline__
#define __inline__ inline
#endif
#endif
#endif /* xcast_inline not defined */

#ifndef xcast_force_inline 
#if defined(_MSC_VER)
#define xcast_force_inline __forceinline
#elif ( (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__) )
#define xcast_force_inline __attribute__((always_inline)) static __inline__
#else
#define xcast_force_inline static xcast_inline
#endif
#endif /* xcast_force_inline not defined */

#ifndef NULL
#  ifdef __cplusplus
#  define NULL        (0L)
#  else /* !__cplusplus */
#  define NULL        ((void*) 0)
#  endif /* !__cplusplus */
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

#ifndef ssize_t
#define ssize_t int32_t
#endif

#undef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef  ABS
#define ABS(a)     (((a) < 0) ? -(a) : (a))

#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t(*xcast_func_pt)(void *user_data, void *data);

typedef struct xc_variant_s xcast_variant_t;

#ifdef __cplusplus
}
#endif


#endif /* XCAST_BASIC_TYPES_H_ */

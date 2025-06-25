#ifndef __khrplatform_h_
#define __khrplatform_h_

#if defined(__SCITECH_SNAP__) && !defined(KHRONOS_STATIC)
#   define KHRONOS_STATIC 1
#endif

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   if defined(KHRONOS_STATIC)
#       define KHRONOS_APICALL
#       define KHRONOS_APIENTRY
#   elif defined(_WINDLL)
#       define KHRONOS_APICALL __declspec(dllexport)
#   else
#       define KHRONOS_APICALL __declspec(dllimport)
#   endif
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APICALL
#   define KHRONOS_APIENTRY
#endif

#ifndef KHRONOS_APIATTRIBUTES
#define KHRONOS_APIATTRIBUTES
#endif

#if defined(_WIN64) || defined(__LP64__)
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
typedef int8_t                 khronos_int8_t;
typedef uint8_t                khronos_uint8_t;
typedef int16_t                khronos_int16_t;
typedef uint16_t               khronos_uint16_t;
typedef int32_t                khronos_int32_t;
typedef uint32_t               khronos_uint32_t;
typedef int64_t                khronos_int64_t;
typedef uint64_t               khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif defined(__VMS ) || defined(__sgi)
#include <inttypes.h>
typedef int8_t                 khronos_int8_t;
typedef uint8_t                khronos_uint8_t;
typedef int16_t                khronos_int16_t;
typedef uint16_t               khronos_uint16_t;
typedef int32_t                khronos_int32_t;
typedef uint32_t               khronos_uint32_t;
typedef int64_t                khronos_int64_t;
typedef uint64_t               khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif defined(_WIN32) && !defined(__SCITECH_SNAP__)
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;
typedef signed   long  int     khronos_int32_t;
typedef unsigned long  int     khronos_uint32_t;
typedef signed   __int64       khronos_int64_t;
typedef unsigned __int64       khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif defined(__sun__) || defined(__digital__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__) || defined(_LP64)
typedef long int               khronos_intptr_t;
typedef unsigned long int      khronos_uintptr_t;
#else
typedef int                    khronos_intptr_t;
typedef unsigned int           khronos_uintptr_t;
#endif
#endif
typedef int8_t                 khronos_int8_t;
typedef uint8_t                khronos_uint8_t;
typedef int16_t                khronos_int16_t;
typedef uint16_t               khronos_uint16_t;
typedef int32_t                khronos_int32_t;
typedef uint32_t               khronos_uint32_t;
typedef int64_t                khronos_int64_t;
typedef uint64_t               khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif 0
#else
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;
typedef signed   long  int     khronos_int32_t;
typedef unsigned long  int     khronos_uint32_t;
typedef signed   long long int khronos_int64_t;
typedef unsigned long long int khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#endif

typedef float                  khronos_float_t;
typedef khronos_uint32_t       khronos_boolean_enum_t;

enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = 0x7FFFFFFF
};

#endif
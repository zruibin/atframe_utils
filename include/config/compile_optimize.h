// Copyright 2022 atframework

#pragma once

// ================ branch prediction information ================
#if !defined(UTIL_LIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define UTIL_LIKELY_IF(...) if (__VA_ARGS__) [[likely]]
#    endif
#  endif
#endif
#if !defined(UTIL_LIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define UTIL_LIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), true))
#endif
#ifndef UTIL_LIKELY_IF
#  define UTIL_LIKELY_IF(...) if (__VA_ARGS__)
#endif

#if !defined(UTIL_UNLIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define COPP_UNLIKELY_IF(...) if (__VA_ARGS__) [[unlikely]]
#    endif
#  endif
#endif
#if !defined(UTIL_UNLIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define UTIL_UNLIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), false))
#endif
#ifndef UTIL_UNLIKELY_IF
#  define UTIL_UNLIKELY_IF(...) if (__VA_ARGS__)
#endif

// ---------------- branch prediction information ----------------

// ================ import/export ================
// @see https://gcc.gnu.org/wiki/Visibility
// @see http://releases.llvm.org/9.0.0/tools/clang/docs/AttributeReference.html
// 不支持 borland/sunpro_cc/xlcpp

// ================ import/export: for compilers ================
#if defined(__GNUC__) && !defined(__ibmxl__)
//  GNU C++/Clang
//
// Dynamic shared object (DSO) and dynamic-link library (DLL) support
//
#  if __GNUC__ >= 4
#    if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
// All Win32 development environments, including 64-bit Windows and MinGW, define
// _WIN32 or one of its variant spellings. Note that Cygwin is a POSIX environment,
// so does not define _WIN32 or its variants.
#      ifndef UTIL_SYMBOL_EXPORT
#        define UTIL_SYMBOL_EXPORT __attribute__((__dllexport__))
#      endif
#      ifndef UTIL_SYMBOL_IMPORT
#        define UTIL_SYMBOL_IMPORT __attribute__((__dllimport__))
#      endif

#    else

#      ifndef UTIL_SYMBOL_EXPORT
#        define UTIL_SYMBOL_EXPORT __attribute__((visibility("default")))
#      endif
#      ifndef UTIL_SYMBOL_IMPORT
#        define UTIL_SYMBOL_IMPORT __attribute__((visibility("default")))
#      endif
#      ifndef UTIL_SYMBOL_VISIBLE
#        define UTIL_SYMBOL_VISIBLE __attribute__((visibility("default")))
#      endif
#      ifndef UTIL_SYMBOL_LOCAL
#        define UTIL_SYMBOL_LOCAL __attribute__((visibility("hidden")))
#      endif

#    endif

#  else
// config/platform/win32.hpp will define UTIL_SYMBOL_EXPORT, etc., unless already defined
#    ifndef UTIL_SYMBOL_EXPORT
#      define UTIL_SYMBOL_EXPORT
#    endif

#    ifndef UTIL_SYMBOL_IMPORT
#      define UTIL_SYMBOL_IMPORT
#    endif
#    ifndef UTIL_SYMBOL_VISIBLE
#      define UTIL_SYMBOL_VISIBLE
#    endif
#    ifndef UTIL_SYMBOL_LOCAL
#      define UTIL_SYMBOL_LOCAL
#    endif

#  endif

#elif defined(_MSC_VER)
//  Microsoft Visual C++
//
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#else
#endif
// ---------------- import/export: for compilers ----------------

// ================ import/export: for platform ================
//  Default defines for UTIL_SYMBOL_EXPORT and UTIL_SYMBOL_IMPORT
//  If a compiler doesn't support __declspec(dllexport)/__declspec(dllimport),
//  its boost/config/compiler/ file must define UTIL_SYMBOL_EXPORT and
//  UTIL_SYMBOL_IMPORT
#if !defined(UTIL_SYMBOL_EXPORT) && (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))

#  ifndef UTIL_SYMBOL_EXPORT
#    define UTIL_SYMBOL_EXPORT __declspec(dllexport)
#  endif
#  ifndef UTIL_SYMBOL_IMPORT
#    define UTIL_SYMBOL_IMPORT __declspec(dllimport)
#  endif
#endif
// ---------------- import/export: for platform ----------------

#ifndef UTIL_SYMBOL_EXPORT
#  define UTIL_SYMBOL_EXPORT
#endif
#ifndef UTIL_SYMBOL_IMPORT
#  define UTIL_SYMBOL_IMPORT
#endif
#ifndef UTIL_SYMBOL_VISIBLE
#  define UTIL_SYMBOL_VISIBLE
#endif
#ifndef UTIL_SYMBOL_LOCAL
#  define UTIL_SYMBOL_LOCAL
#endif
#ifndef UTIL_SYMBOL_NONE
#  define UTIL_SYMBOL_NONE
#endif

// ---------------- import/export ----------------

// ================ __cdecl ================
#if defined(__GNUC__) || defined(__GNUG__)
#  ifndef __cdecl
// see https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Function-Attributes.html
// Intel x86 architecture specific calling conventions
#    ifdef _M_IX86
#      define __cdecl __attribute__((__cdecl__))
#    else
#      define __cdecl
#    endif
#  endif
#endif
// ---------------- __cdecl ----------------

// ================ always inline ================

#ifndef UTIL_FORCEINLINE
#  if defined(__clang__)
#    if __cplusplus >= 201103L
#      define UTIL_FORCEINLINE [[gnu::always_inline]] inline
#    else
#      define UTIL_FORCEINLINE __attribute__((always_inline)) inline
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
#    if __cplusplus >= 201103L && (__GNUC__ * 100 + __GNUC_MINOR__) >= 408
#      define UTIL_FORCEINLINE [[gnu::always_inline]] inline
#    else
#      define UTIL_FORCEINLINE __attribute__((always_inline)) inline
#    endif
#  elif defined(_MSC_VER)
#    define UTIL_FORCEINLINE __forceinline
#  else
#    define UTIL_FORCEINLINE inline
#  endif
#endif

#ifndef UTIL_NOINLINE_NOCLONE
#  if defined(__clang__)
#    if __cplusplus >= 201103L
#      define UTIL_NOINLINE_NOCLONE [[gnu::noinline]]
#    else
#      define UTIL_NOINLINE_NOCLONE __attribute__((noinline))
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
#    if __cplusplus >= 201103L && (__GNUC__ * 100 + __GNUC_MINOR__) >= 408
#      define UTIL_NOINLINE_NOCLONE [[gnu::noinline, gnu::noclone]]
#    else
#      define UTIL_NOINLINE_NOCLONE __attribute__((noinline, noclone))
#    endif
#  elif defined(_MSC_VER)
#    define UTIL_NOINLINE_NOCLONE __declspec(noinline)
#  else
#    define UTIL_NOINLINE_NOCLONE
#  endif
#endif

#ifndef UTIL_HAVE_CPP_ATTRIBUTE
#  if defined(__cplusplus) && defined(__has_cpp_attribute)
// NOTE: requiring __cplusplus above should not be necessary, but
// works around https://bugs.llvm.org/show_bug.cgi?id=23435.
#    define UTIL_HAVE_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#  else
#    define UTIL_HAVE_CPP_ATTRIBUTE(x) 0
#  endif
#endif

#ifndef UTIL_CONST_INIT
#  if defined(__cpp_constinit) && __cpp_constinit >= 201907L
#    if defined(_MSC_VER)
#      define UTIL_CONST_INIT
#    else
#      define UTIL_CONST_INIT constinit
#    endif
#  elif UTIL_HAVE_CPP_ATTRIBUTE(clang::require_constant_initialization)
#    define UTIL_CONST_INIT [[clang::require_constant_initialization]]
#  else
#    define UTIL_CONST_INIT
#  endif
#endif

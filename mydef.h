
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYDEF_H__
#define __MYDEF_H__


#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>

/*compiler*/

#if defined(_MSC_VER) 
#define MY_COMPILER_MSVC 1
#define MY_COMPILER_NAME "MSVC"
#elif defined(__GNUC__)||defined(__GNUG__)
#define MY_COMPILER_GCC 1
#define MY_COMPILER_NAME "GCC"
#elif defined(__clang__)
#define MY_COMPILER_CLANG 1
#define MY_COMPILER_NAME "Clang"
#else
#define MY_COMPILER_UNKNOWN 1
#define MY_COMPILER_NAME "Unknown"
#endif


/*OS*/
#if defined(_WIN32) || defined(_WIN64)  || defined(__CYGWIN__)
#define MY_OS_WINDOWS 1
#define MY_OS_WIN MY_OS_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
#define MY_OS_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
#define MY_OS_MAC 1
#else
#define MY_OS_UNKNOWN 1
#endif

/*CPU Architecture*/
#if defined(__x86_64__) || defined(_M_X64)
#define MY_ARCH_X64 1
#elif defined(__i386__) || defined(_M_IX86)
#define MY_ARCH_X86 1
#elif defined(__arm__) || defined(_M_ARM)
#define MY_ARCH_ARM 1
#endif


/*def by compiler*/
#if MY_COMPILER_MSVC
#define INLINE __inline
#define RESTRICT __restrict  // for MSVC
#define MY_ALIGN(n) __declspec(align(n))

#else
#define INLINE inline
#define RESTRICT __restrict  // for GCC/Clang	
#define MY_ALIGN(n) __attribute__((aligned(n)))

#endif



#if MY_COMPILER_MSVC || MY_COMPILER_GCC
#elif 
#define RESTRICT restrict
#endif

#ifdef __cplusplus 
	#define MY_EXTERN_START extern "C" {
	#define MY_EXTERN_END }
#else 
	#define MY_EXTERN_START 
	#define MY_EXTERN_END
#endif



#ifndef max
#define max(A,B) ((A>B)?(A):(B))
#endif

#ifndef min
#define min(A,B) ((A>B)?(B):(A))
#endif



#if MY_OS_WIN
#include <Windows.h>
#elif MY_OS_LINUX
#include <sys/mman.h>
#include <sys/time.h>
#endif

typedef uint32_t mysize_t;
typedef int64_t myclock_t;



#endif //__MYDEF_H__
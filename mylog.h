
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYLOG_H__
#define __MYLOG_H__

#include "mydef.h"


#define MY_LOG_LEVEL_OFF 0
#define MY_LOG_LEVEL_ERROR 1
#define MY_LOG_LEVEL_INFO 2

#ifndef MY_LOG_LEVEL
#ifdef NDEBUG
#define MY_LOG_LEVEL MY_LOG_LEVEL_NONE
#else
#define MY_LOG_LEVEL MY_LOG_LEVEL_INFO
#endif
#endif

#if defined(MY_COMPILER_MSVC)
#define MY_BREAK() __debugbreak()
#elif defined(MY_COMPILER_GCC) || defined(MY_COMPILER_CLANG)
#define MY_BREAK() __builtin_trap()
#else
#define MY_BREAK() ((void)0)
#endif

#if MY_LOG_LEVEL >= MY_LOG_LEVEL_INFO
#define MY_LOG_INFO(fmt, ...)  fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#else
#define MY_LOG_INFO(fmt, ...)  ((void)0)
#endif

#ifdef NDEBUG
	#define MY_ASSERT(cond) ((void)0)
#else
	#define	MY_ASSERT(cond) \
		do{\
			if(!(cond)){\
				fprintf(stderr, "[ASSERT FAILED] %s:line %d: %s\n", __FILE__, __LINE__, #cond); \
				MY_BREAK();\
				abort();\
			}\
		}while(0)
#endif

#endif
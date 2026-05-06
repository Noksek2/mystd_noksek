
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYSTD_H__
#define __MYSTD_H__

#include "mydef.h"
#include "mylog.h"
#include "myallocator.h"


#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 2
#define MYSTD_VERSION_PATCH 0
#define MYSTD_VERSION (MYSTD_VERSION_MAJOR*10000\
    +MYSTD_VERSION_MINOR*100 \
    +MYSTD_VERSION_PATCH)

MY_EXTERN_START
uint8_t g_PM_id;

extern void  mystd_init();
extern void  mystd_destroy();
//extern void* mystd_alloc(mysize_t len, mysize_t ms);
//extern void* mystd_alloc_id(uint8_t core_id, mysize_t len, mysize_t ms);
//extern void  mystd_free(void* ptr);
extern void  mystd_print_system_info();

MY_EXTERN_END

#endif
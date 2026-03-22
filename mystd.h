
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


MY_EXTERN_START
extern void mystd_print_system_info();
#if MY_OS_WINDOWS
static myclock_t myclock_setclock() {
    myclock_t timer;
    QueryPerformanceFrequency((LARGE_INTEGER*)&timer);
    return timer;
}
/*static myclock_t myclock_getclock() {
    myclock_t clock;
    QueryPerformanceCounter(&clock);
    return clock;
}*/
static double myclock_getsec(myclock_t timer) {
    myclock_t clock;
    QueryPerformanceCounter((LARGE_INTEGER*)&clock);
    return (double)clock / (double)timer;
    
}
static myclock_t _myclock_getnanosec(myclock_t timer) {
    myclock_t clock;
    QueryPerformanceCounter((LARGE_INTEGER*) & clock);
    return (myclock_t)(((double)clock / (double)timer) * 1000000000.0);

    //return (double)clock / (double)timer;
}
#elif MY_OS_LINUX
static myclock_t myclock_setclock() {
    return 0;
}
static double myclock_getsec(myclock_t timer) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    //return (double)clock / (double)timer;
}
static int _myclock_getnanosec(myclock_t timer) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec;
    //return (double)clock / (double)timer;
}
#endif
MY_EXTERN_END

#endif
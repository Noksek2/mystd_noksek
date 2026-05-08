
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

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif


#ifdef __cplusplus
#define r_cast(T,DAT) reinterpret_cast<T>(DAT)
#define d_cast(T,DAT) dynamic_cast<T>(DAT)
#define s_cast(T,DAT) static_cast<T>(DAT)
#define c_cast(T,DAT) const_cast<T>(DAT)
#else
#define r_cast(T,DAT) ((T)(DAT))
#define d_cast(T,DAT) ((T)(DAT))
#define s_cast(T,DAT) ((T)(DAT))
#define c_cast(T,DAT) ((T)(DAT))
#endif

#ifndef false
#define false 0
#endif



#if MY_OS_WIN
#include <Windows.h>
#elif MY_OS_LINUX
#include <pthread.h>
#include <sys/mman.h>
#include <sys/time.h>
#endif

typedef uint32_t res_t;
typedef uint32_t mysize_t;
typedef int64_t myclock_t;


enum {
    _8B = 8,
    _16B = 16,
    _32B = 32,
    _64B = 64,
    _128B = 128,
    _256B = 256,
    _512B = 512,

    _1KB = (1024),
    _2KB = _1KB * 2,
    _4KB = _1KB * 4,
    _8KB = _1KB * 8,
    _16KB = _1KB * 16,
    _32KB = _1KB * 32,
    _64KB = _1KB * 64,
    _128KB = _1KB * 128,
    _256KB = _1KB * 256,
    _512KB = _1KB * 512,

    _1MB = (1024 * 1024),
    _2MB = _1MB * 2,
    _4MB = _1MB * 4,
    _8MB = _1MB * 8,
    _16MB = _1MB * 16,
    _32MB = _1MB * 32,
    _64MB = _1MB * 64,
    _128MB = _1MB * 128,
};

// 선언pthread_mutex_t lock;뮤텍스 객체 변수 선언초기화
// PTHREAD_MUTEX_INITIALIZER정적 초기화 (전역/정적 변수용)
// pthread_mutex_init()동적 초기화 (속성 지정 가능)잠금 (Lock)
// pthread_mutex_lock()임계 구역 진입 전 호출 (대기 발생 가능)해제 
// (Unlock)pthread_mutex_unlock()임계 구역 종료 후 호출제거
// pthread_mutex_destroy()사용이 끝난 뮤텍스 자원 반납
// 
// 
// 
// CRITICAL_SECTION cs;
// int sharedResource = 0;
// 
// // Initialize
// InitializeCriticalSection(&cs);
// 
// // In thread function
// EnterCriticalSection(&cs);
// // --- Critical Section Start ---
// sharedResource++; // Safe access
// // --- Critical Section End ---
// LeaveCriticalSection(&cs);
// 
// // Clean up
// DeleteCriticalSection(&cs);
// 
// #include <pthread.h>
// pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&lock);
// pthread_mutex_unlock(&lock);

MY_EXTERN_START

#if MY_OS_WINDOWS
typedef CRITICAL_SECTION mymutex;

static void mymutex_init(mymutex* mut) {
    InitializeCriticalSection(mut);
}
static void mymutex_destroy(mymutex* mut) {
    DeleteCriticalSection(mut);
}
static void mymutex_lock(mymutex* mut) {
    EnterCriticalSection(mut);
}
static void mymutex_unlock(mymutex* mut) {
    LeaveCriticalSection(mut);
}
#elif MY_OS_LINUX
typedef struct mymutex {
    pthread_mutex_t mut;
    pthread_mutexattr_t attr;
} mymutex;
static void mymutex_init(mymutex* mut) {
    pthread_mutexattr_init(&mut->attr);
    pthread_mutexattr_settype(&mut->attr, PTHREAD_MUTEX_RECURSIVE);
    // 3. 설정된 속성으로 뮤텍스 초기화
    pthread_mutex_init(&mut->mut, &mut->attr);

   // mut->mut = PTHREAD_MUTEX_INITIALIZER;
    //pthread_mutex_lock(mut);
}
static void mymutex_destroy(mymutex* mut) {
    pthread_mutex_destroy(&mut->mut);
    pthread_mutexattr_destroy(&mut->attr);
}
static void mymutex_lock(mymutex* mut) {
    pthread_mutex_lock(&mut->mut);
}
static void mymutex_unlock(mymutex* mut) {
    pthread_mutex_unlock(&mut->mut);
}
#endif

#if MY_COMPILER_MSVC
static uint32_t find_msb32_idx(uint32_t mask) {
    DWORD index;
    return _BitScanReverse(&index, (DWORD)mask) ? (uint32_t)index : 0u;
}
static uint32_t find_msb64_idx(uint64_t mask) {
    DWORD index;
    return _BitScanReverse64(&index, (DWORD64)mask) ? (uint32_t)index : 0u;
}
#else 
static uint32_t find_msb32_idx(uint32_t mask) {
    if (mask == 0) return 0;
    return 31 - __builtin_clz(mask);
}
#endif
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
    QueryPerformanceCounter((LARGE_INTEGER*)&clock);
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

#endif //__MYDEF_H__
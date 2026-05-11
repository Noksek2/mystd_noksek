
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYALLOCATOR_H__
#define __MYALLOCATOR_H__

#include "mylog.h"

#define ARENA_1KB (1024-sizeof(mypage))
#define ARENA_4KB (1024*4-sizeof(mypage))
#define ARENA_64KB (1024*64-sizeof(mypage))

#define ARENA_1MB (1024*1024-sizeof(mypage))
#define ARENA_4MB (1024*1024*4-sizeof(mypage))
#define ARENA_64MB (1024*1024*64-sizeof(mypage))

#define ARENA_ALLOC(ALC, T, SZ) (T*)arena_alloc(ALC,sizeof(T)*(SZ))
#define GET_PADDING(LEN) (((max(1,(LEN)) - 1) / sizeof(uint64_t) + 1) * sizeof(uint64_t))

//#ifdef POOL_UNSAFE

enum {
    PAGE_HOLE_DEF = 1,
    PAGE_TAG_EMPTY = 0b00,
    PAGE_TAG_HOLE = 0b01,
    PAGE_TAG_FULL = 0b10,
    PAGE_TAG_USING = 0b11,

    PAGE_SIZE = _4KB,
    SLOWPOOL_PAGE_SIZE = _16KB,
    CACHE_SIZE_DEF = _256KB,
    MYCORE_MAX=8
};

static const mysize_t _1GB = _1MB * 1024;
/*

#include <windows.h>
#include <stdio.h>

int main() {
    SIZE_T size = 1024 * 1024; // 1MB

    // 1. Reserve: 주소 공간만 1MB 확보 (물리 메모리 점유 X)
    void* pReserved = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    if (!pReserved) return 1;

    // 2. Commit: 실제로 쓸 공간만큼 물리 메모리 할당 (여기서는 전체 1MB)
    void* pCommitted = VirtualAlloc(pReserved, size, MEM_COMMIT, PAGE_READWRITE);

    // 3. Protection 변경: 읽기 전용으로 변경해보기 (PAGE_READWRITE -> PAGE_READONLY)
    DWORD oldProtect;
    if (VirtualProtect(pCommitted, size, PAGE_READONLY, &oldProtect)) {
        printf("보호 모드 변경 성공: Read-Only\n");
        // *pCommitted = 'A'; // 만약 여기서 쓰기를 시도하면 Access Violation 발생!
    }

    // 4. Release: 주소 공간 통째로 반납
    // 주의: MEM_RELEASE는 반드시 size를 0으로 주고, Reserve된 시작 주소를 넘겨야 함
    VirtualFree(pReserved, 0, MEM_RELEASE);

    return 0;
}

#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    size_t size = 1024 * 1024; // 1MB

    // 1. Reserve: PROT_NONE(접근 권한 없음)으로 설정하여 주소 공간만 확보
    void* p = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) return 1;

    // 2. Commit: mprotect를 사용하여 권한을 부여 (실제 물리 메모리 매핑 유도)
    // 리눅스는 이 시점에 실제 접근(Write)이 일어날 때 페이지를 할당함 (Lazy Allocation)
    mprotect(p, size, PROT_READ | PROT_WRITE);

    // 3. Protection 변경: 읽기 전용으로 (PROT_READ | PROT_WRITE -> PROT_READ)
    mprotect(p, size, PROT_READ);

    // 4. Release: 메모리 해제
    munmap(p, size);

    return 0;
}
*/

MY_EXTERN_START
#if MY_OS_WINDOWS
static void* mymem_reserve(mysize_t capa) {
    //capa = ((capa - 1) / (_1KB * 64) + 1) * (_1KB * 64);
    malloc(capa);
    
    void* mem = VirtualAlloc(
        NULL,
        capa,
        MEM_RESERVE,
        PAGE_NOACCESS
    );
    MY_ASSERT(mem != NULL);
    return mem;
}
static void* mymem_commit(void* mem, mysize_t capa) {
    mem = VirtualAlloc(
        mem,
        capa,
        MEM_COMMIT,
        PAGE_READWRITE
    );
    //MY_ASSERT(mem != NULL);
    return mem;
}
static void* mymem_rescommit(mysize_t capa){
    void* mem = mymem_reserve(capa);
    MY_ASSERT(mymem_commit(mem, capa) != NULL);
    return mem;
}
static void mymem_release(void* mem, mysize_t capa) {
    VirtualFree(mem, 0, MEM_RELEASE);
}
#elif MY_OS_LINUX
static void* mymem_reserve(mysize_t capa) {
    const mysize_t sz = capa + PAGE_SIZE;
    //capa = ((capa - 1) / (_1KB * 64) + 1) * (_1KB * 64);
    void* mem = mmap(NULL, sz, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    MY_ASSERT(mem != MAP_FAILED);
    
    const uintptr_t alignment = PAGE_SIZE;
    uintptr_t addr = (uintptr_t)mem;
    uintptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    void* aligned_ptr = (void*)aligned_addr;

    // 3. 앞쪽 자투리 반납
    uintptr_t prefix_gap = aligned_addr - addr;
    uintptr_t suffix_gap = alignment - prefix_gap;
    if (prefix_gap > 0u) {
        munmap(mem, prefix_gap);
        munmap((char*)aligned_ptr + capa, suffix_gap);
    }
    return aligned_ptr;
}
static void* mymem_commit(void* mem, mysize_t capa) {
    mprotect(mem, capa, PROT_READ | PROT_WRITE);
    return mem;
}
static void mymem_release(void* mem, mysize_t capa) {
    munmap(mem, capa);
}
#else
#endif

#ifdef _DEBUG
extern size_t g_id;
#endif

//using for pool and arena
typedef struct mypage { 
    struct mypage* next;
#ifdef _DEBUG
    size_t id;
#endif
    mysize_t len; //current used page
    mysize_t capa;//page size
    uint8_t ptr[];
}mypage;

// typedef struct {
//     mypage* blocklist;
//     mypage* freelist;
//     mysize_t allocsize;//now memory
//     uint32_t defaultsize;//pow 2 basic:64KB
//     uint32_t threshold;//pow 2 1<<32=4GB
// } myglobalmemory;

typedef struct myarena_check {
    mypage* tail;
    mypage* head;
    mysize_t len;
}myarena_check;
typedef struct myarena {
    mypage* head;
    mypage* current;
    mysize_t arena_size;
}myarena;

typedef enum {
    POOL_8B,
    POOL_16B, POOL_24B,
    POOL_32B, POOL_48B,
    POOL_64B, POOL_96B,
    POOL_128B, POOL_192B,
    POOL_256B, POOL_384B,
    POOL_512B, POOL_768B,
    POOL_1KB, 
    POOL_SIZE_MAX,
    POOL_4KB, 
    POOL_8KB,
   


    SLOWPOOL_8B,
    SLOWPOOL_16B,
    SLOWPOOL_32B,
    SLOWPOOL_64B, 
    SLOWPOOL_128B, 
    SLOWPOOL_256B,
    SLOWPOOL_512B,
    SLOWPOOL_1KB,
    SLOWPOOL_2KB,
    SLOWPOOL_4KB,
    SLOWPOOL_8KB,
    SLOWPOOL_SIZE_MAX,
    
} ;

static const mysize_t g_poolsize_map16KB[POOL_SIZE_MAX]
= { 8u, 16u, 32u, 64u, 128u,
    256u,504u,1016u,2040u,4080u,
    8176u };

static const mysize_t g_poolpage_cnt16KB[POOL_SIZE_MAX]
= { 2013u, 1014u, 509u, 255u, 127u,
    63u, 32u, 16u, 8u, 4u,
    2u };

static const mysize_t g_poolsize_map32KB[POOL_SIZE_MAX]
= { 8u, 16u, 32u, 64u, 128u,
    256u,504u,1016u,2040u,4080u,
    8184u };

static const mysize_t g_poolpage_cnt32KB[POOL_SIZE_MAX]
= { 4029u, 2030u, 1019u, 510u, 255u,
    127u, 64u, 32u, 16u, 8u,
    4u };

#define g_poolsize_map g_poolsize_map32KB 
#define g_poolpage_cnt g_poolpage_cnt32KB 

//typedef struct mypooltag {
//	struct mypooltag* next;
//} mypooltag;
struct mysizepoolmanager;
struct mysizepool;

typedef struct mysizepoolmanager mysizepoolmanager;
typedef struct mysizepool mysizepool;
typedef struct myfreepool {
    union {
        uint32_t nextoff;
    };
    //struct mypooltag* next;
} myfreepool;

typedef struct mypoolpage { 
    //union {
    //	struct mypoolpage* next; //8
    //	uintptr_t tag;
    //};
    //struct mypooltag* freelist; //8
    
    //union{
    //	
    //}
    union {
        struct mysizepool* pSP;
        struct mysizepoolmanager* pSM;
    };
    uint32_t freeoff;//PAGE +offset
    uint32_t nextpageoff; //CACHE + off
    uint16_t elem_total; //
    uint16_t elem_len;
    uint16_t elem_empty;
    uint8_t memsz_idx;//8<<memsz
    uint8_t tag;
    //uint8_t core_id;
    uint8_t ptr[];
} mypoolpage;//PG
typedef struct myfreelist {
    uint32_t nextoff;
    //struct myfreelist* next;
} myfreelist;
struct mysizepool {
    struct mysizepoolmanager* pSM;
    uintptr_t cachemem;
    mypoolpage* page_curr;
    //mypoolpage* page_rest;
    //mypoolpage* page_tail;
    //mypoolpage* page_full;
    // 
    uint32_t page_hole_off;//cachemem + off;
    uint32_t page_cnt;
    uint32_t page_size_def;//MAX 64KB-1
    //mypoolpage* page_hole;
} ;//SP
typedef struct myfreepage {
    struct myfreepage* next;
} myfreepage;
struct mysizepoolmanager {
    union {
        //400B
        struct {
            struct mysizepool szpool[POOL_SIZE_MAX];
            uint8_t core_id;
            mysize_t threshold;
            myfreepage* freepage;
            void* cachemem;
            mysize_t cachesize;
        };
        uint8_t _[64 * 7];
    };
} ;//SM

typedef struct {
    mysizepoolmanager core[MYCORE_MAX];
    uint8_t cnt_bits;
} mypoolmanager; //PM

//64B 배수로
// nextlist, nextpage는 실제로는 4바이트 이하로 압축이 가능함. 
// 단 포인터 접근을 빠르게 하려는 목적임.
typedef struct mypagehead {
    struct mypagehead* nextPH;
    struct mypagehead* prevPH;
    uint32_t elem_empty;//4B
    uint32_t elem_total;//4B
    
    uint32_t elem_cnt;//4B
    uint32_t nextoff;//8의 배수. elemno가 아님.
    uint16_t poolidx; //4B
    uint16_t poolsize; //4B
    uint16_t pagetag; //4B
    uint32_t _[6];//4B*8
} mypagehead;
//head의 구조체는 4KB당 64B를 저장하는 목적. (64배)
//4,194,304KB=4GB -> head는 65,536KB(64MB)
//head가 꽤 큰 편이지만 (double free check) 현재로써는 감안하기로 함.

typedef struct mysizehead {
    struct mypagehead* curr;
    struct mypagehead* hole;
    uint32_t poolsize;
    uint16_t poolidx;
    uint16_t coreID;
    uint64_t _;
} mysizehead;
//4KB
typedef struct mycachehead {
    struct mysizehead SH[POOL_SIZE_MAX];
    struct mysizehead SH_core[MYCORE_MAX][POOL_SIZE_MAX];
    uint8_t _[64];
} mycachehead;

typedef struct mypagedata {
    //union {
        uint8_t data[PAGE_SIZE];
    //};
} mypagedata;

//cachehead(448?) + headmem(~)
//4GB가 한계 (실제로는 더 적음)
// 초기: cachehead(448) + Page_cnt = 57개
// Pagemem = 4KB*57=228KB
// 페이지 추가 : 57 + 64(4096/64)=121
typedef struct mycachemem {
    mycachehead* cachehead; //8
    mypagehead* headmem; //8 
    mypagedata* pagemem; //8
    mypagehead* freepage;
    uint32_t page_cnt; //4
    uint32_t commit_page_cnt; //4
    uint32_t reserved_page_cnt;
    uint8_t _[16];
} mycachemem;

//얘도 가급적 64B

extern mycachemem g_cachemem;
extern mypoolmanager g_poolmng;
extern mymutex g_lock;



extern void* myslowpool_alloc(uint8_t core_id, mysize_t len, mysize_t ms);
extern uint32_t myslowpool_free(void* ptr);
extern void     myslowpool_destroy();
extern uint8_t  myslowpool_new(mysize_t max_threshold);


// this_page == page_rest | page_curr -> SP -> freepage 가능?
// extern void*    mypool_alloc(uint8_t core_id, mysize_t len, mysize_t ms);
// extern uint32_t mypool_free(void* ptr);
// extern void     mypool_destroy();
// extern uint8_t  mypool_new(mysize_t max_threshold);

// globalmem [cachehead {pagecurr[MAXPOOLIDX], pagehole[MAXPOOLIDX]}: B] [PageHead0] [PageHead1] ...
static uint32_t getpoolidx(uint32_t i) {
    if (i <= 8)                return 0;
    else if (i <= 16)          return 1;
    else if (i <= 24)          return 2;
    else if (i <= 32)          return 3;
    else if (i <= 48)          return 4;
    else if (i <= 64)          return 5;
    else if (i <= 96)          return 6;
    else if (i <= 128)         return 7;
    else if (i <= 192)         return 8;
    else if (i <= 256)         return 9;
    else if (i <= 384)         return 10;
    else if (i <= 512)         return 11;
    else if (i <= 1024)        return 12;
    return 13;
}
static uint32_t getpoolsize(uint32_t idx) {
    switch (idx) {
    case 0:        return 8;
    case 1:        return 16;
    case 2:        return 24;
    case 3:        return 32;
    case 4:        return 48;
    case 5:        return 64;
    case 6:        return 96;
    case 7:        return 128;
    case 8:        return 192;
    case 9:        return 256;
    case 10:       return 384;
    case 11:       return 512;
    case 12:       return 1024;
    }
    return 1024;
}
static uint32_t getpoolsize_from_len(uint32_t i) {
    if (i <= 8)                return 8;
    if (i <= 12)               return 16;
    else if (i <= 16)          return 16;
    else if (i <= 24)          return 24;
    else if (i <= 32)          return 32;
    else if (i <= 48)          return 48;
    else if (i <= 64)          return 64;
    else if (i <= 96)          return 96;
    else if (i <= 128)         return 128;
    else if (i <= 192)         return 192;
    else if (i <= 256)         return 256;
    else if (i <= 384)         return 384;
    else if (i <= 512)         return 512;
    else if (i <= 1024)        return 1024;
    return 13;
}


static inline uint32_t get_pageno_PH(mypagehead* PH) {
    return (uint32_t)(PH - g_cachemem.headmem);
}
static inline uint32_t get_pageno_ptr(void* ptr) {
    uintptr_t addr = ptr;
    return (uint32_t)((addr - s_cast(uintptr_t, g_cachemem.pagemem)) / PAGE_SIZE);
}

static inline mypagehead* get_PH_ptr(void* ptr) {
    uintptr_t addr = ptr;
    const uint32_t pageno = get_pageno_ptr(ptr);
    return g_cachemem.headmem + pageno;
}

static inline uint32_t get_elemno_ptr(void* ptr) {
    uintptr_t addr = ptr;
    const uint32_t pageno
        //= pageno_get_by_ptr(ptr);
        = ((addr - s_cast(uintptr_t, g_cachemem.pagemem)) / PAGE_SIZE);
    const uint32_t poolsize = getpoolsize((g_cachemem.headmem + pageno)->poolidx);
    return (addr - s_cast(uintptr_t, g_cachemem.pagemem + pageno)) / poolsize;
}

extern void*    mypool_alloc(mysize_t len);
extern void     mypool_free(void* ptr);
extern void     mypool_destroy();
extern void     mypool_new(mysize_t max_threshold);


extern void  myarena_new(myarena* alc, mysize_t arena_size);
extern void* myarena_alloc(myarena* alc, mysize_t len);
extern void  myarena_free(myarena* alc);
extern void  myarena_reset_all(myarena* alc);
extern void  myarena_check_new(myarena* alc, myarena_check* checkpoint);
extern void  myarena_rewind(myarena* alc, myarena_check* checkpoint);
extern void* myarena_realloc(myarena* alc, void* p, mysize_t old_capa, mysize_t new_capa);

MY_EXTERN_END

#endif

/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYALLOCATOR_H__
#define __MYALLOCATOR_H__

#include "mylog.h"

#if MY_OS_WIN
#include <Windows.h>
#elif MY_OS_LINUX
#include <sys/mman.h>
#endif

#define _1KB (1024)
#define _4KB (4096)
#define _1MB (1024*1024)
#define _4MB (4096*1024)

#define ARENA_1KB (1024-sizeof(mypage))
#define ARENA_4KB (1024*4-sizeof(mypage))

#define ARENA_1MB (1024*1024-sizeof(mypage))
#define ARENA_4MB (1024*1024*4-sizeof(mypage))

#define ARENA_ALLOC(ALC, T, SZ) (T*)arena_alloc(ALC,sizeof(T)*(SZ))


#define MYVALLOC(T, SZ) (T*)myvalloc(sizeof(T)*(SZ))
#define MYVFREE(T, V, SZ) myvfree(V,sizeof(T)*(SZ))

static void* myvalloc(mysize_t capa) {
#if MY_OS_WINDOWS
	void* mem = VirtualAlloc(
		NULL,
		capa,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	MY_ASSERT(mem != NULL);
#elif MY_OS_LINUX
	void* mem = mmap(
		NULL,
		capa,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	MY_ASSERT(mem != MAP_FAILED);
#else
	void* mem = (mypage*)calloc(capa, 1);
	MY_ASSERT(mem != NULL);
#endif
	return mem;
}
static void myvfree(void* mem, mysize_t capa) {
#if MY_OS_WINDOWS
	VirtualFree(mem, 0, MEM_RELEASE);
#elif MY_OS_LINUX
	munmap(mem, capa);
#else
	free(mem);
#endif
}


#ifdef _DEBUG
extern size_t g_id;
#endif

typedef struct mypage {
	struct mypage* next;
#ifdef _DEBUG
	size_t id;
#endif
	mysize_t len;
	mysize_t capa;
	uint8_t ptr[];
}mypage;

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
	POOLMAP_SIZE = 32,
	POOL_8B = 0,
	POOL_16B, POOL_24B,
	POOL_32B, POOL_40B, POOL_48B, POOL_52B,
	POOL_64B, POOL_80B, POOL_96B, POOL_112B,
	POOL_128B, POOL_160B, POOL_192B, POOL_224B,
	POOL_256B, POOL_320B, POOL_384B, POOL_448B,
	POOL_512B,
	/*
	POOL_1KB,
	POOL_2KB,
	POOL_4KB,
	POOL_8KB,
	POOL_16KB,20 24 28
	POOL_32KB,40 48 56
	POOL_64KB,
	POOL_128KB,
	POOL_256KB,
	POOL_512KB,
	*/
	/*
	POOL_1MB,
	POOL_2MB,
	POOL_4MB,
	POOL_8MB,
	POOL_16MB,
	POOL_32MB,
	POOL_64MB,
	POOL_128MB,
	POOL_256MB,
	POOL_512MB,
	POOL_1GB,
	POOL_2GB,
	POOL_4GB,
	*/
};

/*
typedef struct mypool_head {

}mypool_head;
typedef struct mypool {
	freelist;
	len;
}mypool;

typedef struct mypool_head {
	struct {
		uintptr_t next : 56;
		uint32_t size : 8;
	};
	uint8_t ptr[];
}mypool_head;
struct mypool_block {
	mypool_head* blocklist;
	mysize_t size : 32;
	uint8_t ptr[];
};
typedef struct mypool {
	mypool_head* freelist;
	uint32_t block_len;
	uint32_t block_size : 5;//(8<<block_size) byte
	uint32_t blocklist_len : 27;
}mypool;
typedef struct mypoolmanager {
	mypool pool_map[POOLMAP_SIZE];
	uint32_t block_size_default[POOLMAP_SIZE];
	mysize_t total_size, threshold_size;
	//uint32_t threshold;
	//8byte : 8byte<<0, 16byte:8byte<<1 ...512byte:8byte<<6
	// 512KB : 8<<16
}mypoolmanager;//8


*/


typedef struct myfreepool {
	struct myfreepool* next;
} myfreepool;
typedef struct {
	//64
	myfreepool* freelist;
	//64
	uint8_t** blocklist;
	//64
	uint32_t elem_memsize; //4GB = 32
	uint32_t blocklist_cnt;
	//64
	uint16_t elem_maxcnt; //4GB = 32
	uint16_t elem_idx; //655.. 
	uint16_t block_idx; //4KB 4*1KB= * 655..
	uint16_t _;
} mypool;
typedef struct {
	mypool* poollist;
} mypoolmanager;

static void mypool_new() {

}
static void mypool_alloc() {

}
static void mypool_free() {
}
static void mypool_freeall() {

}
static void mypool_destroy() {

}

MY_EXTERN_START
extern void  myarena_new(myarena* alc, mysize_t arena_size);
extern void* myarena_alloc(myarena* alc, mysize_t len);
extern void  myarena_free(myarena* alc);
extern void  myarena_reset(myarena* alc);
extern void  myarena_check_new(myarena* alc, myarena_check* checkpoint);
extern void  myarena_rewind(myarena* alc, myarena_check* checkpoint);
extern void* myarena_realloc(myarena* alc, void* p, mysize_t old_capa, mysize_t new_capa);


extern void mypoolmanager_new(mypoolmanager* pool_mng);
#ifdef __USE_CLEAN__
//not yet
//extern void arena_clean(myarena* alc,myarena_check* checkpoint);
#endif
MY_EXTERN_END

#endif

/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
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

typedef enum {
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

	PAGE_SIZE=_4KB,
};

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
#if MY_OS_WINDOWS
static void* mymem_reserve(mysize_t capa) {
	//capa = ((capa - 1) / (_1KB * 64) + 1) * (_1KB * 64);
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
static void mymem_release(void* mem, mysize_t capa) {
	VirtualFree(mem, 0, MEM_RELEASE);
}
#elif MY_OS_LINUX
static void* mymem_reserve(mysize_t capa) {
	//capa = ((capa - 1) / (_1KB * 64) + 1) * (_1KB * 64);
	void* mem = mmap(NULL, capa, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	MY_ASSERT(p == MAP_FAILED);
	return mem;
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

typedef struct {
	mypage* blocklist;
	mypage* freelist;
	mysize_t allocsize;//now memory
	uint8_t defaultsize;//pow 2 basic:64KB
	uint8_t threshold;//pow 2 1<<32=4GB
} myglobalmemory;

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
	POOL_SIZE_MAX = 32,
	POOL_8B = 0,//128 (1024)
	POOL_16B, //64 (1024)
	POOL_24B, //42 (1008)
	POOL_32B,//32 (1024)
	POOL_40B, //25 (1000)
	POOL_48B, //21 (1000)
	POOL_52B, //19 (988)
	POOL_64B, //16 (1024)
	POOL_80B, 
	POOL_96B, 
	POOL_112B,
	POOL_128B, POOL_192B, //POOL_160B, POOL_192B, POOL_224B,
	POOL_256B, POOL_384B, //POOL_320B, POOL_384B, POOL_448B,
	POOL_512B, POOL_768B, //POOL_640B, POOL_768B, POOL_896B,
	/*
	POOL_1KB,POOL_1_5KB,
	POOL_2KB,POOL_3KB,
	POOL_4KB,POOL_6KB,
	POOL_8KB,POOL_12KB,
	POOL_16KB,POOL_24KB,
	POOL_32KB,POOL_48KB,
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
} mypoolsize_t;

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

typedef struct mypoolpage {
	struct mypoolpage* next;
	uint32_t len;
	uint16_t capa;
	uint16_t empty;
	uint8_t ptr[];
} mypoolpage;
//8   16  24, 32, 40, 48, 52, 64=64KB
//512 256 

typedef struct {
	//64
	struct myfreepool* freepool;
	//64
	mypage* blocklist;
	//64
	uint32_t elem_memsize; //4GB = 32
	uint32_t blocklist_cnt;
	//64
	uint16_t elem_maxcnt; //4GB = 32
	uint16_t elem_idx; //655.. 
	uint16_t block_idx; //4KB 4*1KB= * 655..
	uint8_t ptr[];
} mypool;
typedef struct {
	mypool* poollist;
} mypoolmanager;

static void mypool_new(uint32_t elem_memsize) {

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
myglobalmemory g_mymemory;

/*
mypoolmanager{
	RESERVED // basic : 4MB -> release/decommit 안 함. thread 마다 하나씩 있음.

	//64KB -> 4KB * 16
	SmallManager
	//64KB -> owner
	Middle
	NormalManager
	BigManager

	sizepool[8B 16B 24B ... ]
	Threshold POOL_THREADHOLD_MAX
	VMEM_THRESHOLD_MAX_DEFAULT=128MB
}
poolblocklist{
	//from pool memory manager
	VMEM [  64KB |  64KB      ]
	S_MNG [1:4KB | 2:4KB | :64KB] ->

	8B BLOCKLIST :
	1[] -> 9[] -> 10[] ...
	512B BLOCKLIST
	11[]->12[]
	BLOCKsdfsdfd
}*/



typedef struct mypoolblock {
	struct mypoolblock* ptr;
	//int i;
}mypoolblock;

//extern void  myarena_new_frompage(mypage* page, myarena* alc, mysize_t arena_size);
extern void  myarena_new(myarena* alc, mysize_t arena_size);
extern void* myarena_alloc(myarena* alc, mysize_t len);
extern void  myarena_free(myarena* alc);
extern void  myarena_reset_all(myarena* alc);
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

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

//#ifdef POOL_UNSAFE

enum {
	PAGE_TAG_EMPTY = 0b00,
	PAGE_TAG_HOLE = 0b01,
	PAGE_TAG_FULL = 0b10,
	PAGE_TAG_USING = 0b11,

	PAGE_SIZE = _16KB,
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
	POOL_8B, 
	POOL_16B,
	POOL_32B,
	POOL_64B,
	POOL_128B,
	POOL_256B,
	POOL_512B,
	POOL_1KB,
	POOL_2KB,
	POOL_4KB,
	POOL_8KB,
	POOL_SIZE_MAX,
	//POOL_8B = 0,//128 (1024)
	//POOL_16B, //64 (1024)
	//POOL_24B, //42 (1008)
	//POOL_32B,//32 (1024)
	//POOL_40B, //25 (1000)
	//POOL_48B, //21 (1000)
	//POOL_52B, //19 (988)
	//POOL_64B, //16 (1024)
	//POOL_80B, 
	//POOL_96B, 
	//POOL_112B,
	//POOL_128B, POOL_192B, //POOL_160B, POOL_192B, POOL_224B,
	//POOL_256B, POOL_384B, //POOL_320B, POOL_384B, POOL_448B,
	//POOL_512B, POOL_768B, //POOL_640B, POOL_768B, POOL_896B,
	
	// POOL_1KB,POOL_1_5KB,
	// POOL_2KB,POOL_3KB,
	// POOL_4KB,POOL_6KB,
	// POOL_8KB,POOL_12KB,
	// POOL_16KB,POOL_24KB,
	// POOL_32KB,POOL_48KB,
	// POOL_64KB,
	// POOL_128KB,
	// POOL_256KB,
	// POOL_512KB,
	
	
	// POOL_1MB,
	// POOL_2MB,
	// POOL_4MB,
	// POOL_8MB,
	// POOL_16MB,
	// POOL_32MB,
	// POOL_64MB,
	// POOL_128MB,
	// POOL_256MB,
	// POOL_512MB,
	// POOL_1GB,
	// POOL_2GB,
	// POOL_4GB,
	
} mypoolsize_t;

/*
// typedef struct mypool_head {
// 
// }mypool_head;
// typedef struct mypool {
// 	freelist;
// 	len;
// }mypool;
// 
// typedef struct mypool_head {
// 	struct {
// 		uintptr_t next : 56;
// 		uint32_t size : 8;
// 	};
// 	uint8_t ptr[];
// }mypool_head;
// struct mypool_block {
// 	mypool_head* blocklist;
// 	mysize_t size : 32;
// 	uint8_t ptr[];
// };
// typedef struct mypool {
// 	mypool_head* freelist;
// 	uint32_t block_len;
// 	uint32_t block_size : 5;//(8<<block_size) byte
// 	uint32_t blocklist_len : 27;
// }mypool;
// typedef struct mypoolmanager {
// 	mypool pool_map[POOLMAP_SIZE];
// 	uint32_t block_size_default[POOLMAP_SIZE];
// 	mysize_t total_size, threshold_size;
// 	//uint32_t threshold;
// 	//8byte : 8byte<<0, 16byte:8byte<<1 ...512byte:8byte<<6
// 	// 512KB : 8<<16
// }mypoolmanager;//8
*/


typedef struct mypooltag {
	struct mypooltag* next;
} mypooltag;
//freelist를 next에 pack하면 16바이트 압축 가능
typedef struct mypoolpage { 
	union {
		struct mypoolpage* next; //8
		uintptr_t tag;
	};
	//struct mypoolpage* prev;
	struct mypooltag* freelist; //8
	uint16_t elem_total; //
	uint16_t elem_len;
	uint16_t elem_empty;
	uint8_t memsz_idx;//8<<memsz
	uint8_t core_id;
	uint8_t ptr[];
} mypoolpage;//PG
typedef struct {
	uint16_t page_cnt;
	uint16_t page_size_def;//MAX 64KB
	uint32_t _;
	mypoolpage* page_curr;
	//mypoolpage* page_rest;
	//mypoolpage* page_tail;
	//mypoolpage* page_full;
	mypoolpage* page_hole;
} mysizepool;//SP
typedef struct {
	union {
		//400B
		struct {
			mysizepool szpool[POOL_SIZE_MAX];
			uint8_t core_id;
			mysize_t threshold;
			mypooltag* freepage;
			void* cachemem;
			mysize_t cachesize;
		};
		uint8_t _[64*7];
	};
} mysizepoolmanager;//SM

typedef struct {
	mysizepoolmanager core[MYCORE_MAX];
	uint8_t cnt_bits;
} mypoolmanager; //PM


extern mypoolmanager g_poolmng;
extern mymutex g_lock;
extern uint32_t g_slowloop;

static const mysize_t g_poolsize_map[POOL_SIZE_MAX]
= { 8u, 16u, 32u, 64u, 128u,
	256u,504u,1016u,2040u,4080u,
	8168u };

static const mysize_t g_poolpage_cnt[POOL_SIZE_MAX]
= { 2012u, 1014u, 509u, 255u, 127u, 
	63u, 32u, 16u, 8u, 4u, 
	2u };

static mysize_t poolidx_to_size(const uint32_t idx) {
	MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	if (idx >= 6u) {
		return g_poolsize_map[idx];
	}
	return 8u << idx;
}
static mysize_t page_head_size_X_get(const mysize_t PGsz, const uint32_t idx) {
	return max(PGsz / (8u << idx) / 8u, 8u);
}
static mysize_t page_head_size_get(const mysize_t PGsz, const uint32_t idx) {
	return sizeof(mypoolpage) + page_head_size_X_get(PGsz,idx);
}
static mysize_t usable_page_size_by_poolidx(const uint32_t idx) {
	const mysize_t head = page_head_size_get(PAGE_SIZE,idx);
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return PAGE_SIZE - head;
}
// 129-248(256B)
// 249-504(512B)
static const uint32_t memsize_to_poolidx(const mysize_t size) {
	const uint32_t msb = find_msb32_idx(size); 
	uint8_t pool_idx = (msb >= 3 ? msb - 3 : 0);
	if ((1u << msb) != size) pool_idx += 1u;
	//if(pool_idx)
	if (g_poolsize_map[pool_idx] < size)pool_idx += 1;
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return pool_idx;
}
//fastpath
static void* mypoolpage_freelist_get(mypoolpage* PG) {
	MY_ASSERT(
		((PG->elem_empty > 0) && PG->freelist != NULL)
		||(PG->elem_empty == 0) && PG->freelist == NULL);
	
	void* ptr = NULL;
	if (PG->freelist) {
		PG->elem_empty -= 1u;
		ptr = PG->freelist;
		PG->freelist = PG->freelist->next;
	}
	return ptr;
}


static void* mypoolpage_elem_get(mypoolpage* PG, const uint32_t idx) {
	uint32_t checksize = page_head_size_X_get(PAGE_SIZE, idx);
	return (PG->ptr + idx * g_poolsize_map[PG->memsz_idx]);
}
static void mypoolpage_init(mypoolpage* PG, mysize_t PG_size, uint8_t core_id, uint8_t pool_idx) {
	const mysize_t memsz = poolidx_to_size(pool_idx);
	const mysize_t head_size = sizeof(mypoolpage) + page_head_size_X_get(PG_size, pool_idx);
	memset(PG, 0, head_size);
	PG->core_id = core_id;
	PG->memsz_idx = pool_idx;
	PG->tag = PAGE_TAG_USING;

	PG->elem_total = g_poolpage_cnt[pool_idx];

	MY_LOG_INFO("Init mypoolpage %X (CID:%u MSZ:%u CNT:%u)", 
				PG, core_id, memsz, PG->elem_total);
}

//slowpath1 : inc elem len
static void* mypoolpage_elem_inc(mypoolpage* PG) {
	if (PG->elem_len == PG->elem_total) return NULL;
	return mypoolpage_elem_get(PG, PG->elem_len++);
	//fastpath
	//if (PG->elem_empty) PG->
}
static void mysizepool_new(mysizepoolmanager* SM, mysizepool* SP, uint8_t core_id, uint8_t pool_idx) {
	
	memset(SP, 0, sizeof(mysizepool));
	SP->page_size_def = PAGE_SIZE;
	SP->page_cnt = 1u;
	//SP->page_curr = mymem_rescommit(SP->page_size_def);
	
	MY_ASSERT(SM->cachemem);
	void* mem = mymem_commit(SM->cachemem, SM->cachesize + SP->page_size_def);
	MY_ASSERT_RETURN(SM->cachemem == mem,);
	
	SP->page_curr = r_cast(mypoolpage*, r_cast(uint8_t*, mem) + SM->cachesize);
	SM->cachesize += SP->page_size_def;

	MY_ASSERT(SP->page_curr);
	mypoolpage_init(SP->page_curr, SP->page_size_def, core_id, pool_idx);
}

static mypoolpage* mysizepool_page_new(mysizepoolmanager* SM, mysizepool* SP, uint8_t core_id, uint8_t pool_idx) {
	MY_ASSERT_RETURN(SP->page_cnt < 0xFFFF, 0);
	
	SP->page_cnt += 1u;
	const void* mem = mymem_commit(SM->cachemem, SM->cachesize + SP->page_size_def);
	mypoolpage* page = r_cast(mypoolpage*, (uint8_t*)mem + SM->cachesize);
	SM->cachesize += SP->page_size_def;

	mypoolpage_init(page, SP->page_size_def, core_id, pool_idx);
	//page->prev = SP->page_curr;
	return page;
}


//just initialize, clear NULL => g_poolmng.core[core_idx]
static void mysizepoolmanager_init(mysize_t threshold, uint8_t core_idx) {
	mysizepoolmanager* const SM = &g_poolmng.core[core_idx];
	SM->core_id = core_idx + 1;
	SM->freepage = NULL;
	SM->threshold = threshold;
	MY_ASSERT(SM->cachemem == NULL);
	SM->cachemem = mymem_reserve(threshold);
	SM->cachesize = 0u;
	//SM->szpool[POOL_8B].
}
static void* mysizepoolmanager_freepage_get(mysizepoolmanager* SM) {
	void* ptr = SM->freepage;
	if (ptr == NULL)return NULL;
	SM->freepage = SM->freepage->next;
	return ptr;
}

//O(N)을 막을 방법 ? SP에서 빈 녀석들 개수(o. 가능) 혹은 Page 주소? (x. 불가능)
static void* mysizepool_hole_get(mysizepool* SP){
	mypoolpage* PG = SP->page_hole;
	if (PG == NULL) return NULL;
	SP->page_hole = SP->page_hole->next;
	SP->page_curr = PG;
	MY_ASSERT(PG->freelist);
	MY_ASSERT(PG->elem_empty);
	PG->freelist = PG->freelist->next;
	PG->elem_empty -= 1u;
	return PG;
	//SP->page_hole = s_cast(uintptr_t, SP->page_hole->next) & ~((uintptr_t)0b11);
	//mypoolpage* PG_prev = NULL;
	//void* ptr;
	//if (PG == NULL) return NULL;
	////rest => elem_empty =0 -> N -> N-1
	//if(PG->elem_total > (PG->elem_len - PG->elem_empty)){
	//	PG->elem_empty -= 1u;
	//	SP->page_rest = PG->next;
	//	SP->page_curr = PG;
	//	goto l_succ;
	//}
	//
	//while(true){
	//	g_slowloop++;
	//	PG_prev = PG;
	//	PG = PG->next;
	//	if(PG==NULL) return NULL;
	//	if(PG->elem_total == (PG->elem_len - PG->elem_empty)) continue;
	//	PG_prev->next = PG->next;
	//	SP->page_curr = PG;
	//	break;
	//}
	//l_succ:
	//	MY_ASSERT(PG->freelist != NULL);
	//	ptr = PG->freelist;
	//	PG->freelist = PG->freelist->next;
	//	return PG->freelist;
}

//PG->next를 sizepool로 위치시키는 것도 가능.
static void mysizepool_hole_push(mypoolpage* PG) {
	const uint8_t core_idx = PG->core_id - 1u;
	mysizepoolmanager* SM = &g_poolmng.core[core_idx];
	mysizepool* SP = &SM->szpool[PG->memsz_idx];
	//g_slowloop++;
	PG->next = SP->page_hole;
	SP->page_hole = PG;

	//page_full -> page_rest
	//if(PG->elem_empty==PG->elem_total)
	//PG->elem_empty++;

//	if (PG->elem_empty < PG->elem_total) return;
//	SM->freepage;
//	//[PG->memsz_id]
}
// 연결 리스트 왔다리 갔다리 하는 쪽에서 문제가 있음.
//extern int g_path[5];
static void* mysizepoolmanager_alloc(uint8_t core_id, uint8_t pool_idx) {
	//
	const uint8_t core_idx = core_id - 1u;
	//여기 접근이 매우 느림
	mysizepoolmanager* const SM = &g_poolmng.core[core_idx];
	mysizepool* SP = &SM->szpool[pool_idx];
	if (SP->page_curr == NULL) {
		MY_LOG_INFO("Init mysizepool (CID:%u POOL%uB)", core_id, 8u << pool_idx);
		mysizepool_new(SM, SP, core_id, pool_idx);
		//New SP;
	}
	void* ptr = NULL;
	//fastpath
	
	//slowpath1 = inc element
	ptr = mypoolpage_elem_inc(SP->page_curr);
	if (ptr) {
		g_path[0]++;
		MY_LOG_INFO("FastPath1 : ");
		return ptr;
	}

	ptr = mypoolpage_freelist_get(SP->page_curr);
	if (ptr) {
		g_path[1]++;
		MY_LOG_INFO("FastPath2 : ");
		return ptr;
	}
	
	SP->page_curr->tag = PAGE_TAG_FULL;
	SP->page_curr = NULL;
	//Page

	//slowpath1.5 = from page_rest
	//O(N)
	//ptr = slowpath1_5(SP);
	ptr = mysizepool_hole_get(SP);
	if (ptr) {
		//g_path[2]++;
		MY_LOG_INFO("SlowPath1.5 : ");

		return ptr;
	}
	else {//Not find
		//SP->page_curr->next = SP->page_hole;
		//SP->page_rest = SP->page_curr;
		
	}
	
	//slowpath2 = new freepage
	mypoolpage* PG = mysizepoolmanager_freepage_get(SM);
	//slowpath3 = no freepage -> new page
	if (PG == NULL) {
		//g_path[4]++;
		PG = mysizepool_page_new(SM,SP, core_id, pool_idx);
		MY_LOG_INFO("SlowPath3 : ");
		if (PG == NULL) {
			MY_LOG_INFO("All SlowPath Failed: ");
			return NULL;
		}
	}
	else {
		//g_path[3]++;
		MY_LOG_INFO("SlowPath2 : ");
	}
	//init page and get elem[0]
	SP->page_curr = PG;
	//mypoolpage_init(PG, SP->page_size_def, core_id, pool_idx);
	ptr = mypoolpage_elem_inc(PG);
	return ptr;

}

static uint8_t mypoolmng_core_new(mysize_t max_threshold) {
	mymutex_lock(&g_lock);
	for (uint8_t bit = 0u; bit < 8u; bit++) {
		if (g_poolmng.cnt_bits & (1u << bit)) continue;
		g_poolmng.cnt_bits |= (1u << bit);
		mysizepoolmanager_init(max_threshold, bit);
		mymutex_unlock(&g_lock);
		return bit + 1u;
	}
	mymutex_unlock(&g_lock);
	//MY_ASSERT(0);
	return 0u;
	//g_poolmng.cache[0].szpool
}

//cnt < MYCORE_MAX
static uint8_t mypoolmng_new(mysize_t max_threshold,uint8_t cnt) {
	MY_ASSERT_RETURN(cnt < MYCORE_MAX, 0u);
	for (uint8_t i = 0u; i < cnt; i++)
		mypoolmng_core_new(max_threshold);
	return g_poolmng.cnt_bits;
	//g_poolmng.cache[0].szpool
}

static void mysizepoolmanager_destroy(mysizepoolmanager* SM) {
	MY_ASSERT_RETURN(SM->cachemem,);
	mymem_release(SM->cachemem, SM->cachesize);
	SM->cachesize = 0u;
		//SM->freepage
		//mypoolpage_free_all()
}
//CRITICAL ZONE -> do mutex or lock
static void mypoolmng_destroy_id(const uint8_t id) {
	MY_ASSERT_RETURN(id <= 8u && id != 0u,); 
	MY_ASSERT(g_poolmng.cnt_bits & (1u << (id-1u)));
	mymutex_lock(&g_lock);
	{
		const uint8_t idx = id - 1u;
		g_poolmng.cnt_bits ^= (1u << (idx));
		mysizepoolmanager_destroy(&g_poolmng.core[idx]);
	}
	mymutex_unlock(&g_lock);
	//g_poolmng.cache[0].szpool
}
static void mypoolmng_destroy() {
	for (uint8_t bit = 0u; bit < 8u; bit++) {
		if ((g_poolmng.cnt_bits & (1u << bit)))
			mypoolmng_destroy_id(bit + 1u);
	}
}
static void* mypool_alloc(uint8_t core_id, mysize_t len, mysize_t ms) {
	const mysize_t size = (len * ms);
	MY_ASSERT_RETURN(core_id <= MYCORE_MAX, 0);
	MY_ASSERT_RETURN((size > 0) && (size < _4KB), NULL);
	
	uint8_t pool_idx = memsize_to_poolidx(size);
	
	const uint8_t core_idx = core_id - 1u;
	void* ptr = mysizepoolmanager_alloc(core_id, pool_idx);
	MY_LOG_INFO("ALLOC %X SZ:(%u x %u)", ptr, len, ms);
	return ptr;
}
//static mysizepoolmng_getcore(uint32_t id) {
//	g_poolmng.core[id];
//}
// this_page == page_rest | page_curr -> SP -> freepage 가능?
static uint32_t mypool_free(void* ptr) {
	//just add freelist
	//page addr
	
	
	
	
	const uintptr_t PH_addr = (r_cast(uintptr_t, ptr) / PAGE_SIZE) * PAGE_SIZE;
	mypoolpage* const PG = r_cast(mypoolpage*, PH_addr);


	// path1 : PG->freelist
	mypooltag* FL = PG->freelist;
	PG->freelist = r_cast(mypooltag*, ptr);
	PG->freelist->next = FL;
	PG->elem_empty++;
	
	// was full page -> partial
	if (!(PG->elem_empty == 1u && PG->elem_len == PG->elem_total)) return true;
	
	//mysizepoolmanager* const SM = &g_poolmng.core[PG->core_id - 1u];
	//mysizepool* SP = &SM->szpool[PG->memsz_idx];
	if (PG->tag == PAGE_TAG_FULL) {
		mysizepool_hole_push(PG);
	}
	return true;
}
static void mypool_freeall() {
}
static void mypool_destroy() {
}


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

#ifdef __USE_CLEAN__
//not yet
//extern void arena_clean(myarena* alc,myarena_check* checkpoint);
#endif
MY_EXTERN_END

#endif
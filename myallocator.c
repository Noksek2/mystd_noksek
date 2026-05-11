//260427-1
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.1
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "myallocator.h"

#ifdef _DEBUG
size_t g_id = 0u;
#endif


/*arena allocator*/
void myarena_new(myarena* alc, mysize_t arena_size) {
	mypage* arena = mymem_reserve(arena_size);
	mymem_commit(arena, arena_size);

	arena->capa = arena_size;

	alc->arena_size = arena_size;
	alc->current = arena;
	alc->head = arena;

#ifdef _DEBUG
		alc->head->id = g_id++;
#endif
	MY_LOG_INFO("new allocator [%llX](%u)", (uint64_t)alc->head, arena_size);
}

void* myarena_alloc(myarena* alc, mysize_t append_len) {

	// debugbreak if append_len is 0
	MY_ASSERT(append_len != 0);

	void* ptr = NULL;
	mypage* arena = alc->current;
	mysize_t padding = GET_PADDING(append_len);
	//MY_LOG_INFO("try id:%llu [%llX] alloc(%d,%d)... ", arena->id, (uint64_t)arena, append_len, padding, arena->capa);

	if (arena->len + append_len <= arena->capa) {

		MY_LOG_INFO("Done [%llX]", (uint64_t)arena->ptr + arena->len);

		ptr = arena->ptr + arena->len;
		arena->len += padding;
	}
	else if (arena->next != NULL) {
		if (append_len <= arena->next->capa) {
			arena->next->len = 0u;
			alc->current = arena->next;
			ptr = alc->current->ptr;

			MY_LOG_INFO("Enable next block id:%llu [%llX]", (uint64_t)alc->current->id, (uint64_t)alc->current);

		}
	}
	//arena->next ==0 || append_len > arena->next->capa 
	if (ptr==NULL) {
		mysize_t new_size = max(alc->arena_size, padding);
		mypage* new_arena = mymem_reserve(new_size);
		new_arena = mymem_commit(new_arena, new_size);
		MY_ASSERT(new_arena != NULL);
		
		new_arena->len = padding;
		new_arena->capa = new_size;
		new_arena->next = arena->next;

		arena->next = new_arena;

		alc->current = new_arena;

		ptr = alc->current->ptr;
#ifdef _DEBUG
		alc->current->id = g_id++;
#endif
		MY_LOG_INFO("full! new allocator id:%llu [%llX], Done [%llX]", alc->current->id, (uint64_t)alc->current, (uint64_t)alc->current->ptr);
	}
	return ptr;
	//arena_alloc(alc->head, len);
}
void myarena_reset_all(myarena* alc) {
	alc->head->len = 0;
	alc->current = alc->head;
}
void myarena_rewind(myarena* alc, myarena_check* checkpoint) {
#if MY_LOG_LEVEL >= MY_LOG_LEVEL_INFO
	printf("before : ");

	for (mypage* head = alc->head; head != 0; head = head->next) {

		printf("[%llu:%u/%u] ", head->id, head->len, head->capa);

	}
	puts("");
#endif

	alc->current = checkpoint->tail;
	alc->current->len = checkpoint->len;

#if MY_LOG_LEVEL >= MY_LOG_LEVEL_INFO
	printf("rewind : ");
	for (mypage* head = alc->head; head != 0; head = head->next) {
		printf("[%llu:%u/%u] ", head->id, head->len, head->capa);
	}
	puts("");
#endif
}
void myarena_free(myarena* alc) {
	mypage* next = 0;
	mypage* now = alc->head;
	while (now != NULL) {
		next = now->next;
		MY_LOG_INFO("free block id:%llu [%llX]..", now->id, (uint64_t)now);
		mymem_release(now, now->capa + sizeof(mypage));
		now = next;
	}
	alc->head = 0;
	alc->current = 0;
}

void myarena_check_new(myarena* alc, myarena_check* checkpoint) {
	myarena_check ch = {
		.head = alc->head,
		.tail = alc->current,
		.len = alc->current->len,
	};
	*checkpoint = ch;
}
void* myarena_realloc(myarena* alc, void* p, mysize_t old_capa, mysize_t new_capa) {
	uint8_t* arena_ptr = (alc->current->ptr + alc->current->len);

	if ((arena_ptr == (uint8_t*)p + old_capa) 
		&& (arena_ptr + new_capa <= alc->current->capa)) {
		alc->current->len += new_capa;
		return arena_ptr;
	}
	return myarena_alloc(alc, new_capa);
}




static mysize_t poolidx_to_size(const uint32_t pidx) {
	MY_ASSERT_RETURN(pidx < POOL_SIZE_MAX, POOL_SIZE_MAX - 1u);
	if (pidx >= 6u) {
		return g_poolsize_map[pidx];
	}
	return 8u << pidx;
}
static mysize_t page_head_size_X_get(const mysize_t PGsz, const uint32_t pool_idx) {
	return max(PGsz / (8u << pool_idx) / 8u, 8u);
}
static mysize_t page_head_size_get(const mysize_t PGsz, const uint32_t pool_idx) {
	return sizeof(mypoolpage) + page_head_size_X_get(PGsz, pool_idx);
}
static mysize_t usable_page_size_by_poolidx(const uint32_t pool_idx) {
	const mysize_t head = page_head_size_get(PAGE_SIZE, pool_idx);
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return PAGE_SIZE - head;
}

static const uint32_t memsize_to_poolidx(const mysize_t size) {
	const uint32_t msb = find_msb32_idx(size);
	uint8_t pool_idx = (msb >= 3 ? msb - 3 : 0);
	if ((1u << msb) != size) pool_idx += 1u;

	if (g_poolsize_map[pool_idx] < size)pool_idx += 1;
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return pool_idx;
}
//fastpath
static void* mypoolpage_freelist_get(mypoolpage* PG) {
	MY_ASSERT(
		((PG->elem_empty > 0) && PG->freeoff != NULL)
		|| (PG->elem_empty == 0) && PG->freeoff == NULL);

	void* ptr = NULL;
	if (PG->freeoff) {
		PG->elem_empty -= 1u;
		ptr = (void*)((uintptr_t)(PG) + PG->freeoff);
		PG->freeoff = r_cast(myfreepool*, ptr)->nextoff;
	}
	return ptr;
}


static void* mypoolpage_elem_get(mypoolpage* PG, const uint32_t eidx) {
	uint32_t checksize = page_head_size_X_get(PAGE_SIZE, PG->memsz_idx);
	return (PG->ptr + checksize + eidx * g_poolsize_map[PG->memsz_idx]);
}
static void mypoolpage_init(mysizepool* SP, mypoolpage* PG, mysize_t PG_size, uint8_t core_id, uint8_t pool_idx) {
	const mysize_t memsz = poolidx_to_size(pool_idx);
	const mysize_t head_size = sizeof(mypoolpage) + page_head_size_X_get(PG_size, pool_idx);
	memset(PG, 0, head_size);
	PG->pSP = SP;
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
	SP->pSM = SM;
	SP->page_hole_off = PAGE_HOLE_DEF;
	
	//SP->page_curr = mymem_rescommit(SP->page_size_def);

	MY_ASSERT(SM->cachemem);
	void* mem = mymem_commit(SM->cachemem, SM->cachesize + SP->page_size_def);
	MY_ASSERT_RETURN(SM->cachemem == mem, );

	SP->page_curr = r_cast(mypoolpage*, r_cast(uint8_t*, mem) + SM->cachesize);
	SP->cachemem = s_cast(uintptr_t, SM->cachemem);

	SM->cachesize += SP->page_size_def;

	MY_ASSERT(SP->page_curr);
	mypoolpage_init(SP, SP->page_curr, SP->page_size_def, core_id, pool_idx);
}

static mypoolpage* mysizepool_page_new(mysizepoolmanager* SM, mysizepool* SP, uint8_t core_id, uint8_t pool_idx) {
	MY_ASSERT_RETURN(SP->page_cnt < 0xFFFF, 0);

	SP->page_cnt += 1u;
	const void* mem = mymem_commit(SM->cachemem, SM->cachesize + SP->page_size_def);
	MY_ASSERT_RETURN(mem == SM->cachemem, NULL);
	mypoolpage* page = r_cast(mypoolpage*, (uint8_t*)mem + SM->cachesize);
	SM->cachesize += SP->page_size_def;

	mypoolpage_init(SP, page, SP->page_size_def, core_id, pool_idx);
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
}
static void* mysizepoolmanager_freepage_get(mysizepoolmanager* SM) {
	void* ptr = SM->freepage;
	if (ptr == NULL)return NULL;
	SM->freepage = SM->freepage->next;
	return ptr;
}

static void* mysizepool_hole_pop(mysizepool* SP) {
	//PG_curr = page_hole
	//page_hole
	uintptr_t PG_addr = SP->cachemem + SP->page_hole_off;
	mypoolpage* PG = r_cast(mypoolpage*, PG_addr);
	if (PG == NULL) return NULL;
	SP->page_hole_off = PG->nextpageoff;
	SP->page_curr = PG;
	MY_ASSERT(PG->freeoff);
	MY_ASSERT(PG->elem_empty);
	
	myfreepool* FL = r_cast(myfreepool*, PG_addr + PG->freeoff);
	PG->tag = PAGE_TAG_USING;
	PG->freeoff = FL->nextoff;
	PG->elem_empty -= 1u;
	return PG;
}

//PG->next를 sizepool로 위치시키는 것도 가능.
static void mysizepool_hole_push(mypoolpage* PG) {
	mysizepool* SP = PG->pSP;
	
	PG->nextpageoff = SP->page_hole_off;
	SP->page_hole_off = s_cast(uint32_t, (uintptr_t)PG - s_cast(uintptr_t, SP->cachemem));

}
// 연결 리스트 왔다리 갔다리 하는 쪽에서 문제가 있음.
//extern int g_path[5];
static void* mysizepoolmanager_alloc(uint8_t core_id, uint8_t pool_idx) {
	//
	const uint8_t core_idx = core_id - 1u;
	mysizepoolmanager* const SM = &g_poolmng.core[core_idx];
	mysizepool* SP = &SM->szpool[pool_idx];
	if (SP->page_curr == NULL) {
		MY_LOG_INFO("Init mysizepool (CID:%u POOL%uB)", core_id, 8u << pool_idx);
		mysizepool_new(SM, SP, core_id, pool_idx);
	}
	void* ptr = NULL;

	ptr = mypoolpage_elem_inc(SP->page_curr);
	if (ptr) {
		MY_LOG_INFO("FastPath1 : ");
		return ptr;
	}

	ptr = mypoolpage_freelist_get(SP->page_curr);
	if (ptr) {
		MY_LOG_INFO("FastPath2 : ");
		return ptr;
	}

	SP->page_curr->tag = PAGE_TAG_FULL;
	SP->page_curr = NULL;
	
	//slowpath1
	if (SP->page_hole_off == 1u){}
	else {
		ptr = mysizepool_hole_pop(SP);
		if (ptr) {
			MY_LOG_INFO("SlowPath1.5 : ");
			return ptr;
		}
	}

	//slowpath2 = new freepage
	mypoolpage* PG = mysizepoolmanager_freepage_get(SM);
	//slowpath3 = no freepage -> new page
	if (PG == NULL) {
		PG = mysizepool_page_new(SM, SP, core_id, pool_idx);
		MY_LOG_INFO("SlowPath3 : ");
		MY_ASSERT_RETURN(PG, NULL);
	}
	else {
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
static uint8_t mypoolmng_new(mysize_t max_threshold, uint8_t cnt) {
	MY_ASSERT_RETURN(cnt < MYCORE_MAX, 0u);
	for (uint8_t i = 0u; i < cnt; i++)
		mypoolmng_core_new(max_threshold);
	return g_poolmng.cnt_bits;
	//g_poolmng.cache[0].szpool
}

static void mysizepoolmanager_destroy(mysizepoolmanager* SM) {
	MY_ASSERT_RETURN(SM->cachemem, );
	mymem_release(SM->cachemem, SM->cachesize);
	memset(SM, 0, sizeof(mysizepoolmanager));
	SM->cachesize = 0u;
}
//CRITICAL ZONE -> do mutex or lock
static void mypoolmng_destroy_id(const uint8_t id) {
	MY_ASSERT_RETURN(id <= 8u && id != 0u, );
	MY_ASSERT(g_poolmng.cnt_bits & (1u << (id - 1u)));
	mymutex_lock(&g_lock);
	{
		const uint8_t idx = id - 1u;
		g_poolmng.cnt_bits ^= (1u << (idx));
		mysizepoolmanager_destroy(&g_poolmng.core[idx]);
	}
	mymutex_unlock(&g_lock);
}
static void mypoolmng_destroy() {
	for (uint8_t bit = 0u; bit < 8u; bit++) {
		if ((g_poolmng.cnt_bits & (1u << bit)))
			mypoolmng_destroy_id(bit + 1u);
	}
}


uint8_t mypool_new(mysize_t max_threshold) {
	return mypoolmng_core_new(max_threshold);
}
void* mypool_alloc(uint8_t core_id, mysize_t len, mysize_t ms) {
	const mysize_t size = (len * ms);
	MY_ASSERT_RETURN(core_id <= MYCORE_MAX, 0);
	MY_ASSERT_RETURN((size > 0) && (size < PAGE_SIZE-sizeof(mypoolpage)-8), NULL);

	uint8_t pool_idx = memsize_to_poolidx(size);

	const uint8_t core_idx = core_id - 1u;
	void* ptr = mysizepoolmanager_alloc(core_id, pool_idx);
	//MY_LOG_INFO("ALLOC %X SZ:(%u x %u)", ptr, len, ms);
	return ptr;
}

uint32_t mypool_free(void* ptr) {
	uintptr_t ptr_addr = r_cast(uintptr_t, ptr);
	const uintptr_t PH_addr = (r_cast(uintptr_t, ptr) / PAGE_SIZE) * PAGE_SIZE;
	mypoolpage* const PG = r_cast(mypoolpage*, PH_addr);

	// path1 : PG->freelist
	//myfreepool* prev_FL = r_cast(myfreepool*, PH_addr + prev_freeoff);
	myfreepool* FL = r_cast(myfreepool*, ptr);
	
#ifdef _DEBUG
	//check;
#endif
	
	FL->nextoff = PG->freeoff;
	PG->freeoff = s_cast(uint32_t, ptr_addr - PH_addr);
	
	PG->elem_empty++;

	// was full page -> partial
	if (!(PG->elem_empty == 1u && PG->elem_len == PG->elem_total)) return true;

	//mysizepoolmanager* const SM = &g_poolmng.core[PG->core_id - 1u];
	//mysizepool* SP = &SM->szpool[PG->memsz_idx];
	if (PG->tag == PAGE_TAG_FULL) {
		PG->tag = PAGE_TAG_HOLE;
		mysizepool_hole_push(PG);
	}
	return true;
}
void mypool_destroy() {
	mypoolmng_destroy();
}
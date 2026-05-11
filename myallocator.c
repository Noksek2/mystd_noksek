
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
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
	MY_LOG_TEST("new allocator [%llX](%u)", (uint64_t)alc->head, arena_size);
}

void* myarena_alloc(myarena* alc, mysize_t append_len) {

	// debugbreak if append_len is 0
	MY_ASSERT(append_len != 0);

	void* ptr = NULL;
	mypage* arena = alc->current;
	mysize_t padding = GET_PADDING(append_len);
	//MY_LOG_TEST("try id:%llu [%llX] alloc(%d,%d)... ", arena->id, (uint64_t)arena, append_len, padding, arena->capa);

	if (arena->len + append_len <= arena->capa) {

		MY_LOG_TEST("Done [%llX]", (uint64_t)arena->ptr + arena->len);

		ptr = arena->ptr + arena->len;
		arena->len += padding;
	}
	else if (arena->next != NULL) {
		if (append_len <= arena->next->capa) {
			arena->next->len = 0u;
			alc->current = arena->next;
			ptr = alc->current->ptr;

			MY_LOG_TEST("Enable next block id:%llu [%llX]", (uint64_t)alc->current->id, (uint64_t)alc->current);

		}
	}
	//arena->next ==0 || append_len > arena->next->capa 
	if (ptr == NULL) {
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
		MY_LOG_TEST("full! new allocator id:%llu [%llX], Done [%llX]", alc->current->id, (uint64_t)alc->current, (uint64_t)alc->current->ptr);
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
		MY_LOG_TEST("free block id:%llu [%llX]..", now->id, (uint64_t)now);
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
static mysize_t page_head_size_X_get(const mysize_t PGsz, const uint32_t poolidx) {
	return max(PGsz / (8u << poolidx) / 8u, 8u);
}
static mysize_t page_head_size_get(const mysize_t PGsz, const uint32_t poolidx) {
	return sizeof(mypoolpage) + page_head_size_X_get(PGsz, poolidx);
}
static mysize_t usable_page_size_by_poolidx(const uint32_t poolidx) {
	const mysize_t head = page_head_size_get(SLOWPOOL_PAGE_SIZE, poolidx);
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return SLOWPOOL_PAGE_SIZE - head;
}

static const uint32_t memsize_to_poolidx(const mysize_t size) {
	const uint32_t msb = find_msb32_idx(size);
	uint8_t poolidx = (msb >= 3 ? msb - 3 : 0);
	if ((1u << msb) != size) poolidx += 1u;

	if (g_poolsize_map[poolidx] < size)poolidx += 1;
	//MY_ASSERT_RETURN(idx <= POOL_SIZE_MAX, POOL_SIZE_MAX);
	return poolidx;
}
//fastpath
static void* mypoolpage_freelist_get(mypoolpage* PG) {
	MY_ASSERT(
		((PG->elem_empty > 0) && PG->freeoff != 0u)
		|| (PG->elem_empty == 0) && PG->freeoff == 0u);

	void* ptr = NULL;
	if (PG->freeoff) {
		PG->elem_empty -= 1u;
		ptr = (void*)((uintptr_t)(PG)+PG->freeoff);
		PG->freeoff = r_cast(myfreepool*, ptr)->nextoff;
	}
	return ptr;
}


static void* mypoolpage_elem_get(mypoolpage* PG, const uint32_t eidx) {
	uint32_t checksize = page_head_size_X_get(SLOWPOOL_PAGE_SIZE, PG->memsz_idx);
	return (PG->ptr + checksize + eidx * g_poolsize_map[PG->memsz_idx]);
}
static void mypoolpage_init(mysizepool* SP, mypoolpage* PG, mysize_t PG_size, uint8_t core_id, uint8_t poolidx) {
	const mysize_t memsz = poolidx_to_size(poolidx);
	const mysize_t head_size = sizeof(mypoolpage) + page_head_size_X_get(PG_size, poolidx);
	memset(PG, 0, head_size);
	PG->pSP = SP;
	PG->memsz_idx = poolidx;
	PG->tag = PAGE_TAG_USING;

	PG->elem_total = g_poolpage_cnt[poolidx];

	MY_LOG_TEST("Init mypoolpage %X (CID:%u MSZ:%u CNT:%u)",
				PG, core_id, memsz, PG->elem_total);
}

//slowpath1 : inc elem len
static void* mypoolpage_elem_inc(mypoolpage* PG) {
	if (PG->elem_len == PG->elem_total) return NULL;
	return mypoolpage_elem_get(PG, PG->elem_len++);
	//fastpath
	//if (PG->elem_empty) PG->
}
static void mysizepool_new(mysizepoolmanager* SM, mysizepool* SP, uint8_t core_id, uint8_t poolidx) {

	memset(SP, 0, sizeof(mysizepool));
	SP->page_size_def = SLOWPOOL_PAGE_SIZE;
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
	mypoolpage_init(SP, SP->page_curr, SP->page_size_def, core_id, poolidx);
}

static mypoolpage* mysizepool_page_new(mysizepoolmanager* SM, mysizepool* SP, uint8_t core_id, uint8_t poolidx) {
	MY_ASSERT_RETURN(SP->page_cnt < 0xFFFF, 0);

	SP->page_cnt += 1u;
	const void* mem = mymem_commit(SM->cachemem, SM->cachesize + SP->page_size_def);
	MY_ASSERT_RETURN(mem == SM->cachemem, NULL);
	mypoolpage* page = r_cast(mypoolpage*, (uint8_t*)mem + SM->cachesize);
	SM->cachesize += SP->page_size_def;

	mypoolpage_init(SP, page, SP->page_size_def, core_id, poolidx);
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

	//mypoolpage* r_cast(mypoolpage*, cachemem + SP->page_hole_off);
	PG->nextpageoff = SP->page_hole_off;
	SP->page_hole_off = s_cast(uint32_t, (uintptr_t)PG - s_cast(uintptr_t, SP->cachemem));

	//page_full -> page_rest
	//if(PG->elem_empty==PG->elem_total)
	//PG->elem_empty++;

//	if (PG->elem_empty < PG->elem_total) return;
//	SM->freepage;
//	//[PG->memsz_id]
}
// 연결 리스트 왔다리 갔다리 하는 쪽에서 문제가 있음.
//extern int g_path[5];
static void* mysizepoolmanager_alloc(uint8_t core_id, uint8_t poolidx) {
	//
	const uint8_t core_idx = core_id - 1u;
	//여기 접근이 매우 느림
	mysizepoolmanager* const SM = &g_poolmng.core[core_idx];
	mysizepool* SP = &SM->szpool[poolidx];
	if (SP->page_curr == NULL) {
		MY_LOG_TEST("Init mysizepool (CID:%u POOL%uB)", core_id, 8u << poolidx);
		mysizepool_new(SM, SP, core_id, poolidx);
		//New SP;
	}
	void* ptr = NULL;
	//fastpath

	//slowpath1 = inc element
	ptr = mypoolpage_elem_inc(SP->page_curr);
	if (ptr) {
		//g_path[0]++;
		MY_LOG_TEST("FastPath1 : ");
		return ptr;
	}

	ptr = mypoolpage_freelist_get(SP->page_curr);
	if (ptr) {
		//g_path[1]++;
		MY_LOG_TEST("FastPath2 : ");
		return ptr;
	}

	SP->page_curr->tag = PAGE_TAG_FULL;
	SP->page_curr = NULL;
	//Page

	//slowpath1.5 = from page_rest
	if (SP->page_hole_off == 1u) {}
	else {
		ptr = mysizepool_hole_pop(SP);
		if (ptr) {
			//g_path[2]++;
			MY_LOG_TEST("SlowPath1.5 : ");
			return ptr;
		}
	}

	//slowpath2 = new freepage
	mypoolpage* PG = mysizepoolmanager_freepage_get(SM);
	//slowpath3 = no freepage -> new page
	if (PG == NULL) {
		//g_path[4]++;
		PG = mysizepool_page_new(SM, SP, core_id, poolidx);
		MY_LOG_TEST("SlowPath3 : ");
		MY_ASSERT_RETURN(PG, NULL);
	}
	else {
		//g_path[3]++;
		MY_LOG_TEST("SlowPath2 : ");
	}
	//init page and get elem[0]
	SP->page_curr = PG;
	//mypoolpage_init(PG, SP->page_size_def, core_id, poolidx);
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


uint8_t myslowpool_new(mysize_t max_threshold) {
	return mypoolmng_core_new(max_threshold);
}
void* myslowpool_alloc(uint8_t core_id, mysize_t len, mysize_t ms) {
	const mysize_t size = (len * ms);
	MY_ASSERT_RETURN(core_id <= MYCORE_MAX, 0);
	MY_ASSERT_RETURN((size > 0) && (size < SLOWPOOL_PAGE_SIZE - sizeof(mypoolpage) - 8), NULL);

	uint8_t poolidx = memsize_to_poolidx(size);

	const uint8_t core_idx = core_id - 1u;
	void* ptr = mysizepoolmanager_alloc(core_id, poolidx);
	//MY_LOG_TEST("ALLOC %X SZ:(%u x %u)", ptr, len, ms);
	return ptr;
}

uint32_t myslowpool_free(void* ptr) {
	uintptr_t ptr_addr = r_cast(uintptr_t, ptr);
	const uintptr_t PH_addr = (r_cast(uintptr_t, ptr) / SLOWPOOL_PAGE_SIZE) * SLOWPOOL_PAGE_SIZE;
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
void myslowpool_destroy() {
	mypoolmng_destroy();
}

extern void mycachemem_new(uint32_t size) {
	mycachemem* CM = &g_cachemem;

	MY_ASSERT(CM->headmem == NULL);
	memset(&g_cachemem, 0, sizeof(mycachemem));

	void* phead;
	phead = mymem_reserve(sizeof(mycachehead) + (size / sizeof(mypagedata))*sizeof(mypagehead));
	CM->cachehead = mymem_commit(phead, _8KB);
	CM->commit_page_cnt = (_8KB - sizeof(mycachehead)) / sizeof(mypagehead);

	uintptr_t pagehead_addr = s_cast(uintptr_t, CM->cachehead + 1);
	CM->headmem = r_cast(mypagehead*, pagehead_addr);

	MY_ASSERT(CM->pagemem == NULL);
	CM->pagemem = mymem_reserve(size);
	CM->page_cnt = 0u;

	for (int i = 0; i < POOL_SIZE_MAX; i++) {
		CM->cachehead->SH[i].poolidx = i;
		CM->cachehead->SH[i].poolsize = getpoolsize(i);
	}
}

extern void mycachemem_commit(mycachehead* CH) {
}
static void mypagehead_init(mysizehead* SH) {
	mypagehead* PH = SH->curr;
	memset(PH, 0, sizeof(mypagehead));
	PH->poolidx = SH->poolidx;
	PH->poolsize = SH->poolsize;
	PH->pagetag = PAGE_TAG_USING;
	PH->elem_total = PAGE_SIZE / SH->poolsize;
	MY_LOG_TEST("NEW POOL PAGEHEAD size:%u (idx:%u, elem:%u)", PH->poolsize, PH->poolidx, PH->elem_total);
}
void* mypool_newpage(mysizehead* SH) {
	mycachemem* CM = &g_cachemem;
	if (CM->page_cnt == CM->commit_page_cnt) {
		MY_LOG_TEST("COMMIT! 4KB +256KB (head, Page cnt+=64)");
		void* ptr = mymem_commit(CM->headmem + CM->commit_page_cnt, PAGE_SIZE);
		MY_ASSERT_RETURN(ptr, NULL);
		CM->commit_page_cnt += PAGE_SIZE / sizeof(mypagehead);
		CM->pagemem = mymem_commit(CM->pagemem, CM->commit_page_cnt * PAGE_SIZE);
		MY_ASSERT_RETURN(CM->pagemem, NULL);
	}
	else if (CM->page_cnt == 0u) {
		CM->pagemem = mymem_commit(CM->pagemem, CM->commit_page_cnt * PAGE_SIZE);
		//if(CM->pagemem)
		MY_ASSERT_RETURN(CM->pagemem, NULL);
	}
	SH->curr = CM->headmem + CM->page_cnt++;

	mypagehead_init(SH);
	return SH->curr;
}
void* mypool_freelist_get(mypagehead* PH, mypagedata* PD) {
	PH->elem_empty -= 1u;
	MY_ASSERT(PH->nextoff);
	myfreelist* ptr = r_cast(myfreelist*, &PD->data[(PH->nextoff - 1)]);
	PH->nextoff = ptr->nextoff;
	return ptr;
}
void* mypool_freelist_push(mypagehead* PH, void* ptr) {
}
//void mypool_freepage_get(mycachemem* CM, mysizehead* SH) {
//	SH->curr = CM->freepage;
//	SH->curr->pagetag = PAGE_TAG_USING;
//	CM->freepage = CM->freepage->nextPH;
//	CM->freepage->nextPH
//}
//void mypool_freepage_push(mycachemem* CM, mysizehead* SH) {
//	SH->curr = CM->freepage;
//	SH->curr->pagetag = PAGE_TAG_USING;
//	CM->freepage = CM->freepage->nextPH;
//	CM->freepage->nextPH
//}
//4MB = 4KB * 1024 + head(64KB)
void mypool_new(mysize_t page_threshold) {
	mycachemem_new(page_threshold);
}
void* mypool_alloc(mysize_t len) {
	mycachemem* CM = &g_cachemem;
	MY_ASSERT_RETURN(len > 0u && len < _4KB, NULL);
	uint32_t idx = getpoolidx(len);
	uint32_t size = getpoolsize(idx); //(8 << (idx / 2)) + (4 << (idx / 2)) * (idx & 1);
	mysizehead* SH = &CM->cachehead->SH[idx];
	mypagedata* PD = NULL;
	MY_LOG_TEST("size %u: poolidx(%d, %u)", len, idx, size);

	if (SH->curr == NULL) {
		if (mypool_newpage(SH) == NULL) return NULL;
	}

	mypagehead* PH = SH->curr;
	PD = (CM->pagemem + (SH->curr - CM->headmem));
	//cnt
	if (PH->elem_empty) {
		void* ptr = mypool_freelist_get(PH, PD);
		MY_LOG_TEST("FREELIST , PAGE:%u, ELEM_NO : %u", PH - CM->headmem, ((uintptr_t)ptr - (uintptr_t)PD) / size);
		return ptr;
	}
	else if (PH->elem_cnt < PH->elem_total) {
		MY_LOG_TEST("CNT++, PAGE:%u, ELEM_NO : %u", PH - CM->headmem, PH->elem_cnt);
		return &PD->data[size * PH->elem_cnt++];
	}

	PH->pagetag = PAGE_TAG_FULL;
	if (SH->hole) {
		MY_LOG_TEST("PAGE FULL : %X(page no.%u) , FIND HOLE %X(page no.%u)",
					PH, get_pageno_PH(PH), SH->hole, get_pageno_PH(SH->hole));
		SH->curr = SH->hole;
		SH->hole = SH->hole->nextPH;
		PH = SH->curr;
		PH->pagetag = PAGE_TAG_USING;
	}
	else if (false&&CM->freepage) {
		//mypool_freepage_get(CM,SH);
	}
	else {
		const mypagehead* pPH = PH;
		
		//freepage get
		PH = mypool_newpage(SH);
		MY_LOG_TEST("PAGE FULL : %X(page no.%u), NEW Page %X(page no.%u)",
					pPH, (pPH - CM->headmem), PH, (PH - CM->headmem));
	}
	PD = (CM->pagemem + (PH - CM->headmem));

	if (PH->elem_cnt < PH->elem_total) {
		MY_LOG_TEST("CNT++");
		return &PD->data[size * PH->elem_cnt++];
	}
	else if (PH->elem_empty) {
		MY_LOG_TEST("EMPTY++");
		return mypool_freelist_get(PH, PD);
	}
	return NULL;
}
void mypool_alloc_by() {

}
void mypool_free(void* ptr) {
	//ptr -> PagePtr
	//PagePtr -> Poolsize
	//elem_no
	mycachemem* CM = &g_cachemem;
	const uintptr_t elem_PH_off = s_cast(uintptr_t, ptr) - s_cast(uintptr_t, CM->pagemem);
	const uintptr_t page_no = elem_PH_off / PAGE_SIZE;
	
	mypagehead* PH = &CM->headmem[page_no];
	MY_ASSERT(PH->elem_empty < PH->elem_total);
	const uintptr_t elemoff = (s_cast(uintptr_t, ptr) % PAGE_SIZE);


	PH->elem_empty += 1u;

	//HOLE
	//if (PH->elem_empty == PH->elem_total) {
	//	if (PH->prevPH) {
	//
	//	}
	//	if (PH->prevPHidx) {
	//		(CM->headmem + (PH->prevPHidx - 1))->nextPH = PH->nextPH;
	//		PH->prevPHidx = 0u;
	//	}
	//	if (PH->nextPH) {
	//		PH->nextPH->prev
	//		(CM->headmem + (PH->prevPHidx - 1))->nextPH = PH->nextPH;
	//		PH->prevPHidx = 0u;
	//	}
	//	PH->nextPH = CM->freepage;
	//	CM->freepage = PH;
	//}
	//if (PH->pagetag == PAGE_TAG_FULL) {
	//	mypagehead** const holePH = &CM->cachehead->SH[PH->poolidx].hole;
	//	PH->nextPH = *holePH;
	//	PH->prevPHidx = 0u;
	//	(*holePH)->prevPHidx = (PH - g_cachemem.headmem) + 1u;
	//	*holePH = PH;
	//}
	
	MY_ASSERT((s_cast(uintptr_t, ptr) % PAGE_SIZE) % (PH->poolsize) == 0);
	r_cast(myfreelist*, ptr)->nextoff = PH->nextoff;
	PH->nextoff = elemoff + 1u;
	MY_LOG_TEST("FREE %X (Page no.%u, elem no.%u)", ptr, 
				page_no, 
				((uintptr_t)(ptr) - (uintptr_t)(&CM->pagemem[page_no])) / getpoolsize(PH->poolidx));
}
void mypool_free_by() {

}
void mypool_destroy() {
	mycachemem* CM = &g_cachemem;
	mymem_release(CM->cachehead, CM->commit_page_cnt * sizeof(mypagehead) + sizeof(mycachehead));
	mymem_release(CM->pagemem, CM->page_cnt * PAGE_SIZE);
	memset(CM, 0, sizeof(mycachemem));
}
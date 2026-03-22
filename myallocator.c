
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
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
	mysize_t padding = ((append_len - 1) / sizeof(uint64_t) + 1) * sizeof(uint64_t);
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
		MY_ASSERT(mymem_commit(new_arena, new_size) != NULL);
		
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
void myarena_reset(myarena* alc) {
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

	if ((arena_ptr == (uint8_t*)p + old_capa) && (new_capa <= alc->current->capa)) {
		return arena_ptr;
	}
	return myarena_alloc(alc, new_capa);
}
/*pool allocator*/

void mypoolmanager_new(mypoolmanager* pool_mng) {
}
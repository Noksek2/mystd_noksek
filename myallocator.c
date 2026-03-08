
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.1.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "myallocator.h"

#ifdef _DEBUG
size_t g_id = 0u;
#endif

static INLINE myarena* _arena_alloc(mysize_t size) {
	myarena* mem;
#if MY_OS_WINDOWS
	mem = (myarena*)VirtualAlloc(
		NULL,
		size + sizeof(myarena),
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	MY_ASSERT(mem != NULL);
#elif MY_OS_LINUX
	mem = (myarena*)mmap(
		NULL,                   
		size + sizeof(myarena),
		PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	MY_ASSERT(mem != MAP_FAILED);
#else
	mem = (myarena*)calloc(size, 1);
	MY_ASSERT(mem != NULL);
#endif
	return mem;
}
void allocator_new(myallocator* alc, mysize_t arena_size) {
	myarena* arena = _arena_alloc(arena_size);
	arena->capa = arena_size;
	alc->arena_size = arena_size;
	alc->current = arena;
	alc->head = arena;

#ifdef _DEBUG
		alc->head->id = g_id++;
#endif
	MY_LOG_INFO("new allocator [%llX](%d)", (size_t)alc->head, arena_size);
}

void* allocator_alloc(myallocator* alc, mysize_t append_len) {
	/*
	1. just move pointer (arena->len + append_len <= arena->capa)
	2-1. next ptr (=<next->capa)
	2-2. new ptr (new_arena->next=arena->next, arena->next=new_arena)

	*/
	MY_ASSERT(append_len != 0);

	void* ptr = NULL;
	myarena* arena = alc->current;
	mysize_t padding = ((append_len - 1) / sizeof(size_t) + 1) * sizeof(size_t);
	//MY_LOG_INFO("try id:%llu [%llX] alloc(%d,%d)... ", arena->id, (size_t)arena, append_len, padding, arena->capa);

	if (arena->len + append_len <= arena->capa) {

		MY_LOG_INFO("Done [%llX]", (size_t)arena->ptr + arena->len);

		ptr = arena->ptr + arena->len;
		arena->len += padding;
	}
	else if (arena->next != NULL) {
		if (append_len <= arena->next->capa) {
			arena->next->len = 0u;
			alc->current = arena->next;
			ptr = alc->current->ptr;

			MY_LOG_INFO("Enable next block id:%llu [%llX]", alc->current->id, alc->current);

		}
	}
	//arena->next ==0 || append_len > arena->next->capa 
	if (ptr==NULL) {
		mysize_t new_size = max(alc->arena_size, padding);
		myarena* new_arena = _arena_alloc(new_size);
		new_arena->len = padding;
		new_arena->capa = new_size;
		new_arena->next = arena->next;
		arena->next = new_arena;
		alc->current = new_arena;
		ptr = alc->current->ptr;
#ifdef _DEBUG
		alc->current->id = g_id++;
#endif
		MY_LOG_INFO("full! new allocator id:%llu [%llX], Done [%llX]", alc->current->id, (size_t)alc->current, (size_t)alc->current->ptr);
	}
	return ptr;
	//arena_alloc(alc->head, len);
}
void allocator_reset(myallocator* alc) {
	alc->head->len = 0;
	alc->current = alc->head;
}
void allocator_rewind(myallocator* alc, myarena_check* checkpoint) {
#if MY_LOG_LEVEL >= MY_LOG_LEVEL_INFO
	printf("before : ");

	for (myarena* head = alc->head; head != 0; head = head->next) {

		printf("[%llu:%u/%u] ", head->id, head->len, head->capa);

	}
	puts("");
#endif

	alc->current = checkpoint->tail;
	alc->current->len = checkpoint->len;

#if MY_LOG_LEVEL >= MY_LOG_LEVEL_INFO
	printf("rewind : ");
	for (myarena* head = alc->head; head != 0; head = head->next) {
		printf("[%llu:%u/%u] ", head->id, head->len, head->capa);
	}
	puts("");
#endif
}
void allocator_free(myallocator* alc) {
	myarena* next = 0;
	myarena* now = alc->head;
	while (now != NULL) {
		next = now->next;
		MY_LOG_INFO("free block id:%llu [%llX]..", now->id, (size_t)now);

#if MY_OS_WINDOWS
		VirtualFree(now, 0, MEM_RELEASE);
#elif MY_OS_LINUX
		munmap(now, now->capa + sizeof(myarena));
#else
		free(now);
#endif
		now = next;
	}
	alc->head = 0;
	alc->current = 0;
}

void allocator_check(myallocator* alc, myarena_check* checkpoint) {
	myarena_check ch = {
		.head = alc->head,
		.tail = alc->current,
		.len = alc->current->len,
	};
	*checkpoint = ch;
}
void* allocator_realloc(myallocator* alc, void* p, mysize_t old_capa, mysize_t new_capa) {
	uint8_t* arena_ptr = (alc->current->ptr + alc->current->len);

	if ((arena_ptr == (uint8_t*)p + old_capa) && (new_capa <= alc->current->capa)) {
		return arena_ptr;
	}
	return allocator_alloc(alc, new_capa);
}

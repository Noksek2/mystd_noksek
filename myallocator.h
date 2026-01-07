
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.1.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYALLOCATOR_H__
#define __MYALLOCATOR_H__

#include "mylog.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if MY_OS_WIN
#include <Windows.h>
#elif MY_OS_LINUX
#include <sys/mman.h>
#endif

#define ARENA_1MB (1024*1024-sizeof(myarena))
#define ARENA_4MB (1024*1024*4-sizeof(myarena))

#define ALLOCATOR_ALLOC(ALC, T, SZ) (T*)allocator_alloc(ALC,sizeof(T)*(SZ))


#ifndef max
#define max(A,B) ((A>B)?(A):(B))
#endif
#ifdef _DEBUG
extern size_t g_id;
#endif
typedef uint32_t mysize_t;
typedef struct myarena {
	struct myarena* next;
#ifdef _DEBUG
	size_t id;
#endif
	mysize_t len;
	mysize_t capa;
	uint8_t ptr[];
}myarena;

typedef struct myarena_check {
	myarena* tail;
	myarena* head;
	mysize_t len;
}myarena_check;

typedef struct myallocator {
	myarena* head;
	myarena* current;
	mysize_t arena_size;
}myallocator;

MY_EXTERN_START
	extern void allocator_new(myallocator* alc, mysize_t arena_size);
	extern void* allocator_alloc(myallocator* alc, mysize_t len);
	extern void allocator_free(myallocator* alc);
	extern void allocator_reset(myallocator* alc);
	extern void allocator_check(myallocator* alc, myarena_check* checkpoint);
	extern void allocator_rewind(myallocator* alc, myarena_check* checkpoint);
	extern void* allocator_realloc(myallocator* alc, void* p, mysize_t old_capa, mysize_t new_capa);

#ifdef __USE_CLEAN__
	//not yet
	//extern void allocator_clean(myallocator* alc,myarena_check* checkpoint);
#endif
MY_EXTERN_END

#endif
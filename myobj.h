
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYOBJ_H__
#define __MYOBJ_H__

#include "mystd.h"
typedef struct mylist mylist;
typedef struct mypair mypair;
typedef struct mystr mystr;
typedef struct myvalue myvalue;
typedef struct myhashmap myhashmap;

typedef struct mycontext {
	void* (*alloc)(uint32_t);
	void (*free)(void*);
} mycontext;


enum{
	HASH_SKIP=1,
};
struct mystr {
	char* ptr;
	uint32_t hash;
	uint32_t len;
};
struct myvalue {
	union {
		int32_t i32;
		uint64_t u;
		int64_t i;
		float f32;
		double f;
		struct mystr* str;
		struct myobj* obj;
		void* p;
	};
};
struct mypair {
	union {
		struct mystr* key;
		struct myvalue key_v;
	};
	struct myvalue val;
};

struct mylist {
	struct myvalue* val;
	uint32_t len;
	uint32_t capa;
};
struct myhashmap {
	struct mypair* pairs;
	uint32_t len;
	uint32_t capa;
	uint32_t empty;
	uint32_t _;
};

static uint32_t make_hash_u8(const char* key, uint32_t length) {
	uint32_t hash = 2166136261u; // FNV offset basis
	for (uint32_t i = 0; i < length; i++) {
		hash ^= key[i];
		hash *= 16777619; // FNV prime
	}
	return hash;
}

//mystr
//mylist
//myhashmap 
MY_EXTERN_START;

typedef enum {
	MYOBJ_DEFAULT = 0,
	MBOBJ_NO_CUSTOM_ALLOC = 1,
} myobj_flag;

extern mycontext g_ctx;
static void myobj_init_ctx(const mycontext* ctx) {
	g_ctx = *ctx;
}
static void myobj_init(myobj_flag flag) {
	mystd_init();
	mycontext ctx;
	if (flag == MYOBJ_DEFAULT) {
		ctx.alloc = mypool_alloc;
		ctx.free = mypool_free;
	}
	else {
		ctx.alloc = malloc;
	}
	myobj_init_ctx(&ctx);
}
static void myobj_destroy() {
	mystd_destroy();
}
//mystr


//mystr
static mystr* mystr_new(const char* buf) {
	mystr* str;
	const uint32_t len = strlen(buf);
	str = (mystr*)g_ctx.alloc(sizeof(mystr));
	MY_ASSERT_RETURN(str, NULL);
	str->ptr = (char*)g_ctx.alloc(len);
	memmove(str->ptr, buf, len);
	str->hash = make_hash_u8(buf, len);
	str->len = len;
	return str;
}
static void mystr_destroy(mystr* str) {
	MY_ASSERT(str);
	g_ctx.free(str);
	MY_ASSERT(str->ptr);
	g_ctx.free(str->ptr);
}



// mylist
static mylist mylist_new(uint32_t capa, uint32_t _len) {
	mylist list;
	list.capa = capa;
	list.len = _len;
	list.val = (myvalue*)g_ctx.alloc(capa * sizeof(myvalue));
	MY_ASSERT_RETURN(list.val, list);
	return list;
}
static mylist mylist_push(mylist* list, myvalue val){
	MY_ASSERT(list->len < list->capa);
	list->val[list->len++]=val;
}
static myvalue mylist_pop(mylist* list){
	MY_ASSERT(list->len);
	return list->val[--list->len];
}
static void mylist_destroy(mylist* list) {
	MY_ASSERT(list->val);
	g_ctx.free(list->val);
}

static boolean myhashmap_is_empty(const mypair* pair) {
	return (pair->key == NULL);
}
static boolean myhashmap_is_dummy(const mypair* pair) {
	return ((uintptr_t)pair->key == (uintptr_t)HASH_SKIP);
}
//myhashmap
static myhashmap myhashmap_new(uint32_t capa) {
	myhashmap map;
	map.capa = capa;
	map.len = 0u;
	map.pairs = (mypair*)g_ctx.alloc(capa * sizeof(mypair));
	MY_ASSERT(map.pairs);
	return map;
}
static mypair* myhashmap_find(myhashmap* map, mystr* key) {
	MY_ASSERT(key);
	uint32_t hash = (key->hash);
	const uint32_t prev_idx = hash % map->capa;
	uint32_t idx = prev_idx;
	mypair* pairs;
	while(true){
		pairs = &map->pairs[idx];
		if(pairs->key==NULL) break;
		else if(pairs->key->hash == hash){
			if(pairs->key == key)
				return pairs;
		}
		idx += 1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) break;
	}
	return NULL;
}

//can add duplicating? data

static uint32_t myhashmap_add(myhashmap* map, mypair pair) {
	MY_ASSERT(map->len < map->capa);
	uint32_t hash = (pair.key->hash);
	const uint32_t prev_idx = hash % map->capa;
	uint32_t idx = prev_idx;
	mypair* pairs;
	while(true){
		pairs = &map->pairs[idx];
		if(pairs->key == NULL){
			*pairs = pair;
			break;
		}
		else if(pairs->key->hash == hash){
			if(pairs->key == pair.key)
				MY_ASSERT_RETURN("DUPLICATED!!",);
		}
		idx+=1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) {
			MY_ASSERT("HASHMAP EMPTY LIST NOT FOUND");
			break;
		}
	}
	map->len++;
	return idx;
}
static uint32_t myhashmap_erase(myhashmap* map, mystr* key) {
	MY_ASSERT(map->len < map->capa);
	uint32_t hash = (key->hash);
	const uint32_t prev_idx = hash % map->capa;
	uint32_t idx = prev_idx;
	mypair* pairs;
	while(true){
		pairs = &map->pairs[idx];
		if(pairs->key == NULL){
			MY_ASSERT("Not found");
		}
		else if(pairs->key->hash == hash){
			if (pairs->key == key) {
				pairs->key = (mystr*)HASH_SKIP;
				break;
			}
		}
		idx+=1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) {
			MY_ASSERT("HASHMAP EMPTY LIST NOT FOUND");
			break;
		}
	}
	map->len++;
	return idx;
}

static void myhashmap_destroy(myhashmap* map) {
	uint32_t cnt=0u;
	for(uint32_t i=0u; i < map->capa; i++){
		if(map->pairs){
			g_ctx.free(map->pairs);
			map->pairs=NULL;
			cnt+=1u;
		}
	}
}

static void myvalue_print(myvalue v) {
	printf("%llX", v.u);
}
static void myprintf(const char* buf,...) {
	va_list v;
	va_start(v, buf);
	char* pbuf = (char*)buf;
	myvalue val;

	while (*pbuf != '\0') {
		if (*pbuf == '%') {
			pbuf++;
			switch (*pbuf) {
			case 'v': 
				val = va_arg(v, myvalue);
				myvalue_print(val);
				break;
			case 'd':
				printf("%d", va_arg(v, int));
				break;
			case 's':
				printf("%s", va_arg(v, const char*));
				break;
			default:
				MY_ASSERT(0);
				goto redo;
			}
			
		}
		else {
			putchar(*pbuf);
		}
		pbuf++;
	redo:;
	}

}
MY_EXTERN_END;
/*

mylist list=mylist_new(32u,0u);
list.
myhashmap map = myhashmap_new(24)


g_strpool;
"ABCDEF"
"BMFOG"
"CEFDG"
"XMCVOKSDL"

*/
#endif
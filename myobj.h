
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "mystd.h"
typedef struct mystack mystack;
typedef struct mypair mypair;
typedef struct mystr mystr;
typedef struct myvalue myvalue;

enum{
	HASH_SKIP=1,
};
struct mystr {
	uint32_t hash;
	uint32_t len;
	uint8_t ptr[];
};
struct myobj {
	uint32_t hash;
	uint32_t len;
	uint8_t ptr[];
};
struct myvalue {
	union {
		uint64_t u;
		int64_t i;
		double f;
		struct mystr* str;
		struct myobj* obj;
	};
};
struct mypair {
	union{
		struct mystr* key;
		struct myvalue key_v;
	}
	struct myvalue val;
};

struct mystack {
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
//mystack
//myhashmap 
MY_EXTERN_START;

//mystr


//mystr
static mystr* mystr_new(const char* buf) {
	mystr* str;
	const uint32_t len = strlen(buf) - 1;
	str = mypool_alloc(len + sizeof(mystr));
	MY_ASSERT_RETURN(str, NULL);
	str->hash = make_hash_u8(buf, len);
	str->len = len;
	return str;
}
static void mystr_destroy(mystr* str) {
	mypool_free(str);
}



// mystack
static mystack mystack_new(uint32_t capa, uint32_t _len) {
	mystack stack;
	stack.capa = capa;
	stack.len = _len;
	stack.val = mypool_alloc(capa * sizeof(myvalue));
	MY_ASSERT_RETURN(stack.val, stack);
	return stack;
}
static mystack mystack_push(mystack* stack, myvalue val){
	MY_ASSERT(stack->len < stack->capa);
	stack->val[stack->len++]=val;
}
static myvalue mystack_pop(mystack* stack){
	MY_ASSERT(stack->len);
	stack->val[--stack->len]=val;
}
static void mystack_destroy(mystack* stack) {
	MY_ASSERT(stack->val);
	mypool_free(stack->val);
}

//myhashmap
static myhashmap myhashmap_new(uint32_t capa) {
	myhashmap map;
	map.capa = capa;
	map.len = 0u;
	map.pairs = mypool_alloc(capa * sizeof(mypair));
	MY_ASSERT(map.pairs);
	return map;
}
static mypair* myhashmap_find(myhashmap* map, mystr* key) {
	MY_ASSERT(key);
	uint32_t hash = (key->hash);
	const uint32_t prev_idx = hash % map->capa;
	const uint32_t idx = prev_idx;
	mypair* pairs;
	while(true){
		pairs = &map->pairs[idx];
		if(pairs->key==NULL) break;
		else if(pairs->key->hash == hash){
			if(pairs->key == key)
				return pairs;
		}
		idx+=1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) break;
	}
	return NULL;
}

//can add duplicating? data

static void myhashmap_add(myhashmap* map, mypair pair) {
	MY_ASSERT(map->len < map->capa);
	uint32_t hash = (pair.str->hash);
	const uint32_t prev_idx = hash % map->capa;
	const uint32_t idx = prev_idx;
	mypair* pairs=&map->pairs[idx];
	while(true){
		if(pairs->key == NULL){
			*pairs = pair;
			break;
		}
		else if(pairs->key->hash == hash){
			if(pairs->key == key)
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
}
static void myhashmap_erase(myhashmap* map, mystr* key) {
	MY_ASSERT(map->len < map->capa);
	uint32_t hash = (pair.str->hash);
	const uint32_t prev_idx = hash % map->capa;
	const uint32_t idx = prev_idx;
	mypair* pairs=&map->pairs[idx];
	while(true){
		if(pairs->key == NULL){
			*pairs = pair;
			break;
		}
		else if(pairs->key->hash == hash){
			if(pairs->key == key)
				*pairs=NULL;
		}
		idx+=1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) {
			MY_ASSERT("HASHMAP EMPTY LIST NOT FOUND");
			break;
		}
	}
	map->len++;
}


static mypair* myhashmap_find_key_v(myhashmap* map, myvalue v) {
	MY_ASSERT(key);
	uint32_t hash = (key->hash);
	const uint32_t prev_idx = hash % map->capa;
	const uint32_t idx = prev_idx;
	mypair* pairs;
	while(true){
		pairs = &map->pairs[idx];
		if(pairs->key_v==NULL) break;
		else if(pairs->key_v->hash == hash){
			if(pairs->key_v == v)
				return pairs;
		}
		idx+=1;
		if(idx==map->capa) idx=0u;
		else if(idx==prev_idx) break;
	}
	return NULL;
}



static void myhashmap_destroy(myhashmap* map) {
	uint32_t cnt=0u;
	for(uint32_t i=0u; i < map->capa; i++){
		if(map->pairs){
			mypool_free(map->pairs);
			map->pairs=NULL;
			cnt+=1u;
		}
	}
}

MY_EXTERN_END;
/*

mystack stack=mystack_new(32u,0u);
stack.
myhashmap map = myhashmap_new(24)


g_strpool;
"ABCDEF"
"BMFOG"
"CEFDG"
"XMCVOKSDL"

*/

/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "mystd.h"
typedef struct mystack mystack;
typedef struct mypair mypair;
typedef struct mystr mystr;
typedef struct myvalue myvalue;

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
	struct mystr* str;
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
};

static uint32_t make_hash_u8(const char* key, uint32_t length) {
	uint32_t hash = 2166136261u; // FNV offset basis
	for (uint32_t i = 0; i < length; i++) {
		hash ^= key[i];
		hash *= 16777619; // FNV prime
	}
	return hash;
}
MY_EXTERN_START;

static mystack mystack_new(uint32_t capa, uint32_t _len) {
	mystack stack;
	stack.capa = capa;
	stack.len = _len;
	stack.val = mypool_alloc(capa);
	MY_ASSERT_RETURN(stack.val, stack);
	return stack;
}
static void mystack_destroy(mystack* stack) {
	mypool_free(stack);
}
static void mystack_destroy(mystack*);
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

MY_EXTERN_END;
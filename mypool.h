
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.1.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#ifndef __MYPOOL_H__
#define __MYPOOL_H__

#include "myallocator.h"

typedef struct mypool mypool;
typedef struct mypool_head mypool_head;
typedef struct mypoolmanager mypoolmanager;

enum {
	POOLMAP_SIZE=32,
	POOL_8B=0,
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
	POOL_16KB,
	POOL_32KB,
	POOL_64KB,
	POOL_128KB,
	POOL_256KB,
	POOL_512KB,
	POOL_1MB,
	POOL_2MB,
	POOL_4MB,

};
struct mypool_head {
	uint32_t len;
	uint8_t ptr[];
};
struct mypool {
	mypool_head* freelist;
};
struct mypoolmanager {
	mypool pool_map[POOLMAP_SIZE];
	uint32_t total_size;
	//uint32_t threshold;
	//8byte : 8byte<<0, 16byte:8byte<<1 ...512byte:8byte<<6 
	// 512KB : 8<<16
};//8
static void poolmanager_init(mypoolmanager* mng) {
	mng->pool_map[POOL_8B].freelist;
}


#endif

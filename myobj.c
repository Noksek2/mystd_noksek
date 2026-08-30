
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.3.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "myobj.h"
mycontext g_ctx;

void myobj_new(uint32_t max_threshold){
	mypool_new(max_threshold);
	MY_ASSERT(g_ctx.alloc == NULL);
	g_ctx.alloc = mypool_alloc;
	MY_ASSERT(g_ctx.free == NULL);
	g_ctx.free = mypool_free;
}
void myobj_init_ctx(const mycontext* ctx){
	memcpy(&g_ctx, ctx, sizeof(mycontext));
}
void myobj_destroy() {
	g_ctx.alloc = NULL;
	g_ctx.free = NULL;
	mypool_destroy(0);
}
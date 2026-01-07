#include "mystd.h"
int main() {
	myallocator alc;
	allocator_new(&alc, 1000);
	for(volatile int i=0;i< 10;i++)
		allocator_alloc(&alc, 5000000);
	allocator_free(&alc);
	mystd_print_system_info();
	return 0;
}
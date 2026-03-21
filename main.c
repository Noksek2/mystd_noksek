#include "mystd.h"
int main() {
	myarena alc;
	arena_new(&alc, 1000);
	for(volatile int i=0;i< 10;i++)
		arena_alloc(&alc, 5000000);
	arena_free(&alc);
	mystd_print_system_info();
	return 0;
}
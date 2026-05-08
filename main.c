#include "mystd.h"
enum { MAXarr = 100001 };
//uint32_t g_slowloop = 0;
//int g_path[5];
int main() {
	int* arr[MAXarr] = { 0, };
	myclock_t clock;
	clock = myclock_setclock();
	mystd_init();
	void* ptr;
	double st, en;
	myarena arena;

	printf("N\tM\tPool\tMalloc\tArena\n");
	for (int N = 1000; N < 12001; N += 5000) {
		for (int pi = POOL_8B; pi < POOL_SIZE_MAX-1; pi++) {
			int M = g_poolsize_map[pi];
			printf("%u\t%u\t", N, M);
			st = myclock_getsec(clock);
			for (int i = 0; i < N; i++) {
				arr[i] = mypool_alloc(g_PM_id, 1, M);
			}
			for (int i = 0; i < N; i++) {
				mypool_free(arr[i]);
			}
			en = myclock_getsec(clock);
			printf("%.4lf\t", en - st);
			mypool_destroy();

			st = myclock_getsec(clock);
			for (int i = 0; i < N; i++) {
				arr[i] = malloc(M);
			}
			for (int i = 0; i < N; i++) {
				free(arr[i]);
			}
			en = myclock_getsec(clock);
			printf("%.4lf\t", en - st);
			

			st = myclock_getsec(clock);
			myarena_new(&arena, _16KB);
			for (int i = 0; i < N; i++) {
				arr[i] = myarena_alloc(&arena, M);
			}
			myarena_free(&arena);
			en = myclock_getsec(clock);
			printf("%.4lf\n", en - st);
			mypool_new(_64MB * 4);
		}
	}
	mystd_destroy();
	return 0;
}
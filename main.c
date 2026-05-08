#include "mystd.h"
enum { MAXarr = 100001 };
//uint32_t g_slowloop = 0;
//int g_path[5];

//random size alloc, free
int test_alloc2() {
	const int N = 100000;
	int* RandSize = malloc(N * sizeof(int));

	void** arr = malloc(MAXarr * sizeof(size_t));

	myclock_t clock;
	clock = myclock_setclock();
	mystd_init();
	void* ptr;
	double st, en;
	myarena arena;

	printf("N\tPool\tMalloc\n");
	//for () {

	//printf("%u\t%u\t", N, M);
	int sum;
	for (int T = 0; T < 10; T++) {
		sum = 0;
		for (int i = 0; i < N; i++) {
			RandSize[i] = 8 << (rand() % (POOL_8B + T + 1));
			sum += RandSize[i];
		}
		printf("%.3lf\t", (double)sum / (double)N);
		st = myclock_getsec(clock);
		for (int i = 0; i < N; i++) {
			arr[i] = mypool_alloc(g_PM_id, 1, RandSize[i]);
		}
		for (int i = 0; i < N; i++) {
			mypool_free(arr[i]);
		}
		en = myclock_getsec(clock);
		printf("%.4lf\t", en - st);
		mypool_destroy();
		st = myclock_getsec(clock);
		for (int i = 0; i < N; i++) {
			arr[i] = malloc(RandSize[i]);
		}
		for (int i = 0; i < N; i++) {
			free(arr[i]);
		}
		en = myclock_getsec(clock);
		printf("%.4lf\n", en - st);
		mypool_new(_64MB * 4);
	}
	/*
		st = myclock_getsec(clock);
		myarena_new(&arena, _16KB);
		for (int i = 0; i < N; i++) {
			arr[i] = myarena_alloc(&arena, M);
		}
		myarena_free(&arena);
		en = myclock_getsec(clock);
		printf("%.4lf\n", en - st);
		mypool_new(_64MB * 4);
	*/
	//}
	free(arr);
	mystd_destroy();
	return 0;
}
int test_alloc1()
{
	int* arr[MAXarr] = { 0, };
	myclock_t clock;
	clock = myclock_setclock();
	mystd_init();
	void* ptr;
	double st, en;
	myarena arena;

	printf("N\tM\tPool\tMalloc\tArena\n");
	for (int N = 1000; N < 12001; N += 5000) {
		for (int pi = POOL_SIZE_MAX - 2; pi <= POOL_SIZE_MAX - 1; pi++) {
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
}

int main() {
	test_alloc2();
	return 0;
}
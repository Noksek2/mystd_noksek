#include "mystd.h"
#include <unistd.h>
enum { MAXarr = 100001 };
//uint32_t g_slowloop = 0;
//int g_path[5];
int main() {
	double st, en;
	myclock_t clock;
	int* arr[MAXarr] = { 0, };
	clock = myclock_setclock();
	mystd_init();
	void* ptr;
	//double st, en;

	printf("N\tM\tPool\tMalloc\n");
	for (int N = 1000; N < 20001; N += 5000) {
		for (int M = 8; M < _4KB; M <<= 1) {
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
			printf("%.4lf\n", en - st);
			mypool_new(_64MB*4);
		}
	}
	mystd_destroy();
	return 0;
}
#include "mystd.h"
void sample_clock() {
	puts("=== myclock ===");
	myclock_t timer,lst,lend;
	double time, time2;
	timer = myclock_setclock();
	time = myclock_getsec(timer);
	
	for (volatile int i = 0; i < 1000000; i++) {
		if (i % 10000 == 0)printf("Wek ");
		if (i % 15000 == 0)printf("Wak ");
	}
	puts("");
	time2 = myclock_getsec(timer);
	printf("time : %.6lf\n\n", time2 - time);
}

void sample_arena() {
	puts("=== myarena ===");
	myarena alc;
	myarena_new(&alc, 1000000);
	for(volatile int i=0;i< 10;i++)
		myarena_alloc(&alc, 5000000);
	myarena_free(&alc);
}
int main() {
	myclock_t timer, t1, t2;

	timer=myclock_setclock();
	t1=myclock_getsec(timer);

	puts("=== mystd example code ===");	
	mystd_print_system_info();
	sample_clock();
	sample_arena();

	t2=myclock_getsec(timer);
	printf("t1:%.6lf, t2:%.6lf\n", t1, t2); 
	printf("nano:%d\n", myclock_getnanosec(0));
	printf("progress time %.8lf (sec)\n", t2-t1);
	return 0;
}

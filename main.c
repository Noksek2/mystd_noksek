#include "mystd.h"
enum { MAXarr = 100001, MAXSIZE=10000 };
//uint32_t g_slowloop = 0;
//int g_path[5];

int test_slowpool1();
int test_slowpool2();
void test_pool1();
void test_pool2();
void test_alloc_main();
void print_binary(const uint32_t num) {
    uint32_t mask = 0x80000000;
    const uint32_t pmask = 0x11111111;
    char buf[41];
    char* p = buf;
    while (mask) {
        *p = (mask & num) ? '1' : '0'; p++;
        if (mask & pmask) { *p = ' '; p++; }
        mask >>= 1;
    }
    *p = '\0';
    puts(buf);
}

void pushpop() {
    const int N = 10000;
    void** arr = malloc(N * sizeof(size_t));
    mypool_new(_4MB);
    uint32_t len = getpoolsize(POOL_SIZE_MAX - 1u);
    double st, en;
    myclock_t clock;
    clock = myclock_setclock();
    st = myclock_getsec(clock);

    arr[0] = mypool_alloc(len);
        arr[1] = mypool_alloc(len);
    for (int i = 0; i < N; i++) {

       // mypool_dump();
        arr[2] = mypool_alloc(len);
        arr[3] = mypool_alloc(len);
        //mypool_dump();
        mypool_free(arr[0]);
        mypool_free(arr[1]);
        //mypool_dump();
        arr[0] = mypool_alloc(len);
        arr[1] = mypool_alloc(len);
       // mypool_dump();
        mypool_free(arr[2]);
        mypool_free(arr[3]);
        //mypool_dump();
    }
   
    en = myclock_getsec(clock);
    //mypool_dump();
    printf("%.4lf\t", en - st);

    st = myclock_getsec(clock);
    mypool_destroy();

    arr[0] = malloc(len);
    arr[1] = malloc(len);
    for (int i = 0; i < N; i++) {

        // mypool_dump();
        arr[2] = malloc(len);
        arr[3] = malloc(len);
        //mypool_dump();
        free(arr[0]);
        free(arr[1]);
        //mypool_dump();
        arr[0] = malloc(len);
        arr[1] = malloc(len);
        // mypool_dump();
        free(arr[2]);
        free(arr[3]);
    }
    en = myclock_getsec(clock);
    printf("%.4lf\t\n", en - st);
}

void test_alloc_main()
{
    int* arr[MAXarr] = { 0, };
    myclock_t clock;
    clock = myclock_setclock();
    mystd_init();
    myslowpool_destroy();
    void* ptr;
    double st, en;
    myarena arena;

    printf("N\tM\tPool\tMalloc\tArena\n");
   
    for (int N = 1000; N < 12001; N += 5000) {
        for (int pi = POOL_8B; pi < POOL_SIZE_MAX; pi++) {
            myslowpool_new(_64MB);
            //int M = g_poolsize_map[pi];
            int M = getpoolsize(pi);
            printf("%u\t%u\t", N, M);
            st = myclock_getsec(clock);
            for (int i = 0; i < N; i++) {
                arr[i] = myslowpool_alloc(g_PM_id, 1, M);
            }
            for (int i = 0; i < N; i++) {
                myslowpool_free(arr[i]);
            }
            en = myclock_getsec(clock);
            printf("%.4lf\t", en - st);
            myslowpool_destroy();
            mypool_new(_64MB);
            st = myclock_getsec(clock);
            for (int i = 0; i < N; i++) {
                arr[i] = mypool_alloc(M);
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
        }
    }
    mystd_destroy();
}
void test_pool2()
{
    const int N = 10000;
    int* RandSize = malloc(N * sizeof(int));

    void** arr = malloc(MAXarr * sizeof(size_t));

    myclock_t clock;
    clock = myclock_setclock();
    mystd_init();
    myslowpool_destroy();
    void* ptr;
    double st, en;
    myarena arena;

    printf("N\tMalloc\tNewPool\n");
    //for () {

    //printf("%u\t%u\t", N, M);
    int sum;
    for (int T = 0; T < POOL_SIZE_MAX; T++) {
        sum = 0;
        for (int i = 0; i < N; i++) {
            RandSize[i] = getpoolsize(rand() % (T + 1));
            sum += RandSize[i];
        }
        printf("%.3lf\t", (double)sum / (double)N);
        //myslowpool_new(_64MB * 4);
        //st = myclock_getsec(clock);
        //for (int i = 0; i < N; i++) {
        //    arr[i] = myslowpool_alloc(g_PM_id, 1, RandSize[i]);
        //}
        //for (int i = 0; i < N; i++) {
        //    myslowpool_free(arr[i]);
        //}
        //en = myclock_getsec(clock);
        //printf("%.4lf\t", en - st);
        //myslowpool_destroy();


        st = myclock_getsec(clock);
        for (int i = 0; i < N; i++) {
            arr[i] = malloc(RandSize[i]);
        }
        for (int i = 0; i < N; i++) {
            free(arr[i]);
        }
        en = myclock_getsec(clock);
        printf("%.4lf\t", en - st);

        mypool_new(_64MB*2);

        st = myclock_getsec(clock);
        for (int i = 0; i < N; i++) {
            arr[i] = mypool_alloc(RandSize[i]);
        }
        for (int i = 0; i < N; i++) {
            mypool_free(arr[i]);
        }
        en = myclock_getsec(clock);
        printf("%.4lf\n", en - st);
        
        mypool_destroy();
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
}

void test_pool1()
{
    mypool_new(_64MB);
    uint32_t msb, idx;
    void* ptr[MAXSIZE];
    clock_t st, en;
    st = clock();
    for (int i = 5; i < POOL_SIZE_MAX; i++) {
        for (int j = 0; j < MAXSIZE; j++) {
            ptr[j] = mypool_alloc(getpoolsize(i));
        }
        // msb = find_msb32_idx(i);
        // printf("%5d, ", i+1);
        // print_binary(0b11 << (msb));
        for (int j = 0; j < MAXSIZE; j++) {
            mypool_free(ptr[j]);
        }
    }
    en = clock();
    printf("time : %d", en - st);

    st = clock();
    for (int i = 5; i < POOL_SIZE_MAX; i++) {
        for (int j = 0; j < MAXSIZE; j++) {
            ptr[j] = malloc(getpoolsize(i));
        }
        // msb = find_msb32_idx(i);
        // printf("%5d, ", i+1);
        // print_binary(0b11 << (msb));
        for (int j = 0; j < MAXSIZE; j++) {
            free(ptr[j]);
        }
    }
    en = clock();
    printf("time : %d", en - st);
    mypool_destroy();
}


//random size alloc, free
int test_slowpool2() {
    const int N = 10000;
    int* RandSize = malloc(N * sizeof(int));

    void** arr = malloc(MAXarr * sizeof(size_t));

    myclock_t clock;
    clock = myclock_setclock();
    mystd_init();
	g_PM_id = myslowpool_new(_64MB*4);
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
            arr[i] = myslowpool_alloc(g_PM_id, 1, RandSize[i]);
        }
        for (int i = 0; i < N; i++) {
            myslowpool_free(arr[i]);
        }
        en = myclock_getsec(clock);
        printf("%.4lf\t", en - st);
        myslowpool_destroy();
        st = myclock_getsec(clock);
        for (int i = 0; i < N; i++) {
            arr[i] = malloc(RandSize[i]);
        }
        for (int i = 0; i < N; i++) {
            free(arr[i]);
        }
        en = myclock_getsec(clock);
        printf("%.4lf\n", en - st);
        myslowpool_new(_64MB * 4);
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
int test_slowpool1()
{
    int* arr[MAXarr] = { 0, };
    myclock_t clock;
    clock = myclock_setclock();
    mystd_init();
	g_PM_id = myslowpool_new(_64MB*4);
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
                arr[i] = myslowpool_alloc(g_PM_id, 1, M);
            }
            for (int i = 0; i < N; i++) {
                myslowpool_free(arr[i]);
            }
            en = myclock_getsec(clock);
            printf("%.4lf\t", en - st);
            myslowpool_destroy();

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
            myslowpool_new(_64MB * 4);
        }
    }
    mystd_destroy();
}

int main(){
	test_alloc_main();
}

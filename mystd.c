//260427-1
/* Apache License 2.0 (See LICENSE file for details)
https://github.com/Noksek2/mystd_noksek Noksek2 v0.2.0
Is there any problem in this code, Please use Issues for bug reports.
*/

#include "mystd.h"
mypoolmanager g_poolmng;
mymutex g_lock;
uint8_t g_PM_id;
void mystd_init(void) {
    mymutex_init(&g_lock);
    uint8_t id = mypool_new(_64MB*2);//128MB
    MY_ASSERT(id > 0 && id <= MYCORE_MAX);
    g_PM_id = id;
}
void mystd_destroy(void) {
    mypool_destroy();
    mymutex_destroy(&g_lock);
}
void mystd_print_system_info(void) {
    const char* os_name = "Unknown OS";
    const char* cc_name = "Unknown Compiler";
    // 1. OS 결정
#if MY_OS_WINDOWS
    os_name = "Windows";
#elif MY_OS_LINUX
    os_name = "Linux";
#elif MY_OS_MAC
    os_name = "Apple";
#endif

    // 2. 컴파일러 결정
#if MY_COMPILER_MSVC
    cc_name = "MSVC";
#elif MY_COMPILER_GCC
    cc_name = "GCC";
#elif MY_COMPILER_CLANG
    cc_name = "Clang";
#endif

	puts("--- mystd system info (by Noksek2)---");
	printf("Version\t\t: v%d.%d.%d\n", MYSTD_VERSION_MAJOR, MYSTD_VERSION_MINOR, MYSTD_VERSION_PATCH);
    printf("OS\t\t: %s\n", os_name);
    printf("Compiler\t: %s\n", cc_name);

#if MY_ARCH_X64
    puts("Arch\t\t: x64");
#elif MY_ARCH_ARM
    puts("Arch\t\t: ARM");
#endif
    puts("-------------------------------------");
}

static char printable(char c) {
	if (c == '_' || (c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')) return c;
	return ' ';
}


void myarena_dump(myarena* alc) {
	puts("ARENA DUMP ");
	mypage* pg = alc->head;
	while (pg != NULL) {
		printf("(%u / %u) \n", pg->len, pg->capa);
		for (mysize_t i = 0; i < pg->len; i++) {
			printf("%02X ", pg->ptr[i]);
			if ((i + 1) % 16 == 0) {
				printf("| ");
				for (int j = i - 15; j <= i; j++)
					printf("%c", printable((char)(pg->ptr[j])));
				puts("");
			}
		}

		pg = pg->next;
	}
}

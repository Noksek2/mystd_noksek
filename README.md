# MyStd (by Noksek)
## v0.2.0 (UNSTABLE)
- unstable yet.
## Apache License 2.0
## Function/Macro lists
### mystd.h
```c
/*mystd v0.2.0*/
#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 2
#define MYSTD_VERSION_PATCH 0

extern void  mystd_init();
extern void  mystd_destroy();

typedef uint64_t myclock_t;
```
- `void mystd_print_system_info()`
- `myclock_t myclock_setclock()`
- `double    myclock_getsec(myclock_t timer)`

### myallocator.h

- `void  myarean_new   (myarena* alc, mysize_t arena_size)`
- `void* myarean_alloc (myarena* alc, mysize_t len)`
- `void  myarean_free  (myarena* alc)`
- `void  myarean_reset_all (myarena* alc)`
- `void  myarean_check_new (myarena* alc, myarena_check* checkpoint)`
- `void  myarean_rewind (myarena* alc, myarena_check* checkpoint)`
- `void* myarean_realloc(myarena* alc, void* p, mysize_t old_capa, mysize_t new_capa)`

- `ALLOCATOR_ALLOC(ALC, T, SZ)`


### mylog.h
- `MY_LOG_LEVEL`, `MY_LOG_LEVEL_OFF`, `MY_LOG_LEVEL_ERROR`, `MY_LOG_LEVEL_INFO`
- `MY_BREAK()`
- `MY_ASSERT(cond)`
- `MY_LOG_INFO(fmt, ...)`

### mydef.h
- `MY_COMPILER_MSVC`, `MY_COMPILER_GCC`, `MY_COMPILER_CLANG`, `MY_COMPILER_UNKNOWN`
- `MY_OS_WIN`, `MY_OS_LINUX`, `MY_OS_MAC`, `MY_OS_UNKNOWN`
- `MY_ARCH_X64`, `MY_ARCH_X86`, `MY_ARCH_ARM`
- `INLINE`, `RESTRICT`, `MY_ALIGN(n)`
- `MY_EXTERN_START`, `MY_EXTERN_END`



## SAMPLE (main.c)

```c
void test_alloc1()
{ //bla bla
}
void test_alloc2()
{ //bla bla 
}
int main(int argc, char** argv)
{
	mystd_
	test_alloc2();
	return 0;
}
```

- Linux/gcc
```bash
$ make
$ ./bin/mystd_test
```

- Windows/gcc
```bat
>make.bat
...
>.\bin\mystd_test
```
Executable file `mystd_test` would be created in /bin/ directory

sorry for no msvc yet

###

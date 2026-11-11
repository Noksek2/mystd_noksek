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


- macros

| name | type | value | explain | where |
| --- | --- | --- | --- | --- |
| MYSTD_VERSION_MAJOR | int | 0 | ver major | mystd.h |
| MYSTD_VERSION_MINOR | int | 2 | ver minor | mystd.h |
| MYSTD_VERSION_PATCH | int | 0 | ver patch | mystd.h |
| MY_COMPILER_MSVC | int | 0(else), 1 (if msvc) | msvc compiler | mydef.h |
| MY_COMPILER_GCC | int | 0(else), 1 (if gcc) | gcc compiler | mydef.h |
| MY_COMPILER_CLANG | int | 0(else), 1 (if clang) | clang compiler | mydef.h |
| MY_COMPILER_UNKNOWN | int | 0(msvc, gcc, clang), 1(else) | unknown compiler | mydef.h |
| MY_OS_WINDOWS<br> MY_OS_WIN | int | 0(else), 1(windows) | windows OS | mydef.h |
| MY_OS_LINUX | int | 0(else), 1(linux) | LINUX(Debian, etc) OS | mydef.h |
| MY_OS_MAC | int | 0(else), 1(mac) | MAC(darwin) OS | mydef.h |
| MY_OS_UNKNOWN | int | 0(default os), 1(else) | Unknown OS | mydef.h |
| MY_ARCH_X64 | int | 0, 1(x86_64, AMD64) | x86_64, AMD64 arch | mydef.h |
| MY_ARCH_X86 | int | 0, 1(x86) | x86 arch | mydef.h |
| MY_ARCH_ARM | int | 0, 1(ARM) | ARM arch | mydef.h |
| MY_EXTERN_START <br> MY_EXTERN_END | - | ` #if _cpluspplus ... extern "C" ` | when C++ | mydef.h | 

### mydef.h
- `MY_COMPILER_MSVC`, `MY_COMPILER_GCC`, `MY_COMPILER_CLANG`, `MY_COMPILER_UNKNOWN`
- `MY_OS_WIN`, `MY_OS_LINUX`, `MY_OS_MAC`, `MY_OS_UNKNOWN`
- `MY_ARCH_X64`, `MY_ARCH_X86`, `MY_ARCH_ARM`
- `INLINE`, `RESTRICT`, `MY_ALIGN(n)`
- `MY_EXTERN_START`, `MY_EXTERN_END`



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

### Codeberg
`v0.3.0` Beta testing is currently being conducted by codeberg repo.
- https://codeberg.org/Noksek/mystd_noksek

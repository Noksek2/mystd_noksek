# mystd_noksek v0.3.0-dev
## Intro
### Information
- Language: C
- Operating Systems: Windows 11 x64, Linux (Debian/Ubuntu)
- Current Version: v0.3.0-dev
- License: Apache License 2.0 (As of mystd v0.3.0)
- Development Progress: <50%

### Development Objectives
- Minimize External Dependencies: Reduce reliance on external libraries。

- Personal Project Utility: Designed for use in personal projects such as Virtual Machines (VMs) and interpreted languages。

- Educational Purpose: To study and understand system programming and runtime systems using C。

- Practical Application: Hands-on implementation of Operating System theories。

### Supported Features / Modules / Macros
The primary goal is to use native OS-provided libraries wherever possible, avoiding standard libraries (like stdlib) except for console output。

- System Metadata Macros: Macros identifying the compiler, OS, and environment (e.g., OS info, compiler info, etc.)。

- Core Utilities: Essential macros and constants required for feature implementation。

- Synchronization: Simple mutex functionality (mymutex)。

- Memory Management: Custom memory allocators (myarena, mypool)。

- Time Utilities: High-resolution time functions (myclock)。

- Miscellaneous: Minimal helper functions for improved convenience。

## mystd.h
```c
/*mystd v0.3.0*/
#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 3
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


mypool of `v0.2` changed by myslowpool. Do not recommended to use.
- `void  mypool_new(uint32_t max_threshold)`
- `void* mypool_alloc(uint32_t len)`
- `void  mypool_free(void* ptr)`
- `void  mypool_destroy()`

### mylog.h
- `MY_LOG_LEVEL`, `MY_LOG_LEVEL_OFF`, `MY_LOG_LEVEL_ERROR`, `MY_LOG_LEVEL_INFO`, `MY_LOG_LEVEL_TEST`
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

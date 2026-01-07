# MyStd (by Noksek)
## UNSTABLE
## v0.1.0
## Apache License 2.0
## Function/Macro lists
### mystd.h
```c
/*mystd v0.1.0*/
#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 1
#define MYSTD_VERSION_PATCH 0```
-`void mystd_print_system_info()`

### myallocator.h
-`void  allocator_new(myallocator* alc, mysize_t arena_size)`
-`void* allocator_alloc(myallocator* alc, mysize_t len)`
-`void 	allocator_free(myallocator* alc)`
-`void  allocator_reset(myallocator* alc)`
-`void 	allocator_check(myallocator* alc, myarena_check* checkpoint)`
-`void 	allocator_rewind(myallocator* alc, myarena_check* checkpoint)`
-`void* allocator_realloc(myallocator* alc, void* p, mysize_t old_capa, mysize_t new_capa)`

-`ALLOCATOR_ALLOC(ALC, T, SZ)`

### mylog.h
-`MY_LOG_LEVEL`, `MY_LOG_LEVEL_OFF`, `MY_LOG_LEVEL_ERROR`, `MY_LOG_LEVEL_INFO`
-`MY_BREAK()`
-`MY_ASSERT(cond)`
-`MY_LOG_INFO(fmt, ...)`

### mydef.h
-`MY_COMPILER_MSVC`, `MY_COMPILER_GCC`, `MY_COMPILER_CLANG`, `MY_COMPILER_UNKNOWN`
-`MY_OS_WIN`, `MY_OS_LINUX`, `MY_OS_MAC`, `MY_OS_UNKNOWN`
-`MY_ARCH_X64`, `MY_ARCH_X86`, `MY_ARCH_ARM`
-`INLINE`, `RESTRICT`, `MY_ALIGN(n)`
-`MY_EXTERN_START`, `MY_EXTERN_END`


# MyStd (by Noksek)
## v0.2.0 (dev branch)
- unstable yet.
## Apache License 2.0
## Function/Macro lists
### mystd.h
```c
/*mystd v0.2.0*/
#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 2
#define MYSTD_VERSION_PATCH 0


typedef uint64_t myclock_t;
```
- `void mystd_print_system_info()`
- `myclock_t myclock_setclock()`
- `double    myclock_getsec(myclock_t timer)`

### myallocator.h

- `void  myarean_new   (myarena* alc, mysize_t arena_size)`
- `void* myarean_alloc (myarena* alc, mysize_t len)`
- `void  myarean_free  (myarena* alc)`
- `void  myarean_reset (myarena* alc)`
- `void  myarean_check_new (myarena* alc, myarena_check* checkpoint)`
- `void  myarean_rewind (myarena* alc, myarena_check* checkpoint)`
- `void* myarean_realloc(myarena* alc, void* p, mysize_t old_capa, mysize_t new_capa)`

- `ALLOCATOR_ALLOC(ALC, T, SZ)`

- pool
```c
typedef enum {
	POOLMAP_SIZE = 32,
	POOL_8B = 0,
	POOL_16B, POOL_24B,
	POOL_32B, POOL_40B, POOL_48B, POOL_52B,
	POOL_64B, POOL_80B, POOL_96B, POOL_112B,
	POOL_128B, POOL_160B, POOL_192B, POOL_224B,
	POOL_256B, POOL_320B, POOL_384B, POOL_448B,
	POOL_512B,
	...
} mypoolsize_t
```

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



## HOW TO USE?
- CHECK the sample(main.c) please


###

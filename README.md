# MyStd (by Noksek)
## v0.1.1 (UNSTABLE)
## Apache License 2.0
## Function/Macro lists
### mystd.h
```c
/*mystd v0.1.1*/
#define MYSTD_VERSION_MAJOR 0 
#define MYSTD_VERSION_MINOR 1
#define MYSTD_VERSION_PATCH 1
```
- `void mystd_print_system_info()`

### myallocator.h
- `void  allocator_new(myallocator* alc, mysize_t arena_size)`
- `void* allocator_alloc(myallocator* alc, mysize_t len)`
- `void 	allocator_free(myallocator* alc)`
- `void  allocator_reset(myallocator* alc)`
- `void 	allocator_check(myallocator* alc, myarena_check* checkpoint)`
- `void 	allocator_rewind(myallocator* alc, myarena_check* checkpoint)`
- `void* allocator_realloc(myallocator* alc, void* p, mysize_t old_capa, mysize_t new_capa)`

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

### main.c (example)
```c
#include "mystd.h"
int main() {
	myallocator alc;
	allocator_new(&alc, 1000);
	for(volatile int i=0;i< 10;i++)
		allocator_alloc(&alc, 5000000);
	allocator_free(&alc);
	mystd_print_system_info();
	return 0;
}
```

- result

```
CC = gcc
CFLAGS = -O2 -D_DEBUG
TARGET = mystd_test
SRCS = mystd.c myallocator.c main.c

all: $(TARGET)

$(TARGET): $(SRCS)
        $(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
clean:
        rm -f $(TARGET)
```

```
$ make
$ ./mystd_test
[INFO] new allocator [7F77329DF000](1000)
[INFO] full! new allocator id:1 [7F773231A000], Done [7F773231A018]
[INFO] full! new allocator id:2 [7F7731E55000], Done [7F7731E55018]
[INFO] full! new allocator id:3 [7F7731990000], Done [7F7731990018]
[INFO] full! new allocator id:4 [7F77314CB000], Done [7F77314CB018]
[INFO] full! new allocator id:5 [7F7731006000], Done [7F7731006018]
[INFO] full! new allocator id:6 [7F7730B41000], Done [7F7730B41018]
[INFO] full! new allocator id:7 [7F773067C000], Done [7F773067C018]
[INFO] full! new allocator id:8 [7F77301B7000], Done [7F77301B7018]
[INFO] full! new allocator id:9 [7F772FCF2000], Done [7F772FCF2018]
[INFO] full! new allocator id:10 [7F772F82D000], Done [7F772F82D018]
[INFO] free block id:0 [7F77329DF000]..
[INFO] free block id:1 [7F773231A000]..
[INFO] free block id:2 [7F7731E55000]..
[INFO] free block id:3 [7F7731990000]..
[INFO] free block id:4 [7F77314CB000]..
[INFO] free block id:5 [7F7731006000]..
[INFO] free block id:6 [7F7730B41000]..
[INFO] free block id:7 [7F773067C000]..
[INFO] free block id:8 [7F77301B7000]..
[INFO] free block id:9 [7F772FCF2000]..
[INFO] free block id:10 [7F772F82D000]..
--- mystd system info (by Noksek2)---
Version         : v0.1.1
OS              : Linux
Compiler        : GCC
Arch            : x64
-------------------------------------
```
###

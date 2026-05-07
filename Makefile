CC = gcc
CFLAGS = -O2 -D_DEBUG
TARGET = mystd_test

# 오브젝트 파일 목록 (소스 파일의 .c를 .o로 변경)
OBJS = mystd.o myallocator.o main.o

all: $(TARGET)

# 최종 바이너리 링크
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 각 소스 파일 컴파일 (의존성 정의)
# main.c는 mystd.h를 참조함
main.o: main.c mystd.h
	$(CC) $(CFLAGS) -c main.c

# mystd.c는 mystd.h와 하위 myallocator.h를 참조함
mystd.o: mystd.c mystd.h myallocator.h
	$(CC) $(CFLAGS) -c mystd.c

# myallocator.c는 관련 헤더들(mylog.h, mydef.h 등)에 의존함
myallocator.o: myallocator.c myallocator.h mylog.h mydef.h
	$(CC) $(CFLAGS) -c myallocator.c
	
clean:
	rm -f $(TARGET) $(OBJS)
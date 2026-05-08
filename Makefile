CC = gcc
CFLAGS = -O2 -D_DEBUG
TARGET = bin/mystd_test
BINDIR = bin_linux
OUTDIR = bin

# 오브젝트 파일을 bin_linux 폴더에 생성
OBJS = $(BINDIR)/mystd.o $(BINDIR)/myallocator.o $(BINDIR)/main.o

all: $(BINDIR) $(OUTDIR) $(TARGET)

# bin_linux 폴더 생성 (없으면 만듦)
$(BINDIR):
	mkdir -p $(BINDIR)

# bin 폴더 생성 (없으면 만듦)
$(OUTDIR):
	mkdir -p $(OUTDIR)

# 최종 바이너리 링크 (bin 폴더에 저장)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 각 소스 파일 컴파일
$(BINDIR)/main.o: main.c mystd.h
	$(CC) $(CFLAGS) -c main.c -o $@

$(BINDIR)/mystd.o: mystd.c mystd.h myallocator.h
	$(CC) $(CFLAGS) -c mystd.c -o $@

$(BINDIR)/myallocator.o: myallocator.c myallocator.h mylog.h mydef.h
	$(CC) $(CFLAGS) -c myallocator.c -o $@

clean:
	rm -rf $(BINDIR) $(OUTDIR)
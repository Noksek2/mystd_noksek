## Variables ##
TARGET = $(OUTDIR)/mystd_test

CC := gcc
CFLAGS := -O2 -D_DEBUG -Wall -W -pedantic
CFLAGS += -g
CFLAGS += -MMD -MP

BINDIR := bin_linux
OUTDIR := bin

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,$(BINDIR)/%.o,$(SOURCES))
DEPENDS = $(patsubst %.c,$(BINDIR)/%.d,$(SOURCES))

## Targets ##
.PHONY: all make-dest clean run help

# Default Goal #
all: $(TARGET)

$(TARGET): $(OBJECTS) | make-dest
	$(CC) $^ $(CFLAGS) -o $@

$(OBJECTS): $(BINDIR)/%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

make-dest:
	mkdir -p $(BINDIR) $(OUTDIR)

# Dependency #
-include $(DEPENDS)

# Tasks #
clean:
	rm -rf $(BINDIR) $(OUTDIR)

run:
	$(TARGET)

help:
	@echo "   $(call color,1;33,make) - builds the target executable."
	@echo "   $(call color,1;33,make clean) - removes all the build artifacts."
	@echo "   $(call color,1;33,make run) - executes the target program."
	@echo "   $(call color,1;33,make help) - show this manual."

# Canned Recipes #
define color
\033[$1m$2\033[0m
endef

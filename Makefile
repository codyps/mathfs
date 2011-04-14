OBJ = fs.c.o
BIN = mathfs

CC = gcc
RM = rm -f

CFLAGS = -ggdb -O2
override CFLAGS += -Wall -pipe -MMD
FUSE_FLAGS := $(shell pkg-config fuse --cflags --libs)

.PHONY: all
all: build

.PHONY: build
build: $(BIN)

.PHONY: rebuild
rebuild: | clean
	$(MAKE) -C . build

.PHONY: clean
clean:
	$(RM) $(BIN) $(wildcard $(BIN)-g*.tar) $(wildcard *.d)

%.c.o : %.c
	$(CC) $(CFLAGS) $(FUSE_FLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(FUSE_FLAGS) -o $@ $^

.PHONY: archive
VER:=$(shell git rev-parse --verify --short HEAD 2>/dev/null)
PKG_NAME:=$(BIN)-g$(VER)
archive:
	git archive --prefix='$(PKG_NAME)/' HEAD > $(PKG_NAME).tar

-include $(wildcard *.d)

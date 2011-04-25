OBJ = 
BIN = mathfs tokenizer test_parse

.PHONY: all
all: build

mathfs: fs.c.o error.c.o parse.c.o eval.c.o

tokenizer: stack_eval.c.o tokenizer.c.o error.c.o

test_parse: error.c.o parse.c.o eval.c.o test_parse.c.o

CC = gcc
RM = rm -f

CFLAGS = -ggdb
override CFLAGS += -Wall -pipe -MMD
FUSE_CFLAGS  := $(shell pkg-config fuse --cflags)
FUSE_LDFLAGS := $(shell pkg-config fuse --libs)

.PHONY: build
build: $(BIN)

.PHONY: rebuild
rebuild: | clean
	$(MAKE) -C . build

.PHONY: clean
clean:
	$(RM) $(BIN) $(BIN)-g*.tar *.d *.o $(OBJ)

%.c.o : %.c
	$(CC) $(CFLAGS) $(FUSE_CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(FUSE_LDFLAGS) -o $@ $^

.PHONY: archive
VER:=$(shell git rev-parse --verify --short HEAD 2>/dev/null)
PKG_NAME:=$(BIN)-g$(VER)
archive:
	git archive --prefix='$(PKG_NAME)/' HEAD > $(PKG_NAME).tar

-include $(wildcard *.d)

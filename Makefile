VERSION = 0.30
CC	= gcc
LFLAGS	= -lpthread

CFLAGS += -std=gnu99 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64   \
	-D_XOPEN_SOURCE=600 -O2 -D_POSIX_C_SOURCE=200112L -Wall     \
	-Wcast-align -Wpointer-arith -pthread -Wbad-function-cast
# CFLAGS += -O0 -ggdb3 -Wconversion -Werror # -pg
#CFLAGS += -DNDEBUG

override TARGETS := rpc-barrier rpc-base rpc-piped_thread rpc-piped_process

.PHONY: all clean dist

all:    ${TARGETS}

dist:
	rm -f *.o

clean:  dist
	rm -f ${TARGETS}
	find -name '*.~' -exec rm {} \;

%: %.c Makefile
	$(CC) $(CFLAGS) $< -o $@

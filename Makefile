CFLAGS  = -Wall -O2 -g -std=gnu99 -pedantic
LDFLAGS = -pthread

GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0)
GLIB_LIBS   = $(shell pkg-config --libs glib-2.0)

AEROTOOLS_ROOT = ../aerotools-ng

OBJS = obj/modules/cpu.o obj/modules/mem.o obj/modules/gpu_nvidia.o \
       obj/modules/net.o obj/modules/time.o obj/modules/vfs.o
       
ifdef enable_aq5
	OBJS += obj/libaquaero5.o obj/modules/aq5.o
endif

.PHONY: all clean

all: mi3stat


mi3stat: $(OBJS) obj/main.o
	$(CC) $(LDFLAGS) -o $@ $+ $(GLIB_LIBS) 


obj/libaquaero5.o: $(AEROTOOLS_ROOT)/src/libaquaero5.c $(AEROTOOLS_ROOT)/src/libaquaero5.h
	$(CC) $(CFLAGS) -I ../aerotools-ng/src -c -o $@ $<
	
obj/modules/aq5.o: src/modules/aq5.c src/modules/aq5.h
	$(CC) $(CFLAGS) -I ../aerotools-ng/src -c -o $@ $<
	
obj/main.o: src/main.c
	$(CC) $(CFLAGS) $(GLIB_CFLAGS) -c -o $@ $<


obj/modules/%.o: src/modules/%.c src/modules/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -f obj/*.o obj/modules/*.o mi3stat

# Makefile for h5fnal/src

CC = gcc
CFLAGS = -fPIC -O3 -fno-omit-frame-pointer -g -Wall
CPPFLAGS = -I$(HDF5_INC)
LDFLAGS = -L$(HDF5_LIB) -lhdf5

all: libh5fnal.so
libs: libh5fnal.so

h5fnal.o: h5fnal.c h5fnal.h
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c h5fnal.c -o h5fnal.o

util.o: util.c util.h
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c util.c -o util.o

v_mc_hit_collection.o: v_mc_hit_collection.c v_mc_hit_collection.h h5fnal.h
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c v_mc_hit_collection.c -o v_mc_hit_collection.o

v_mc_truth.o: v_mc_truth.c v_mc_truth.h h5fnal.h
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c v_mc_truth.c -o v_mc_truth.o

assns.o: assns.c assns.h h5fnal.h
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c assns.c -o assns.o

libh5fnal.so: h5fnal.o util.o v_mc_hit_collection.o v_mc_truth.o assns.o
	$(CC) -shared -fPIC -o $(@) $(LDFLAGS) $(^)

.PHONY: clean

clean:
	@rm -rf *.o
	@rm -rf libh5fnal.so

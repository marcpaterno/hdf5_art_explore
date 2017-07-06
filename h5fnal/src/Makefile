# Makefile for h5fnal/src

CC = ../../../hdf5/bin/h5cc
CFLAGS = -Wall

all: libh5fnal.a
libs: libh5fnal.a

h5fnal.o: h5fnal.c h5fnal.h
	$(CC) $(CFLAGS) -c h5fnal.c -o h5fnal.o

v_mc_hit_collection.o: v_mc_hit_collection.c v_mc_hit_collection.h h5fnal.h
	$(CC) $(CFLAGS) -c v_mc_hit_collection.c -o v_mc_hit_collection.o

v_mc_truth.o: v_mc_truth.c v_mc_truth.h h5fnal.h
	$(CC) $(CFLAGS) -c v_mc_truth.c -o v_mc_truth.o

assns.o: assns.c assns.h h5fnal.h
	$(CC) $(CFLAGS) -c assns.c -o assns.o

libh5fnal.a: h5fnal.o v_mc_hit_collection.o v_mc_truth.o assns.o
	ar rcs libh5fnal.a h5fnal.o v_mc_hit_collection.o v_mc_truth.o assns.o

.PHONY: clean

clean:
	@rm -rf *.o
	@rm -rf libh5fnal.a

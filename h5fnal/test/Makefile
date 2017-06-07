# Makefile for h5fnal/test

CC = ../../../hdf5/bin/h5cc
CPPFLAGS = -I../src
CFLAGS = -Wall
LDFLAGS = -L../src
LIBS = -lh5fnal
HDF5 = ../../../hdf5/bin/

all: test_v_mc_hit_collection test_v_mc_truth

test_v_mc_hit_collection: test_v_mc_hit_collection.c ../src/libh5fnal.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_hit_collection test_v_mc_hit_collection.c $(LIBS)

test_v_mc_truth: test_v_mc_truth.c ../src/libh5fnal.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_truth test_v_mc_truth.c $(LIBS)

check: test_v_mc_hit_collection test_v_mc_truth
	./test_h5fnal.sh

.PHONY: clean check

clean:
	rm -rf *.o
	rm -rf *.h5
	rm -rf *.out
	rm -rf test_v_mc_hit_collection
	rm -rf test_v_mc_truth

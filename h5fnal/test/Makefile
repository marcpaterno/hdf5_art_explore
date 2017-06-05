# Makefile for h5fnal/test

CC = ../../../hdf5/bin/h5cc
CPPFLAGS = -I../src
CFLAGS = -Wall
LDFLAGS = -L../src
LIBS = -lh5fnal
HDF5 = ../../../hdf5/bin/

all: test_v_mc_hit_collection test_v_mc_truth

test_v_mc_hit_collection: test_v_mc_hit_collection.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_hit_collection test_v_mc_hit_collection.c $(LIBS)

test_v_mc_truth: test_v_mc_truth.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_truth test_v_mc_truth.c $(LIBS)


check: test_v_mc_hit_collection test_v_mc_truth
	./test_v_mc_hit_collection
	./test_v_mc_truth
	$(HDF5)h5ls -r v_mc_hc.h5
	$(HDF5)h5dump -H v_mc_hc.h5
	$(HDF5)h5dump --dataset="testrun/testevent/vomchc" -c 3 v_mc_hc.h5
	$(HDF5)h5ls -r v_mc_truth.h5

.PHONY: clean check

clean:
	rm -rf *.o
	rm -rf *.h5
	rm -rf test_v_mc_hit_collection
	rm -rf test_v_mc_truth

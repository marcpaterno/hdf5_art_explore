# Makefile for h5fnal/test

CC = ../../../hdf5/bin/h5cc
CPPFLAGS = -I../src
CFLAGS = -Wall
LDFLAGS = -L../src
LIBS = -lh5fnal
HDF5 = ../../../hdf5/bin/

all: test_v_mc_hit_collection test_v_mc_truth test_assns

test_v_mc_hit_collection: test_v_mc_hit_collection.c ../src/libh5fnal.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_hit_collection test_v_mc_hit_collection.c $(LIBS)

test_v_mc_truth: test_v_mc_truth.c ../src/libh5fnal.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_truth test_v_mc_truth.c $(LIBS)

test_assns: test_assns.c ../src/libh5fnal.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_assns test_assns.c $(LIBS)

check: test_v_mc_hit_collection test_v_mc_truth test_assns
	@./test_h5fnal.sh

.PHONY: clean check

clean:
	@rm -rf *.o
	@rm -rf *.out
	@rm -rf v_mc_hc.h5
	@rm -rf test_v_mc_hit_collection
	@rm -rf test_v_mc_truth
	@rm -rf test_assns

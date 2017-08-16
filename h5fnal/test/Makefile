# Makefile for h5fnal/test

CC = gcc
CPPFLAGS = -I../src -I$(HDF5_INC)
CFLAGS = -Wall -O3 -fno-omit-frame-pointer -g -fPIC
LDFLAGS = -L../src -L$(HDF5_LIB)
LIBS = -lh5fnal -lhdf5

all: test_v_mc_hit_collection test_v_mc_truth test_assns

test_v_mc_hit_collection: test_v_mc_hit_collection.c ../src/libh5fnal.so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_hit_collection test_v_mc_hit_collection.c $(LIBS)

test_v_mc_truth: test_v_mc_truth.c ../src/libh5fnal.so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o test_v_mc_truth test_v_mc_truth.c $(LIBS)

test_assns: test_assns.c ../src/libh5fnal.so
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

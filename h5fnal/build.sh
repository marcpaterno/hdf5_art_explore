#!/bin/bash
../hdf5/bin/h5cc -o h5fnal.o -c h5fnal.c
ar rcs libh5fnal.a h5fnal.o

../hdf5/bin/h5cc -lh5fnal -L. -o test_v_mc_hit_collection test_v_mc_hit_collection.c

./test_v_mc_hit_collection

../hdf5/bin/h5ls -r v_mc_hc.h5

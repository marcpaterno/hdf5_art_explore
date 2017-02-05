#!/bin/bash
../hdf5/bin/h5cc -c h5fnal.c -o h5fnal.o
ar rcs libh5fnal.a h5fnal.o

../hdf5/bin/h5cc -lh5fnal -L. -o test_v_mc_hit_collection test_v_mc_hit_collection.c

./test_v_mc_hit_collection

../hdf5/bin/h5ls -r v_mc_hc.h5
#../hdf5/bin/h5dump -H v_mc_hc.h5
#../hdf5/bin/h5dump --dataset="testrun/testevent/vomchc" -c 3 v_mc_hc.h5

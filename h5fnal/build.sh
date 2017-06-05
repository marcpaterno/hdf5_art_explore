#!/bin/bash

# Path to HDF5 installation bin directory
HDF5=../../hdf5/bin/

# libh5fnal
${HDF5}h5cc -c h5fnal.c -o h5fnal.o
${HDF5}h5cc -c v_mc_hit_collection.c -o v_mc_hit_collection.o
${HDF5}h5cc -c v_mc_truth.c -o v_mc_truth.o
ar rcs libh5fnal.a h5fnal.o v_mc_hit_collection.o v_mc_truth.o


# Vector of MC Hit Collection
${HDF5}h5cc -lh5fnal -L. -o test_v_mc_hit_collection test_v_mc_hit_collection.c
./test_v_mc_hit_collection
${HDF5}h5ls -r v_mc_hc.h5
#${HDF5}h5dump -H v_mc_hc.h5
#${HDF5}h5dump --dataset="testrun/testevent/vomchc" -c 3 v_mc_hc.h5


# Vector of MC Truth
${HDF5}h5cc -lh5fnal -L. -o test_v_mc_truth test_v_mc_truth.c
./test_v_mc_truth
${HDF5}h5ls -r v_mc_truth.h5

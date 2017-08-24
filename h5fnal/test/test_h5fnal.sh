#!/bin/bash

# Get rid of old files
rm -rf v_mc_hc.h5
rm -rf test_v_mc_hit_collection.h5ls.out
rm -rf test_v_mc_hit_collection.h5dump_H.out
rm -rf test_v_mc_hit_collection.h5dump_dataset.out

# Run unit tests
./test_v_mc_hit_collection
./test_v_mc_truth
./test_assns

# Check HDF5 tool output
#echo -n "Checking output: vector of MC Hit Collection (h5ls): "
#h5ls -r v_mc_hc.h5 > test_v_mc_hit_collection.h5ls.out 2>&1
#diff test_v_mc_hit_collection.h5ls.correct test_v_mc_hit_collection.h5ls.out 2>&1 >/dev/null
#if [ $? -ne 0 ] ; then
#    echo "*** FAILED ***"
#else
#    echo "SUCCESS!"
#fi

#echo -n "Checking output: vector of MC Hit Collection (h5dump -H): "
#h5dump -H v_mc_hc.h5 > test_v_mc_hit_collection.h5dump_H.out 2>&1
#diff test_v_mc_hit_collection.h5dump_H.correct test_v_mc_hit_collection.h5dump_H.out 2>&1 >/dev/null
#if [ $? -ne 0 ] ; then
#    echo "*** FAILED ***"
#else
#    echo "SUCCESS!"
#fi


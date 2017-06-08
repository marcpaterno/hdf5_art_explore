/* v_mc_hit_collection.c */

#include "h5fnal.h"

/*********************/
/* MC HIT COLLECTION */
/*********************/

/* A vector of MC Hit Collection is implemented as a single dataset of
 * compound type in the file.
 *
 * Notes and differences from the UML diagram:
 *
 *  * The channel is recorded for each MC Hit
 *
 *  * The elements of the partVertex are stored separately
 */

hid_t
h5fnal_create_v_mc_hit_collection_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_hit_t))) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Tinsert(tid, "fSignalTime", HOFFSET(h5fnal_mc_hit_t, signal_time), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fSignalWidth", HOFFSET(h5fnal_mc_hit_t, signal_width), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPeakAmp", HOFFSET(h5fnal_mc_hit_t, peak_amp), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fCharge", HOFFSET(h5fnal_mc_hit_t, charge), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartVertexX", HOFFSET(h5fnal_mc_hit_t, part_vertex_x), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartVertexY", HOFFSET(h5fnal_mc_hit_t, part_vertex_y), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartVertexZ", HOFFSET(h5fnal_mc_hit_t, part_vertex_z), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartEnergy", HOFFSET(h5fnal_mc_hit_t, part_energy), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fTrackId", HOFFSET(h5fnal_mc_hit_t, part_track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fChannel", HOFFSET(h5fnal_mc_hit_t, channel), H5T_NATIVE_UINT) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_v_mc_hit_collection_type */



h5fnal_v_mc_hit_coll_t
h5fnal_create_v_mc_hit_collection(hid_t loc_id, const char *name)
{
    hid_t dcpl_id = -1;
    hid_t sid = -1;
    h5fnal_v_mc_hit_coll_t vector;
    hsize_t chunk_dims[1];
    hsize_t init_dims[1];
    hsize_t max_dims[1];

    /* Set up chunking (size is arbitrary for now) */
    chunk_dims[0] = 128;
    if((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_chunk(dcpl_id, 1, chunk_dims) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_deflate(dcpl_id, 6) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if((sid = H5Screate_simple(1, init_dims, max_dims)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create datatype */
    if((vector.datatype_id = h5fnal_create_v_mc_hit_collection_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")

    /* Create dataset */
    if((vector.dataset_id = H5Dcreate2(loc_id, name, vector.datatype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    /* close everything */
    if(H5Pclose(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR

    return vector;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
        H5Pclose(dcpl_id);
        H5Dclose(vector.dataset_id);
        H5Tclose(vector.datatype_id);
    } H5E_END_TRY;

    vector.dataset_id = H5FNAL_BAD_HID_T; 
    vector.datatype_id = H5FNAL_BAD_HID_T; 

    return vector;
} /* end h5fnal_create_v_mc_hit_collection() */

h5fnal_v_mc_hit_coll_t
h5fnal_open_v_mc_hit_collection(hid_t loc_id, const char *name)
{
    h5fnal_v_mc_hit_coll_t vector;

    /* Create datatype */
    if((vector.datatype_id = h5fnal_create_v_mc_hit_collection_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")

    /* Open dataset */
    if((vector.dataset_id = H5Dopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    return vector;

error:
    H5E_BEGIN_TRY {
        H5Dclose(vector.dataset_id);
        H5Tclose(vector.datatype_id);
    } H5E_END_TRY;

    vector.dataset_id = H5FNAL_BAD_HID_T; 
    vector.datatype_id = H5FNAL_BAD_HID_T; 

    return vector;
} /* end h5fnal_open_v_mc_hit_collection() */

herr_t
h5fnal_close_v_mc_hit_collection(h5fnal_v_mc_hit_coll_t vector)
{
    if(H5Dclose(vector.dataset_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tclose(vector.datatype_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Dclose(vector.dataset_id);
        H5Tclose(vector.datatype_id);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
} /* end h5fnal_close_v_mc_hit_collection() */


herr_t
h5fnal_write_hits(h5fnal_v_mc_hit_coll_t vector, size_t n_hits, h5fnal_mc_hit_t *hits)
{
    hid_t file_sid = -1;             /* dataspace ID                             */
    hid_t memory_sid = -1;             /* dataspace ID                             */
    hsize_t curr_dims[1];   /* initial size of dataset                  */
    hsize_t new_dims[1];   /* new size of data dataset             */
    hsize_t start[1];
    hsize_t stride[1];
    hsize_t count[1];
    hsize_t block[1];

    /* Create the memory dataspace (set of points describing the data size, etc.) */
    curr_dims[0] = n_hits;
    if((memory_sid = H5Screate_simple(1, curr_dims, curr_dims)) < 0)
        H5FNAL_HDF5_ERROR

    /* Get the size (current size only) of the dataset */
    if((file_sid = H5Dget_space(vector.dataset_id)) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Sget_simple_extent_dims(file_sid, curr_dims, NULL) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR

    /* Resize the dataset to hold the new data */
    new_dims[0] = curr_dims[0] + n_hits;
    if(H5Dset_extent(vector.dataset_id, new_dims) < 0)
        H5FNAL_HDF5_ERROR

    /* Get the resized file space */
    if((file_sid = H5Dget_space(vector.dataset_id)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create a hyperslab describing where the data should go */
    start[0] = curr_dims[0];
    stride[0] = 1;
    count[0] = n_hits;
    block[0] = 1;
    if(H5Sselect_hyperslab(file_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        H5FNAL_HDF5_ERROR

    /* Write the hits to the dataset */
    if(H5Dwrite(vector.dataset_id, vector.datatype_id, memory_sid, file_sid, H5P_DEFAULT, hits) < 0)
        H5FNAL_HDF5_ERROR

    /* Close everything */
    if(H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Sclose(memory_sid) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(file_sid);
        H5Sclose(memory_sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
} /* end h5fnal_write_hits() */


hssize_t
h5fnal_get_hits_count(h5fnal_v_mc_hit_coll_t vector)
{
    hid_t sid = H5FNAL_BAD_HID_T;
    hssize_t n_hits = -1;

    if((sid = H5Dget_space(vector.dataset_id)) < 0)
        H5FNAL_HDF5_ERROR
    if((n_hits = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR

    return n_hits;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return -1;
} /* end h5fnal_get_hits_count() */


herr_t
h5fnal_read_all_hits(h5fnal_v_mc_hit_coll_t vector, h5fnal_mc_hit_t *hits)
{
    if(!hits)
        H5FNAL_PROGRAM_ERROR("hits parameter cannot be NULL")

    if(H5Dread(vector.dataset_id, vector.datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, hits) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
    
} /* end h5fnal_read_all_hits() */


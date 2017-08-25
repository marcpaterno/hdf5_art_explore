/* v_mc_hit_collection.c */

#include <stdlib.h>

#include "h5fnal.h"

/*********************/
/* MC HIT COLLECTION */
/*********************/

/* Dataset names for this data product */
#define H5FNAL_HIT_DATASET_NAME         "hits"
#define H5FNAL_HITCOLL_DATASET_NAME     "hit_collections"


/************************************************************************
 * h5fnal_create_hit_type()
 *
 * Creates and returns an HDF5 compound datatype that represents an MCHit.
 * The fields correspond directly to the internal data stored in an MCHit.
 ************************************************************************/
hid_t
h5fnal_create_hit_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_hit_t))) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "fSignalTime", HOFFSET(h5fnal_hit_t, signal_time), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fSignalWidth", HOFFSET(h5fnal_hit_t, signal_width), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fPeakAmp", HOFFSET(h5fnal_hit_t, peak_amp), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fCharge", HOFFSET(h5fnal_hit_t, charge), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fPartVertexX", HOFFSET(h5fnal_hit_t, part_vertex_x), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fPartVertexY", HOFFSET(h5fnal_hit_t, part_vertex_y), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fPartVertexZ", HOFFSET(h5fnal_hit_t, part_vertex_z), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fPartEnergy", HOFFSET(h5fnal_hit_t, part_energy), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "fTrackId", HOFFSET(h5fnal_hit_t, part_track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR;

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_hit_type */


/************************************************************************
 * h5fnal_create_hitcoll_type()
 *
 * Creates and returns an HDF5 compound datatype that represents an MCHitCollection.
 * The fChannel field represents a hit collection's channel, and the start and count
 * fields are indexes into the hits dataset. We use count instead of an end index
 * to more easily represent empty hit collections.
 ************************************************************************/
hid_t
h5fnal_create_hitcoll_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_hitcoll_t))) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "fChannel", HOFFSET(h5fnal_hitcoll_t, channel), H5T_NATIVE_UINT) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "start", HOFFSET(h5fnal_hitcoll_t, start), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "count", HOFFSET(h5fnal_hitcoll_t, count), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR;

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_hitcoll_type */


/************************************************************************
 * h5fnal_close_vector_on_err()
 *
 * Closes HDF5 IDs for this data product with no error
 * checking. Used to shut everything down and set the hid_t
 * values to bad values when we need to ensure the vector
 * is invalid.
 ************************************************************************/
static void
h5fnal_close_vector_on_err(h5fnal_vect_hitcoll_t *vector)
{
    if (vector) {
        H5E_BEGIN_TRY {
            H5Dclose(vector->hit_dset_id);
            H5Tclose(vector->hit_dtype_id);
            H5Dclose(vector->hitcoll_dset_id);
            H5Tclose(vector->hitcoll_dtype_id);
            H5Gclose(vector->top_level_group_id);
        } H5E_END_TRY;

        vector->hit_dset_id         = H5FNAL_BAD_HID_T;
        vector->hit_dtype_id        = H5FNAL_BAD_HID_T;
        vector->hitcoll_dset_id     = H5FNAL_BAD_HID_T;
        vector->hitcoll_dtype_id    = H5FNAL_BAD_HID_T;
        vector->top_level_group_id  = H5FNAL_BAD_HID_T;
    }

    return;
} /* end h5fnal_close_vector_on_err() */


/************************************************************************
 * h5fnal_create_v_mc_hit_collection()
 ************************************************************************/
herr_t
h5fnal_create_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_vect_hitcoll_t *vector)
{
    hid_t dcpl_id = -1;
    hid_t sid = -1;
    hsize_t chunk_dims[1];
    hsize_t init_dims[1];
    hsize_t max_dims[1];

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");

    /* Create top-level group */
    if ((vector->top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Set up chunking (size is arbitrary for now) */
    chunk_dims[0] = 1024;
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_chunk(dcpl_id, 1, chunk_dims) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_shuffle(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_deflate(dcpl_id, 6) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if ((sid = H5Screate_simple(1, init_dims, max_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create datatypes */
    if ((vector->hit_dtype_id = h5fnal_create_hit_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create hit datatype");
    if ((vector->hitcoll_dtype_id = h5fnal_create_hitcoll_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create hitcoll datatype");

    /* Create datasets */
    if ((vector->hit_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_HIT_DATASET_NAME, vector->hit_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->hitcoll_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_HITCOLL_DATASET_NAME, vector->hitcoll_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* close everything */
    if (H5Pclose(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* end h5fnal_create_v_mc_hit_collection() */


/************************************************************************
 * h5fnal_open_v_mc_hit_collection()
 ************************************************************************/
herr_t
h5fnal_open_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_vect_hitcoll_t *vector)
{
    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");

    /* Create datatypes */
    if ((vector->hit_dtype_id = h5fnal_create_hit_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create hit datatype");
    if ((vector->hitcoll_dtype_id = h5fnal_create_hitcoll_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create hitcoll datatype");

    /* Open top-level group */
    if ((vector->top_level_group_id = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Open datasets */
    if ((vector->hit_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_HIT_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->hitcoll_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_HITCOLL_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* end h5fnal_open_v_mc_hit_collection() */


/************************************************************************
 * h5fnal_close_v_mc_hit_collection()
 ************************************************************************/
herr_t
h5fnal_close_v_mc_hit_collection(h5fnal_vect_hitcoll_t *vector)
{
    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL")

    if (H5Dclose(vector->hit_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->hit_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(vector->hitcoll_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->hitcoll_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Gclose(vector->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR;

    vector->hit_dset_id         = H5FNAL_BAD_HID_T;
    vector->hit_dtype_id        = H5FNAL_BAD_HID_T;
    vector->hitcoll_dset_id     = H5FNAL_BAD_HID_T;
    vector->hitcoll_dtype_id    = H5FNAL_BAD_HID_T;
    vector->top_level_group_id  = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* end h5fnal_close_v_mc_hit_collection() */


/************************************************************************
 * h5fnal_append_hits()
 ************************************************************************/
herr_t
h5fnal_append_hits(h5fnal_vect_hitcoll_t *vector, h5fnal_vect_hitcoll_data_t *data)
{
    hid_t       sid     = H5FNAL_BAD_HID_T;
    hsize_t     offset;
    hsize_t     u;

    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");
    if (NULL == data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    /* Hit collection fixup.
     *
     * When appending hits and hit collections to non-empty datasets,
     * the 'start' references in the incoming data will have to be
     * modified so that they refer to the correct elements in the dataset. 
     */
    if ((sid = H5Dget_space(vector->hit_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((offset = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;
    if (offset > 0)
        for (u = 0; u < data->n_hit_collections; u++)
            if (data->hit_collections[u].count > 0)
                data->hit_collections[u].start += offset;

    /* append data */
    if (h5fnal_append_data(vector->hit_dset_id, vector->hit_dtype_id, data->n_hits, (const void *)data->hits) < 0)
        H5FNAL_PROGRAM_ERROR("could not append hit data");
    if (h5fnal_append_data(vector->hitcoll_dset_id, vector->hitcoll_dtype_id, data->n_hit_collections, (const void *)data->hit_collections) < 0)
        H5FNAL_PROGRAM_ERROR("could not append hit collection data");

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
} /* end h5fnal_append_hits() */


/************************************************************************
 * h5fnal_read_all_hits()
 ************************************************************************/
herr_t
h5fnal_read_all_hits(h5fnal_vect_hitcoll_t *vector, h5fnal_vect_hitcoll_data_t *data)
{
    hid_t       sid     = H5FNAL_BAD_HID_T;

    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL")

    /* Get the size of the hits dataset */
    if ((sid = H5Dget_space(vector->hit_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((data->n_hits = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the size of the hit collections dataset */
    if ((sid = H5Dget_space(vector->hitcoll_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((data->n_hit_collections = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Generate buffers for reading the hits */
    if (NULL == (data->hits = (h5fnal_hit_t *)calloc(data->n_hits, sizeof(h5fnal_hit_t))))
        H5FNAL_PROGRAM_ERROR("could allocate memory for hits");
    if (NULL == (data->hit_collections = (h5fnal_hitcoll_t *)calloc(data->n_hit_collections, sizeof(h5fnal_hitcoll_t))))
        H5FNAL_PROGRAM_ERROR("could allocate memory for hit collections");

    /* Read the data from the datasets */
    if (H5Dread(vector->hit_dset_id, vector->hit_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->hits) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(vector->hitcoll_dset_id, vector->hitcoll_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->hit_collections) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
    
} /* end h5fnal_read_all_hits() */


/************************************************************************
 * h5fnal_free_hitcoll_mem_data()
 *
 * Convenience function to clean up resources in the in-memory
 * hit and hit collection data struct.
 ************************************************************************/
herr_t
h5fnal_free_hitcoll_mem_data(h5fnal_vect_hitcoll_data_t *data)
{
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    free(data->hits);
    free(data->hit_collections);

    data->hits = NULL;
    data->hit_collections = NULL;

    data->n_hits = 0;
    data->n_hit_collections = 0;

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_free_hitcoll_mem_data() */


/* util.c */

#include <stdlib.h>
#include <string.h>

#include "util.h"

herr_t
h5fnal_add_string_attribute(hid_t loc_id, const char *name, const char *value)
{
    hid_t aid = -1;
    hid_t tid = -1;
    hid_t sid = -1;
    int rank;
    hsize_t dims;
    size_t len;

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == value)
        H5FNAL_PROGRAM_ERROR("value parameter cannot be NULL");

    /* Set up the string type */
    len = strlen(value) + 1;
    if ((tid = H5Tcopy(H5T_C_S1)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tset_size(tid, len) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create a dataspace for the attribute */
    rank = 1;
    dims = 1;
    if ((sid = H5Screate_simple(rank, &dims, NULL)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the attribute */
    if ((aid = H5Acreate(loc_id, name, tid, sid, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Write the value to the attribute */
    if (H5Awrite(aid, tid, (void *)value) < 0)
        H5FNAL_HDF5_ERROR;
    
    /* Close IDs */
    if (H5Aclose(aid) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(tid) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Aclose(aid);
        H5Tclose(tid);
        H5Sclose(sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;

} /* end h5fnal_add_string_attribute() */


herr_t
h5fnal_get_string_attribute(hid_t loc_id, const char *name, char **value)
{
    hid_t aid = -1;
    hid_t tid = -1;
    size_t len;
    hbool_t allocated = false;

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == value)
        H5FNAL_PROGRAM_ERROR("value parameter cannot be NULL");

    /* Open the attribute */
    if((aid = H5Aopen(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the type */
    if ((tid = H5Aget_type(aid)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Allocate memory for the string */
    len = H5Tget_size(tid);
    if (NULL == (*value = (char *)calloc(len, sizeof(char))))
        H5FNAL_PROGRAM_ERROR("could not get memory for attribute read");

    /* Read the attribute */
    if (H5Aread(aid, tid, (void *)*value))
        H5FNAL_HDF5_ERROR;

    /* Close IDs */
    if (H5Aclose(aid) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(tid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:

    if (allocated)
        free(*value);
    *value = NULL;

    H5E_BEGIN_TRY {
        H5Aclose(aid);
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;

} /* end h5fnal_get_string_attribute() */

hssize_t
h5fnal_get_dset_size(hid_t did)
{
    hid_t sid = H5FNAL_BAD_HID_T;
    hssize_t n = -1;

    if (did < 0)
        H5FNAL_PROGRAM_ERROR("did parameter cannot be negative");

    if ((sid = H5Dget_space(did)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((n = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return n;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return -1;
} /* end h5fnal_get_dset_size() */


/* Create an empty, chunked, 1D dataset */
herr_t
h5fnal_create_1D_dset(hid_t loc_id, const char *name, hid_t tid, hsize_t chunk_dim, /*OUT*/ hid_t *did)
{
    hid_t dcpl_id = -1;
    hid_t sid = -1;
    hsize_t chunk_dims[1];
    hsize_t init_dims[1];
    hsize_t max_dims[1];

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("loc_id parameter cannot be negative");
    if (!name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (tid < 0)
        H5FNAL_PROGRAM_ERROR("tid parameter cannot be negative");
    if (!did)
        H5FNAL_PROGRAM_ERROR("did parameter cannot be NULL");

    /* Create the dataset creation property list */
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Set up chunking */
    chunk_dims[0] = chunk_dim;
    if (H5Pset_chunk(dcpl_id, 1, chunk_dims) < 0)
        H5FNAL_HDF5_ERROR;

    /* Turn on compession */
    if (H5Pset_shuffle(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_deflate(dcpl_id, 6) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the dataspace */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if ((sid = H5Screate_simple(1, init_dims, max_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create datasets */
    if ((*did = H5Dcreate2(loc_id, name, tid, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* close everything */
    if (H5Pclose(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
        H5Pclose(dcpl_id);
    } H5E_END_TRY;

    if (did)
        did = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;

} /* end h5fnal_create_1D_dset() */

herr_t
h5fnal_append_data(hid_t did, hid_t tid, hsize_t n_elements, const void *data)
{
    hid_t file_sid = -1;                /* dataspace ID                             */
    hid_t memory_sid = -1;              /* dataspace ID                             */
    hsize_t curr_dims[1];               /* initial size of dataset                  */
    hsize_t new_dims[1];                /* new size of data dataset             */
    hsize_t start[1];
    hsize_t stride[1];
    hsize_t count[1];
    hsize_t block[1];

    /* Create the memory dataspace (set of points describing the data size, etc.) */
    curr_dims[0] = n_elements;
    if ((memory_sid = H5Screate_simple(1, curr_dims, curr_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the size (current size only) of the dataset */
    if ((file_sid = H5Dget_space(did)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sget_simple_extent_dims(file_sid, curr_dims, NULL) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Resize the dataset to hold the new data */
    new_dims[0] = curr_dims[0] + n_elements;
    if (H5Dset_extent(did, new_dims) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the resized file space */
    if ((file_sid = H5Dget_space(did)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create a hyperslab describing where the data should go */
    start[0] = curr_dims[0];
    stride[0] = 1;
    count[0] = n_elements;
    block[0] = 1;
    if (H5Sselect_hyperslab(file_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        H5FNAL_HDF5_ERROR;

    /* Write the data to the dataset */
    if (H5Dwrite(did, tid, memory_sid, file_sid, H5P_DEFAULT, data) < 0)
        H5FNAL_HDF5_ERROR;

    /* Close everything */
    if (H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(memory_sid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(file_sid);
        H5Sclose(memory_sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
} /* end h5fnal_append_data() */


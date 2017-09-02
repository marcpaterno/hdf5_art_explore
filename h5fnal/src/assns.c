/* assns.c */

#include <stdlib.h>
#include <string.h>

#include "assns.h"

#define H5FNAL_ASSNS_PAIR_DATASET_NAME          "pairs"
#define H5FNAL_ASSNS_DATA_DATASET_NAME          "data"

#define H5FNAL_LEFT_DATA_PRODUCT_NAME           "left data product"
#define H5FNAL_RIGHT_DATA_PRODUCT_NAME          "right data product"

hid_t
h5fnal_create_pair_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_pair_t))) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "left_process_index", HOFFSET(h5fnal_pair_t, left_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "left_product_index", HOFFSET(h5fnal_pair_t, left_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "left_key", HOFFSET(h5fnal_pair_t, left_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "right_process_index", HOFFSET(h5fnal_pair_t, right_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "right_product_index", HOFFSET(h5fnal_pair_t, right_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "right_key", HOFFSET(h5fnal_pair_t, right_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR;

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_association_type */

/************************************************************************
 * h5fnal_close_vector_on_err()
 *
 * Closes HDF5 IDs for this data product with no error
 * checking. Used to shut everything down and set the hid_t
 * values to bad values when we need to ensure the vector
 * is invalid.
 ************************************************************************/
static void
h5fnal_close_assns_on_err(h5fnal_assns_t *assns)
{
    if (assns) {

        H5E_BEGIN_TRY {
            H5Dclose(assns->pair_dset_id);
            H5Dclose(assns->data_dset_id);
            H5Tclose(assns->pair_dtype_id);
            H5Tclose(assns->data_dtype_id);
            H5Gclose(assns->top_level_group_id);
        } H5E_END_TRY;

        assns->pair_dset_id         = H5FNAL_BAD_HID_T;
        assns->data_dset_id         = H5FNAL_BAD_HID_T;
        assns->pair_dtype_id        = H5FNAL_BAD_HID_T;
        assns->data_dtype_id        = H5FNAL_BAD_HID_T;
        assns->top_level_group_id   = H5FNAL_BAD_HID_T;

        free(assns->left);
        free(assns->right);

        assns->left = NULL;
        assns->right = NULL;
    }

    return;
} /* end h5fnal_close_assns_on_err() */

herr_t
h5fnal_create_assns(hid_t loc_id, const char *name, const char *left, const char *right, 
        hid_t data_dtype_id, h5fnal_assns_t *assns)
{
    hid_t dcpl_id = -1;
    hid_t sid = -1;
    hsize_t chunk_dims[1];
    hsize_t init_dims[1];
    hsize_t max_dims[1];
    size_t dp_len;

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == left)
        H5FNAL_PROGRAM_ERROR("left parameter cannot be NULL");
    if (NULL == right)
        H5FNAL_PROGRAM_ERROR("right parameter cannot be NULL");
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    /* Initialize the data product struct */
    memset(assns, 0, sizeof(h5fnal_assns_t));

    /* Create top-level group */
    if ((assns->top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Add the left and right data product strings to the top-level group */
    if (h5fnal_add_string_attribute(assns->top_level_group_id, H5FNAL_LEFT_DATA_PRODUCT_NAME, left) < 0)
        H5FNAL_PROGRAM_ERROR("could not add left data product name attribute");
    if (h5fnal_add_string_attribute(assns->top_level_group_id, H5FNAL_RIGHT_DATA_PRODUCT_NAME, right) < 0)
        H5FNAL_PROGRAM_ERROR("could not add left data product name attribute");

    /* Store the names of the right and left data products in the struct */
    dp_len = strlen(left) + 1;
    if (NULL == (assns->left = (char *)malloc(dp_len)))
        H5FNAL_PROGRAM_ERROR("could not get memory for left data product string");
    strcpy(assns->left, left);
    dp_len = strlen(right) + 1;
    if (NULL == (assns->right = (char *)malloc(dp_len)))
        H5FNAL_PROGRAM_ERROR("could not get memory for right data product string");
    strcpy(assns->right, right);

    /* Create the dataset creation property list */
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Set up chunking (size is arbitrary for now) */
    chunk_dims[0] = 1024;
    if (H5Pset_chunk(dcpl_id, 1, chunk_dims) < 0)
        H5FNAL_HDF5_ERROR;

    /* Turn on compession */
    if (H5Pset_shuffle(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_deflate(dcpl_id, 6) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if ((sid = H5Screate_simple(1, init_dims, max_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the pair datatype */
    if ((assns->pair_dtype_id = h5fnal_create_pair_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create pair datatype");

    /* Create the pair dataset */
    if ((assns->pair_dset_id = H5Dcreate2(assns->top_level_group_id, H5FNAL_ASSNS_PAIR_DATASET_NAME,
            assns->pair_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Store the 'extra' data datatype and create the associated dataset of that type.
     *
     * We'll store a 'known invalid' value if the datatype is invalid (i.e.: not used)
     * to make things more consistent.
     */
    if (data_dtype_id >= 0) {
        if((assns->data_dtype_id = H5Tcopy(data_dtype_id)) < 0)
            H5FNAL_HDF5_ERROR;
        if ((assns->data_dset_id = H5Dcreate2(assns->top_level_group_id, H5FNAL_ASSNS_DATA_DATASET_NAME,
                assns->data_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
            H5FNAL_HDF5_ERROR;
    }
    else {
        assns->data_dtype_id = H5FNAL_BAD_HID_T;
        assns->data_dset_id = H5FNAL_BAD_HID_T;
    }

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

    if (assns)
        h5fnal_close_assns_on_err(assns);

    return H5FNAL_FAILURE;
} /* h5fnal_create_assns */

herr_t
h5fnal_open_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns)
{
    htri_t  data_dataset_exists;

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    /* Initialize the data product struct */
    memset(assns, 0, sizeof(h5fnal_assns_t));

    /* Create datatype */
    if ((assns->pair_dtype_id = h5fnal_create_pair_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create pair datatype");

    /* Open top-level group */
    if ((assns->top_level_group_id = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the left and right data product names */
    if (h5fnal_get_string_attribute(assns->top_level_group_id, H5FNAL_LEFT_DATA_PRODUCT_NAME, &(assns->left)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get left data product name attribute");
    if (h5fnal_get_string_attribute(assns->top_level_group_id, H5FNAL_RIGHT_DATA_PRODUCT_NAME, &(assns->right)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get left data product name attribute");

    /* Open pair dataset */
    if ((assns->pair_dset_id = H5Dopen2(assns->top_level_group_id, H5FNAL_ASSNS_PAIR_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Open data dataset and get its type, if it exists */
    if ((data_dataset_exists = H5Lexists(assns->top_level_group_id, H5FNAL_ASSNS_DATA_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if (data_dataset_exists) {
        if ((assns->data_dset_id = H5Dopen2(assns->top_level_group_id, H5FNAL_ASSNS_DATA_DATASET_NAME, H5P_DEFAULT)) < 0)
            H5FNAL_HDF5_ERROR;
        if ((assns->data_dtype_id = H5Dget_type(assns->data_dset_id)) < 0)
            H5FNAL_HDF5_ERROR;
    }
    else {
        assns->data_dset_id = H5FNAL_BAD_HID_T;
        assns->data_dtype_id = H5FNAL_BAD_HID_T;
    }

    return H5FNAL_SUCCESS;

error:
    if (assns)
        h5fnal_close_assns_on_err(assns);

    return H5FNAL_FAILURE;
} /* h5fnal_open_assns */

herr_t
h5fnal_close_assns(h5fnal_assns_t *assns)
{
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    free(assns->left);
    free(assns->right);
    assns->left = NULL;
    assns->right = NULL;

    if (H5Gclose(assns->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Dclose(assns->pair_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(assns->pair_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Only close these if they were used */
    if (assns->data_dset_id >= 0)
        if (H5Dclose(assns->data_dset_id) < 0)
            H5FNAL_HDF5_ERROR;
    if (assns->data_dtype_id >= 0)
        if (H5Tclose(assns->data_dtype_id) < 0)
            H5FNAL_HDF5_ERROR;

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->pair_dset_id = H5FNAL_BAD_HID_T;
    assns->pair_dtype_id = H5FNAL_BAD_HID_T;
    assns->data_dset_id = H5FNAL_BAD_HID_T;
    assns->data_dtype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if (assns)
        h5fnal_close_assns_on_err(assns);

    return H5FNAL_FAILURE;

} /* h5fnal_close_assns */

herr_t
h5fnal_append_assns(h5fnal_assns_t *assns, h5fnal_assns_data_t *data)
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
    curr_dims[0] = data->n;
    if ((memory_sid = H5Screate_simple(1, curr_dims, curr_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the size (current size only) of the dataset. Both datasets are the
     * same size.
     */
    if ((file_sid = H5Dget_space(assns->pair_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sget_simple_extent_dims(file_sid, curr_dims, NULL) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Resize the datasets to hold the new data */
    new_dims[0] = curr_dims[0] + data->n;
    if (H5Dset_extent(assns->pair_dset_id, new_dims) < 0)
        H5FNAL_HDF5_ERROR;
    if (assns->data_dset_id >= 0)
        if (H5Dset_extent(assns->data_dset_id, new_dims) < 0)
            H5FNAL_HDF5_ERROR;

    /* Get the resized file space */
    if ((file_sid = H5Dget_space(assns->pair_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create a hyperslab describing where the data should go */
    start[0] = curr_dims[0];
    stride[0] = 1;
    count[0] = data->n;
    block[0] = 1;
    if (H5Sselect_hyperslab(file_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        H5FNAL_HDF5_ERROR;

    /* Write the pairs to the dataset */
    if (H5Dwrite(assns->pair_dset_id, assns->pair_dtype_id, memory_sid, file_sid, H5P_DEFAULT, data->pairs) < 0)
        H5FNAL_HDF5_ERROR;

    /* Write the data to the dataset, if necessary */
    if (assns->data_dset_id >= 0)
        if (H5Dwrite(assns->data_dset_id, assns->data_dtype_id, memory_sid, file_sid, H5P_DEFAULT, data->data) < 0)
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
} /* end h5fnal_write_assns() */


herr_t
h5fnal_read_all_assns(h5fnal_assns_t *assns, h5fnal_assns_data_t *data)
{
    hid_t       sid     = H5FNAL_BAD_HID_T;

    if (!assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    /* Initialize the data struct */
    memset(data, 0, sizeof(h5fnal_assns_data_t));
 
    /* Get the size of the datasets (both have the same size) */
    if ((sid = H5Dget_space(assns->pair_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((data->n = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Generate a buffer for the pairs data and read it */
    if (NULL == (data->pairs = (h5fnal_pair_t *)calloc(data->n, sizeof(h5fnal_pair_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for pairs");
    if (H5Dread(assns->pair_dset_id, assns->pair_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->pairs) < 0)
        H5FNAL_HDF5_ERROR;

    /* Read the 'extra' associated data, if it exists */
    if (assns->data_dset_id >= 0) {
        size_t type_size = 0;

        /* Note that we can get the native type size from the HDF5 type */
        if (0 == (type_size = H5Tget_size(assns->data_dtype_id)))
            H5FNAL_HDF5_ERROR;
        if (NULL == (data->data = calloc(data->n, type_size)))
            H5FNAL_PROGRAM_ERROR("could not allocate memory for data");
        if (H5Dread(assns->data_dset_id, assns->data_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->data) < 0)
            H5FNAL_HDF5_ERROR;
    }

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    if (data)
        h5fnal_free_assns_mem_data(data);

    return H5FNAL_FAILURE;

} /* end h5fnal_read_all_assns() */

/************************************************************************
 * h5fnal_free_assns_mem_data()
 *
 * Convenience function to clean up resources in the in-memory
 * hit and hit collection data struct.
 ************************************************************************/
herr_t
h5fnal_free_assns_mem_data(h5fnal_assns_data_t *data)
{
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    free(data->pairs);
    free(data->data);

    memset(data, 0, sizeof(h5fnal_assns_data_t));

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_free_assns_mem_data() */


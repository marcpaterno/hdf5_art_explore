/* assns.c */

#include "h5fnal.h"

#define H5FNAL_ASSNS_DATA_DATASET_NAME          "data"
#define H5FNAL_ASSNS_ASSOCIATION_DATASET_NAME  "associations"

hid_t
h5fnal_create_association_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_association_t))) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "left_process_index", HOFFSET(h5fnal_association_t, left_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "left_product_index", HOFFSET(h5fnal_association_t, left_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "left_key", HOFFSET(h5fnal_association_t, left_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "right_process_index", HOFFSET(h5fnal_association_t, right_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "right_product_index", HOFFSET(h5fnal_association_t, right_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "right_key", HOFFSET(h5fnal_association_t, right_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR;

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_association_type */

herr_t
h5fnal_create_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns)
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
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    /* Create top-level group */
    if ((assns->top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Set up chunking (size is arbitrary for now) */
    chunk_dims[0] = 128;
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_chunk(dcpl_id, 1, chunk_dims) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_deflate(dcpl_id, 6) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if ((sid = H5Screate_simple(1, init_dims, max_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create datatype */
    if ((assns->association_datatype_id = h5fnal_create_association_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create association datatype");

    /* Create the association dataset
     * The association data dataset is optional and is created as
     * needed.
     */
    if ((assns->association_dataset_id = H5Dcreate2(assns->top_level_group_id, H5FNAL_ASSNS_ASSOCIATION_DATASET_NAME,
            assns->association_datatype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* The assns data dataset is optional and created lazily */
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    /* close everything */
    if (H5Pclose(dcpl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

    H5E_BEGIN_TRY {
        H5Sclose(sid);
        H5Pclose(dcpl_id);
        if(assns) {
            H5Gclose(assns->top_level_group_id);
            H5Dclose(assns->association_dataset_id);
            H5Tclose(assns->association_datatype_id);
            H5Dclose(assns->data_dataset_id);
            H5Tclose(assns->data_datatype_id);
        }
    } H5E_END_TRY;

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->association_dataset_id = H5FNAL_BAD_HID_T;
    assns->association_datatype_id = H5FNAL_BAD_HID_T;
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

error:
    return H5FNAL_FAILURE;
} /* h5fnal_create_assns */

herr_t
h5fnal_open_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns)
{
    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    /* Create datatype */
    if ((assns->association_datatype_id = h5fnal_create_association_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create association datatype");

    /* Open top-level group */
    if ((assns->top_level_group_id = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Open association dataset */
    if ((assns->association_dataset_id = H5Dopen2(assns->top_level_group_id, H5FNAL_ASSNS_ASSOCIATION_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* This is not implemented yet */
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        if (assns) {
            H5Gclose(assns->top_level_group_id);
            H5Dclose(assns->association_dataset_id);
            H5Tclose(assns->association_datatype_id);
            H5Dclose(assns->data_dataset_id);
            H5Tclose(assns->data_datatype_id);
        }
    } H5E_END_TRY;

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->association_dataset_id = H5FNAL_BAD_HID_T;
    assns->association_datatype_id = H5FNAL_BAD_HID_T;
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;
} /* h5fnal_open_assns */

herr_t
h5fnal_close_assns(h5fnal_assns_t *assns)
{
    if (NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    if (H5Gclose(assns->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Dclose(assns->association_dataset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(assns->association_datatype_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Only close these if they were used */
    if (assns->data_dataset_id >= 0)
        if (H5Dclose(assns->data_dataset_id) < 0)
            H5FNAL_HDF5_ERROR;
    if (assns->data_datatype_id >= 0)
        if (H5Tclose(assns->data_datatype_id) < 0)
            H5FNAL_HDF5_ERROR;

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->association_dataset_id = H5FNAL_BAD_HID_T;
    assns->association_datatype_id = H5FNAL_BAD_HID_T;
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if (assns) {
        H5E_BEGIN_TRY {
            H5Gclose(assns->top_level_group_id);
            H5Dclose(assns->association_dataset_id);
            H5Tclose(assns->association_datatype_id);
            H5Dclose(assns->data_dataset_id);
            H5Tclose(assns->data_datatype_id);
        } H5E_END_TRY;
    }

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->association_dataset_id = H5FNAL_BAD_HID_T;
    assns->association_datatype_id = H5FNAL_BAD_HID_T;
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;

} /* h5fnal_close_assns */

herr_t
h5fnal_append_assns(h5fnal_assns_t *assns, size_t n_assns, h5fnal_association_t *associations, void *data)
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
    curr_dims[0] = n_assns;
    if ((memory_sid = H5Screate_simple(1, curr_dims, curr_dims)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the size (current size only) of the dataset */
    if ((file_sid = H5Dget_space(assns->association_dataset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sget_simple_extent_dims(file_sid, curr_dims, NULL) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(file_sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Resize the dataset to hold the new data */
    new_dims[0] = curr_dims[0] + n_assns;
    if (H5Dset_extent(assns->association_dataset_id, new_dims) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the resized file space */
    if ((file_sid = H5Dget_space(assns->association_dataset_id)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create a hyperslab describing where the data should go */
    start[0] = curr_dims[0];
    stride[0] = 1;
    count[0] = n_assns;
    block[0] = 1;
    if (H5Sselect_hyperslab(file_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        H5FNAL_HDF5_ERROR;

    /* Write the assns to the dataset */
    if (H5Dwrite(assns->association_dataset_id, assns->association_datatype_id, memory_sid, file_sid, H5P_DEFAULT, associations) < 0)
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


hssize_t
h5fnal_get_assns_count(h5fnal_assns_t *assns)
{
    hid_t sid = H5FNAL_BAD_HID_T;
    hssize_t n_assns = -1;

    /* Note that we consider the size of the association dataset
     * as the 'number of assns' since it's non-optional.
     */
    if ((sid = H5Dget_space(assns->association_dataset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((n_assns = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    return n_assns;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return -1;
} /* end h5fnal_get_assns_count() */


herr_t
h5fnal_read_all_assns(h5fnal_assns_t *assns, h5fnal_association_t *associations, void *data)
{
    if (!assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL");

    if (H5Dread(assns->association_dataset_id, assns->association_datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, associations) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;

} /* end h5fnal_read_all_assns() */


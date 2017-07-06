/* assns.c */

#include "h5fnal.h"

hid_t
h5fnal_create_association_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_association_t))) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Tinsert(tid, "left_process_index", HOFFSET(h5fnal_association_t, left_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "left_product_index", HOFFSET(h5fnal_association_t, left_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "left_key", HOFFSET(h5fnal_association_t, left_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Tinsert(tid, "right_process_index", HOFFSET(h5fnal_association_t, right_process_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "right_product_index", HOFFSET(h5fnal_association_t, right_product_index), H5T_STD_U16LE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "right_key", HOFFSET(h5fnal_association_t, right_key), H5T_STD_U64LE) < 0)
        H5FNAL_HDF5_ERROR
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
    if(loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter")
    if(NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL")
    if(NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_create_assns */

herr_t
h5fnal_open_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns)
{
    if(loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter")
    if(NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL")
    if(NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_open_assns */

herr_t
h5fnal_close_assns(h5fnal_assns_t *assns)
{
    if(NULL == assns)
        H5FNAL_PROGRAM_ERROR("assns parameter cannot be NULL")

    if(H5Gclose(assns->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Dclose(assns->association_dataset_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tclose(assns->association_datatype_id) < 0)
        H5FNAL_HDF5_ERROR

    /* Only close these if they were used */
    if(assns->data_dataset_id >= 0)
        if(H5Dclose(assns->data_dataset_id) < 0)
            H5FNAL_HDF5_ERROR
    if(assns->data_datatype_id >= 0)
        if(H5Tclose(assns->data_datatype_id) < 0)
            H5FNAL_HDF5_ERROR

    assns->top_level_group_id = H5FNAL_BAD_HID_T;
    assns->association_dataset_id = H5FNAL_BAD_HID_T;
    assns->association_datatype_id = H5FNAL_BAD_HID_T;
    assns->data_dataset_id = H5FNAL_BAD_HID_T;
    assns->data_datatype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if(assns) {
        H5E_BEGIN_TRY {
            H5Gclose(assns->top_level_group_id);
            H5Dclose(assns->association_dataset_id);
            H5Tclose(assns->association_datatype_id);
            H5Dclose(assns->data_dataset_id);
            H5Tclose(assns->data_datatype_id);
        } H5E_END_TRY;
    }

    return H5FNAL_FAILURE;

} /* h5fnal_close_assns */


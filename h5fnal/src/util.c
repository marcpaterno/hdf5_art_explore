/* util.c */

#include <stdlib.h>
#include <string.h>

#include "h5fnal.h"
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


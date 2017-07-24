/* h5fnal.c */

#include "h5fnal.h"

/*******************/
/* RUNS AND EVENTS */
/*******************/

hid_t
h5fnal_create_run(hid_t loc_id, const char *name)
{
    hid_t gid = -1;         /* group ID                                     */
    hid_t gcpl_id = -1;     /* group creation property list ID              */

    /* Create a group to contain the events (or sub-runs).
     *
     * We want to index the group by creation order since that will
     * allow more efficient and straightforward iteration with H5Literate()
     * later and we can't set this up in the root group.
     */
    if ((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) , 0)
        H5FNAL_HDF5_ERROR
    if (H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0)
        H5FNAL_HDF5_ERROR
    if ((gid = H5Gcreate2(loc_id, name, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Pclose(gcpl_id) < 0)
        H5FNAL_HDF5_ERROR

    return gid;

error:
    /* It's best practice to try to close everything at the end, even
     * if there were errors.
     */
    H5E_BEGIN_TRY {
        H5Gclose(gid);
        H5Pclose(gcpl_id);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* end h5fnal_create_run() */

hid_t
h5fnal_open_run(hid_t loc_id, const char *name)
{
    hid_t gid = -1;         /* group ID                                     */

    if ((gid = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    return gid;

error:
    H5E_BEGIN_TRY {
        H5Gclose(gid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* end h5fnal_open_run() */

herr_t
h5fnal_close_run(hid_t loc_id)
{
    if (H5Gclose(loc_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_close_run() */

hid_t
h5fnal_create_event(hid_t loc_id, const char *name)
{
    hid_t gid = -1;         /* group ID                                     */
    hid_t gcpl_id = -1;     /* group creation property list ID              */

    /* Create a group to contain the events (or sub-runs).
     *
     * We want to index the group by creation order since that will
     * allow more efficient and straightforward iteration with H5Literate()
     * later and we can't set this up in the root group.
     */
    if ((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) , 0)
        H5FNAL_HDF5_ERROR
    if (H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0)
        H5FNAL_HDF5_ERROR
    if ((gid = H5Gcreate2(loc_id, name, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Pclose(gcpl_id) < 0)
        H5FNAL_HDF5_ERROR

    return gid;

error:
    H5E_BEGIN_TRY {
        H5Gclose(gid);
        H5Pclose(gcpl_id);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* end h5fnal_create_event() */

hid_t
h5fnal_open_event(hid_t loc_id, const char *name)
{
    hid_t gid = -1;         /* group ID                                     */

    if ((gid = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    return gid;

error:
    H5E_BEGIN_TRY {
        H5Gclose(gid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* end h5fnal_open_event() */

herr_t
h5fnal_close_event(hid_t loc_id)
{
    if (H5Gclose(loc_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_close_event() */


/* h5fnal.c */

#include "h5fnal.h"


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
    if((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) , 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0)
        H5FNAL_HDF5_ERROR
    if((gid = H5Gcreate2(loc_id, name, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Pclose(gcpl_id) < 0)
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

    if((gid = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
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
    if(H5Gclose(loc_id) < 0)
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
    if((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) , 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0)
        H5FNAL_HDF5_ERROR
    if((gid = H5Gcreate2(loc_id, name, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Pclose(gcpl_id) < 0)
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

    if((gid = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
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
    if(H5Gclose(loc_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_close_event() */


hid_t
h5fnal_create_v_mc_hit_collection_type(void)
{
    hid_t tid = -1;

    tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_hit_t));

    if(H5Tinsert(tid, "fSignalTime", HOFFSET(h5fnal_mc_hit_t, signal_time), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fSignalWidth", HOFFSET(h5fnal_mc_hit_t, signal_width), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPeakAmp", HOFFSET(h5fnal_mc_hit_t, peak_amp), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fCharge", HOFFSET(h5fnal_mc_hit_t, charge), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartVertex", HOFFSET(h5fnal_mc_hit_t, part_vertex), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fPartEnergy", HOFFSET(h5fnal_mc_hit_t, part_energy), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fTrackId", HOFFSET(h5fnal_mc_hit_t, part_track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "fChannel", HOFFSET(h5fnal_mc_hit_t, channel), H5T_NATIVE_INT) < 0)
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


#if 0
herr_t
h5fnal_write_hits(hid_t mc_hit_coll, uint64_t n_hits, mc_hit_t *hits)
{
}

herr_t
h5fnal_read_hits(hid_t mc_hit_coll, uint64_t n_hits, mc_hit_t *hits)
{
}
#endif

/* v_mc_truth.c */

#include "h5fnal.h"

/************/
/* MC TRUTH */
/************/

/* A vector of MC Truth is a fairly complicated data type and has a
 * complicated representation in the file.
 *
 *  * A top-level group holds all the MC Truth elements.
 *
 *  * A second level of groups holds the datasets that represent
 *    the MC Truth data. Alternatively, a naming scheme could be used
 *    associate the MC Truth data, but segregating the data using
 *    groups seems more straightforward.
 *
 * Notes and differences from the UML diagram:
 *
 */

hid_t
h5fnal_create_origin_enum_type(void)
{
    int enum_value;
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tenum_create(H5T_NATIVE_INT)) < 0)
        H5FNAL_HDF5_ERROR

    enum_value = 0;
    if (H5Tenum_insert(tid, "kUnknown", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 1;
    if (H5Tenum_insert(tid, "kBeamNeutrino", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 2;
    if (H5Tenum_insert(tid, "kCosmicRay", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 3;
    if (H5Tenum_insert(tid, "kSuperNovaParticle", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 4;
    if (H5Tenum_insert(tid, "kSingleParticle", &enum_value) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_origin_enum_type */

hid_t
h5fnal_create_mc_neutrino_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_neutrino_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "fMode", HOFFSET(h5fnal_mc_neutrino_t, mode), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fInteractionType", HOFFSET(h5fnal_mc_neutrino_t, interaction_type), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fCCNC", HOFFSET(h5fnal_mc_neutrino_t, ccnc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fTarget", HOFFSET(h5fnal_mc_neutrino_t, target), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fHitNuc", HOFFSET(h5fnal_mc_neutrino_t, hit_nuc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fHitQuark", HOFFSET(h5fnal_mc_neutrino_t, hit_quark), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fW", HOFFSET(h5fnal_mc_neutrino_t, w), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fX", HOFFSET(h5fnal_mc_neutrino_t, x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fY", HOFFSET(h5fnal_mc_neutrino_t, y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fQSqr", HOFFSET(h5fnal_mc_neutrino_t, q_sqr), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_neutrino_type */

hid_t
h5fnal_create_mc_particle_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_particle_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "fStatus", HOFFSET(h5fnal_mc_particle_t, status), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fTrackId", HOFFSET(h5fnal_mc_particle_t, track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpdgCode", HOFFSET(h5fnal_mc_particle_t, pdg_code), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fMother", HOFFSET(h5fnal_mc_particle_t, mother), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    /* NOTE: The 'strings' are just indexes into a string dictionary */
    if (H5Tinsert(tid, "fprocess", HOFFSET(h5fnal_mc_particle_t, process_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fendprocess", HOFFSET(h5fnal_mc_particle_t, endprocess_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fmass", HOFFSET(h5fnal_mc_particle_t, mass), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_x", HOFFSET(h5fnal_mc_particle_t, polarization_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_y", HOFFSET(h5fnal_mc_particle_t, polarization_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_z", HOFFSET(h5fnal_mc_particle_t, polarization_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fWeight", HOFFSET(h5fnal_mc_particle_t, weight), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_e", HOFFSET(h5fnal_mc_particle_t, gvtx_e), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_x", HOFFSET(h5fnal_mc_particle_t, gvtx_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_y", HOFFSET(h5fnal_mc_particle_t, gvtx_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_z", HOFFSET(h5fnal_mc_particle_t, gvtx_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "rescatter", HOFFSET(h5fnal_mc_particle_t, rescatter), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_particle_type */

hid_t
h5fnal_create_daughter_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_daughter_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "parent", HOFFSET(h5fnal_daughter_t, parent_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "child", HOFFSET(h5fnal_daughter_t, child_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_daughter_type */

hid_t
h5fnal_create_mc_trajectory_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_trajectory_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "Ec1", HOFFSET(h5fnal_mc_trajectory_t, Ec1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "px1", HOFFSET(h5fnal_mc_trajectory_t, px1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "py1", HOFFSET(h5fnal_mc_trajectory_t, py1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "pz1", HOFFSET(h5fnal_mc_trajectory_t, pz1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "Ec2", HOFFSET(h5fnal_mc_trajectory_t, Ec2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "px2", HOFFSET(h5fnal_mc_trajectory_t, px2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "py2", HOFFSET(h5fnal_mc_trajectory_t, py2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "pz2", HOFFSET(h5fnal_mc_trajectory_t, pz2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_index", HOFFSET(h5fnal_mc_trajectory_t, particle_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_trajectory_type */

hid_t
h5fnal_create_mc_truth_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_truth_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "neutrino_index", HOFFSET(h5fnal_mc_truth_t, neutrino_index), H5T_NATIVE_HSSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_start_index", HOFFSET(h5fnal_mc_truth_t, particle_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_end_index", HOFFSET(h5fnal_mc_truth_t, particle_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "trajectory_start_index", HOFFSET(h5fnal_mc_truth_t, trajectory_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "trajectory_end_index", HOFFSET(h5fnal_mc_truth_t, trajectory_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "daughters_start_index", HOFFSET(h5fnal_mc_truth_t, daughters_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "daughters_end_index", HOFFSET(h5fnal_mc_truth_t, daughters_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_trajectory_type */
herr_t
h5fnal_create_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector)
{
    hid_t dcpl_id = -1;
    hid_t sid = -1;
    hsize_t chunk_dims[1];
    hsize_t init_dims[1];
    hsize_t max_dims[1];

    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL")

    /* Create the top-level group for the vector */
    if ((vector->top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the datatypes */
    if ((vector->origin_enum_dtype_id = h5fnal_create_origin_enum_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if ((vector->neutrino_dtype_id = h5fnal_create_mc_neutrino_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if ((vector->particle_dtype_id = h5fnal_create_mc_particle_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if ((vector->daughter_dtype_id = h5fnal_create_daughter_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if ((vector->trajectory_dtype_id = h5fnal_create_mc_trajectory_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if ((vector->truth_dtype_id = h5fnal_create_mc_truth_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")

    /* Set up chunking (for all datasets, size is arbitrary for now) */
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

    /* Create the datasets */

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
        if (vector) {
            H5Gclose(vector->top_level_group_id);
            H5Tclose(vector->origin_enum_dtype_id);
            H5Tclose(vector->neutrino_dtype_id);
            H5Tclose(vector->particle_dtype_id);
            H5Tclose(vector->daughter_dtype_id);
            H5Tclose(vector->trajectory_dtype_id);
            H5Tclose(vector->truth_dtype_id);
        }
    } H5E_END_TRY;

    vector->origin_enum_dtype_id = H5FNAL_BAD_HID_T;
    vector->neutrino_dtype_id = H5FNAL_BAD_HID_T;
    vector->particle_dtype_id = H5FNAL_BAD_HID_T;
    vector->daughter_dtype_id = H5FNAL_BAD_HID_T;
    vector->trajectory_dtype_id = H5FNAL_BAD_HID_T;
    vector->truth_dtype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;
} /* h5fnal_create_v_mc_truth */

herr_t
h5fnal_open_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector)
{
    /* NOT IMPLEMENTED */

    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_open_v_mc_truth */

herr_t
h5fnal_close_v_mc_truth(h5fnal_v_mc_truth_t *vector)
{
    if (H5Gclose(vector->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->origin_enum_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->neutrino_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->particle_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->daughter_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->trajectory_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tclose(vector->truth_dtype_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        if (vector) {
            H5Gclose(vector->top_level_group_id);
            H5Tclose(vector->neutrino_dtype_id);
            H5Tclose(vector->particle_dtype_id);
            H5Tclose(vector->origin_enum_dtype_id);
            H5Tclose(vector->daughter_dtype_id);
            H5Tclose(vector->trajectory_dtype_id);
            H5Tclose(vector->truth_dtype_id);
        }
    } H5E_END_TRY;

    vector->top_level_group_id = H5FNAL_BAD_HID_T;
    vector->origin_enum_dtype_id = H5FNAL_BAD_HID_T;
    vector->neutrino_dtype_id = H5FNAL_BAD_HID_T;
    vector->particle_dtype_id = H5FNAL_BAD_HID_T;
    vector->daughter_dtype_id = H5FNAL_BAD_HID_T;
    vector->trajectory_dtype_id = H5FNAL_BAD_HID_T;
    vector->truth_dtype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;
} /* h5fnal_close_v_mc_truth */


/* v_mc_truth.c */

#include <stdlib.h>
#include <string.h>

#include "h5fnal.h"

/************/
/* MC TRUTH */
/************/

/* Dataset names */
#define H5FNAL_TRUTH_TRUTH_DATASET_NAME         "truths"
#define H5FNAL_TRUTH_NEUTRINO_DATASET_NAME      "neutrinos"
#define H5FNAL_TRUTH_PARTICLE_DATASET_NAME      "particles"
#define H5FNAL_TRUTH_DAUGHTER_DATASET_NAME      "daughters"
#define H5FNAL_TRUTH_TRAJECTORY_DATASET_NAME    "trajectories"

/* Prototypes */

hid_t
h5fnal_create_origin_type(void)
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
} /* h5fnal_create_origin_type */

hid_t
h5fnal_create_neutrino_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_neutrino_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "fMode", HOFFSET(h5fnal_neutrino_t, mode), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fInteractionType", HOFFSET(h5fnal_neutrino_t, interaction_type), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fCCNC", HOFFSET(h5fnal_neutrino_t, ccnc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fTarget", HOFFSET(h5fnal_neutrino_t, target), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fHitNuc", HOFFSET(h5fnal_neutrino_t, hit_nuc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fHitQuark", HOFFSET(h5fnal_neutrino_t, hit_quark), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fW", HOFFSET(h5fnal_neutrino_t, w), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fX", HOFFSET(h5fnal_neutrino_t, x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fY", HOFFSET(h5fnal_neutrino_t, y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fQSqr", HOFFSET(h5fnal_neutrino_t, q_sqr), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_neutrino_type */

hid_t
h5fnal_create_particle_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_particle_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "fStatus", HOFFSET(h5fnal_particle_t, status), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fTrackId", HOFFSET(h5fnal_particle_t, track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpdgCode", HOFFSET(h5fnal_particle_t, pdg_code), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fMother", HOFFSET(h5fnal_particle_t, mother), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    /* NOTE: The 'strings' are just indexes into a string dictionary */
    if (H5Tinsert(tid, "fprocess", HOFFSET(h5fnal_particle_t, process_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fendprocess", HOFFSET(h5fnal_particle_t, endprocess_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fmass", HOFFSET(h5fnal_particle_t, mass), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_x", HOFFSET(h5fnal_particle_t, polarization_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_y", HOFFSET(h5fnal_particle_t, polarization_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fpolarization_z", HOFFSET(h5fnal_particle_t, polarization_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fWeight", HOFFSET(h5fnal_particle_t, weight), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_e", HOFFSET(h5fnal_particle_t, gvtx_e), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_x", HOFFSET(h5fnal_particle_t, gvtx_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_y", HOFFSET(h5fnal_particle_t, gvtx_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "fGvtx_z", HOFFSET(h5fnal_particle_t, gvtx_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "rescatter", HOFFSET(h5fnal_particle_t, rescatter), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_particle_type */

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
h5fnal_create_trajectory_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_trajectory_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "Ec1", HOFFSET(h5fnal_trajectory_t, Ec1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "px1", HOFFSET(h5fnal_trajectory_t, px1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "py1", HOFFSET(h5fnal_trajectory_t, py1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "pz1", HOFFSET(h5fnal_trajectory_t, pz1), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "Ec2", HOFFSET(h5fnal_trajectory_t, Ec2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "px2", HOFFSET(h5fnal_trajectory_t, px2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "py2", HOFFSET(h5fnal_trajectory_t, py2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "pz2", HOFFSET(h5fnal_trajectory_t, pz2), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_index", HOFFSET(h5fnal_trajectory_t, particle_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_trajectory_type */

hid_t
h5fnal_create_truth_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_truth_t))) < 0)
        H5FNAL_HDF5_ERROR

    if (H5Tinsert(tid, "origin", HOFFSET(h5fnal_truth_t, origin), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "neutrino_index", HOFFSET(h5fnal_truth_t, neutrino_index), H5T_NATIVE_HSSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_start_index", HOFFSET(h5fnal_truth_t, particle_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "particle_end_index", HOFFSET(h5fnal_truth_t, particle_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "trajectory_start_index", HOFFSET(h5fnal_truth_t, trajectory_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "trajectory_end_index", HOFFSET(h5fnal_truth_t, trajectory_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "daughters_start_index", HOFFSET(h5fnal_truth_t, daughters_start_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Tinsert(tid, "daughters_end_index", HOFFSET(h5fnal_truth_t, daughters_end_index), H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_trajectory_type */

/************************************************************************
 * h5fnal_close_vector_on_err()
 *
 * Closes HDF5 IDs for this data product with no error
 * checking. Used to shut everything down and set the hid_t
 * values to bad values when we need to ensure the vector
 * is invalid.
 ************************************************************************/
static void
h5fnal_close_vector_on_err(h5fnal_vect_truth_t *vector)
{
    if (vector) {
        H5E_BEGIN_TRY {
            H5Tclose(vector->origin_dtype_id);

            H5Dclose(vector->neutrino_dset_id);
            H5Tclose(vector->neutrino_dtype_id);

            H5Dclose(vector->particle_dset_id);
            H5Tclose(vector->particle_dtype_id);

            H5Dclose(vector->daughter_dset_id);
            H5Tclose(vector->daughter_dtype_id);

            H5Dclose(vector->trajectory_dset_id);
            H5Tclose(vector->trajectory_dtype_id);

            H5Dclose(vector->truth_dset_id);
            H5Tclose(vector->truth_dtype_id);

            H5Gclose(vector->top_level_group_id);
        } H5E_END_TRY;

        vector->origin_dtype_id     = H5FNAL_BAD_HID_T;

        vector->neutrino_dset_id    = H5FNAL_BAD_HID_T;
        vector->neutrino_dtype_id   = H5FNAL_BAD_HID_T;

        vector->particle_dset_id    = H5FNAL_BAD_HID_T;
        vector->particle_dtype_id   = H5FNAL_BAD_HID_T;

        vector->daughter_dset_id    = H5FNAL_BAD_HID_T;
        vector->daughter_dtype_id   = H5FNAL_BAD_HID_T;

        vector->trajectory_dset_id  = H5FNAL_BAD_HID_T;
        vector->trajectory_dtype_id = H5FNAL_BAD_HID_T;

        vector->truth_dset_id       = H5FNAL_BAD_HID_T;
        vector->truth_dtype_id      = H5FNAL_BAD_HID_T;

        vector->top_level_group_id  = H5FNAL_BAD_HID_T;
    }

    return;
} /* end h5fnal_close_vector_on_err() */

herr_t
h5fnal_create_v_mc_truth(hid_t loc_id, const char *name, h5fnal_vect_truth_t *vector)
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
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL")

    /* Initialize the data product struct */
    memset(vector, 0, sizeof(h5fnal_vect_truth_t));

    /* Create the top-level group for the vector */
    if ((vector->top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the datatypes */
    if ((vector->origin_dtype_id = h5fnal_create_origin_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->neutrino_dtype_id = h5fnal_create_neutrino_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->particle_dtype_id = h5fnal_create_particle_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->daughter_dtype_id = h5fnal_create_daughter_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->trajectory_dtype_id = h5fnal_create_trajectory_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->truth_dtype_id = h5fnal_create_truth_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");

    /* Set up chunking (for all datasets, size is arbitrary for now) */
    chunk_dims[0] = 1024;
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        H5FNAL_HDF5_ERROR;
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

    /* Create the datasets */
    if ((vector->truth_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_TRUTH_TRUTH_DATASET_NAME,
            vector->truth_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->neutrino_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_TRUTH_NEUTRINO_DATASET_NAME,
            vector->neutrino_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->particle_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_TRUTH_PARTICLE_DATASET_NAME,
            vector->particle_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->daughter_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_TRUTH_DAUGHTER_DATASET_NAME,
            vector->daughter_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->trajectory_dset_id = H5Dcreate2(vector->top_level_group_id, H5FNAL_TRUTH_TRAJECTORY_DATASET_NAME,
            vector->trajectory_dtype_id, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
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

    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* h5fnal_create_v_mc_truth */

herr_t
h5fnal_open_v_mc_truth(hid_t loc_id, const char *name, h5fnal_vect_truth_t *vector)
{

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("invalid loc_id parameter");
    if (NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL");
    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");

    /* Initialize the data product struct */
    memset(vector, 0, sizeof(h5fnal_vect_truth_t));

    /* Open top-level group */
    if ((vector->top_level_group_id = H5Gopen2(loc_id, name, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the datatypes */
    if ((vector->origin_dtype_id = h5fnal_create_origin_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->neutrino_dtype_id = h5fnal_create_neutrino_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->particle_dtype_id = h5fnal_create_particle_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->daughter_dtype_id = h5fnal_create_daughter_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->trajectory_dtype_id = h5fnal_create_trajectory_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");
    if ((vector->truth_dtype_id = h5fnal_create_truth_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");

    /* Open the datasets */
    if ((vector->truth_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_TRUTH_TRUTH_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->neutrino_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_TRUTH_NEUTRINO_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->particle_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_TRUTH_PARTICLE_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->daughter_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_TRUTH_DAUGHTER_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((vector->trajectory_dset_id = H5Dopen2(vector->top_level_group_id, H5FNAL_TRUTH_TRAJECTORY_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* h5fnal_open_v_mc_truth */

herr_t
h5fnal_close_v_mc_truth(h5fnal_vect_truth_t *vector)
{
    if (NULL == vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");

    /* Top-level group */
    if (H5Gclose(vector->top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Datatypes */
    if (H5Tclose(vector->origin_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->neutrino_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->particle_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->daughter_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->trajectory_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(vector->truth_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Datasets */
    if (H5Dclose(vector->neutrino_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(vector->particle_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(vector->daughter_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(vector->trajectory_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(vector->truth_dset_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Set IDs to bad values */
    vector->origin_dtype_id     = H5FNAL_BAD_HID_T;

    vector->neutrino_dset_id    = H5FNAL_BAD_HID_T;
    vector->neutrino_dtype_id   = H5FNAL_BAD_HID_T;

    vector->particle_dset_id    = H5FNAL_BAD_HID_T;
    vector->particle_dtype_id   = H5FNAL_BAD_HID_T;

    vector->daughter_dset_id    = H5FNAL_BAD_HID_T;
    vector->daughter_dtype_id   = H5FNAL_BAD_HID_T;

    vector->trajectory_dset_id  = H5FNAL_BAD_HID_T;
    vector->trajectory_dtype_id = H5FNAL_BAD_HID_T;

    vector->truth_dset_id       = H5FNAL_BAD_HID_T;
    vector->truth_dtype_id      = H5FNAL_BAD_HID_T;

    vector->top_level_group_id  = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if (vector)
        h5fnal_close_vector_on_err(vector);

    return H5FNAL_FAILURE;
} /* h5fnal_close_v_mc_truth */

herr_t
h5fnal_append_truths(h5fnal_vect_truth_t *vector, h5fnal_vect_truth_data_t *data)
{
    if (!vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    /* XXX: Fixup datasets here */

    /* append data to all the datasets */
    if (h5fnal_append_data(vector->truth_dset_id, vector->truth_dtype_id, data->n_truths, (const void *)(data->truths)) < 0)
        H5FNAL_PROGRAM_ERROR("could not append truth data");
    if (h5fnal_append_data(vector->trajectory_dset_id, vector->trajectory_dtype_id, data->n_trajectories, (const void *)(data->trajectories)) < 0)
        H5FNAL_PROGRAM_ERROR("could not append trajectory data");
    if (h5fnal_append_data(vector->daughter_dset_id, vector->daughter_dtype_id, data->n_daughters, (const void *)(data->daughters)) < 0)
        H5FNAL_PROGRAM_ERROR("could not append daughter data");
    if (h5fnal_append_data(vector->particle_dset_id, vector->particle_dtype_id, data->n_particles, (const void *)(data->particles)) < 0)
        H5FNAL_PROGRAM_ERROR("could not append particle data");
    if (h5fnal_append_data(vector->neutrino_dset_id, vector->neutrino_dtype_id, data->n_neutrinos, (const void *)(data->neutrinos)) < 0)
        H5FNAL_PROGRAM_ERROR("could not append neutrino data");

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_append_truths() */

herr_t
h5fnal_read_all_truths(h5fnal_vect_truth_t *vector, h5fnal_vect_truth_data_t *data)
{
    if (!vector)
        H5FNAL_PROGRAM_ERROR("vector parameter cannot be NULL");
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    /* Initialize the data struct */
    memset(data, 0, sizeof(h5fnal_vect_truth_data_t));

    /* Get dataset sizes and allocate memory */
    if ((data->n_truths = h5fnal_get_dset_size(vector->truth_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get dataset size");
    if (NULL == (data->truths = (h5fnal_truth_t *)calloc(data->n_truths, sizeof(h5fnal_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    if ((data->n_trajectories = h5fnal_get_dset_size(vector->trajectory_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get dataset size");
    if (NULL == (data->trajectories = (h5fnal_trajectory_t *)calloc(data->n_trajectories, sizeof(h5fnal_trajectory_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    if ((data->n_daughters = h5fnal_get_dset_size(vector->daughter_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get dataset size");
    if (NULL == (data->daughters = (h5fnal_daughter_t *)calloc(data->n_daughters, sizeof(h5fnal_daughter_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    if ((data->n_particles = h5fnal_get_dset_size(vector->particle_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get dataset size");
    if (NULL == (data->particles = (h5fnal_particle_t *)calloc(data->n_particles, sizeof(h5fnal_particle_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    if ((data->n_neutrinos = h5fnal_get_dset_size(vector->neutrino_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get dataset size");
    if (NULL == (data->neutrinos = (h5fnal_neutrino_t *)calloc(data->n_neutrinos, sizeof(h5fnal_neutrino_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    /* Read data */
    if (H5Dread(vector->truth_dset_id, vector->truth_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->truths) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(vector->trajectory_dset_id, vector->trajectory_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->trajectories) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(vector->daughter_dset_id, vector->daughter_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->daughters) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(vector->particle_dset_id, vector->particle_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->particles) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(vector->neutrino_dset_id, vector->neutrino_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data->neutrinos) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    if (data)
        h5fnal_free_truth_mem_data(data);
    return H5FNAL_FAILURE;
} /* end h5fnal_read_all_truths() */

/* Important in case the library and application use a different
 * memory allocator.
 */
herr_t
h5fnal_free_truth_mem_data(h5fnal_vect_truth_data_t *data)
{
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    free(data->truths);
    free(data->trajectories);
    free(data->daughters);
    free(data->particles);
    free(data->neutrinos);

    memset(data, 0, sizeof(h5fnal_vect_truth_data_t));

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end h5fnal_free_truth_mem_data() */



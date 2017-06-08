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
h5fnal_create_mc_particle_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_particle_t))) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Tinsert(tid, "status", HOFFSET(h5fnal_mc_particle_t, status), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "track_id", HOFFSET(h5fnal_mc_particle_t, track_id), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "pdg_code", HOFFSET(h5fnal_mc_particle_t, pdg_code), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "mother", HOFFSET(h5fnal_mc_particle_t, mother), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    /* Strings go here */
    if(H5Tinsert(tid, "mass", HOFFSET(h5fnal_mc_particle_t, mass), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "polarization_x", HOFFSET(h5fnal_mc_particle_t, polarization_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "polarization_y", HOFFSET(h5fnal_mc_particle_t, polarization_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "polarization_z", HOFFSET(h5fnal_mc_particle_t, polarization_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "weight", HOFFSET(h5fnal_mc_particle_t, weight), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "gvtx_e", HOFFSET(h5fnal_mc_particle_t, gvtx_e), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "gvtx_x", HOFFSET(h5fnal_mc_particle_t, gvtx_x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "gvtx_y", HOFFSET(h5fnal_mc_particle_t, gvtx_y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "gvtx_z", HOFFSET(h5fnal_mc_particle_t, gvtx_z), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "rescatter", HOFFSET(h5fnal_mc_particle_t, rescatter), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_particle_type */


hid_t
h5fnal_create_mc_neutrino_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if((tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_neutrino_t))) < 0)
        H5FNAL_HDF5_ERROR

    if(H5Tinsert(tid, "mode", HOFFSET(h5fnal_mc_neutrino_t, mode), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "interaction_type", HOFFSET(h5fnal_mc_neutrino_t, interaction_type), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "ccnc", HOFFSET(h5fnal_mc_neutrino_t, ccnc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "target", HOFFSET(h5fnal_mc_neutrino_t, target), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "hit_nuc", HOFFSET(h5fnal_mc_neutrino_t, hit_nuc), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "hit_quark", HOFFSET(h5fnal_mc_neutrino_t, hit_quark), H5T_NATIVE_INT) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "w", HOFFSET(h5fnal_mc_neutrino_t, w), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "x", HOFFSET(h5fnal_mc_neutrino_t, x), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "y", HOFFSET(h5fnal_mc_neutrino_t, y), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tinsert(tid, "q_sqr", HOFFSET(h5fnal_mc_neutrino_t, q_sqr), H5T_NATIVE_DOUBLE) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_neutrino_type */

hid_t
h5fnal_create_origin_enum_type(void)
{
    int enum_value;
    hid_t tid = H5FNAL_BAD_HID_T;

    if((tid = H5Tenum_create(H5T_NATIVE_INT)) < 0)
        H5FNAL_HDF5_ERROR

    enum_value = 0;
    if(H5Tenum_insert(tid, "kUnknown", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 1;
    if(H5Tenum_insert(tid, "kBeamNeutrino", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 2;
    if(H5Tenum_insert(tid, "kCosmicRay", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 3;
    if(H5Tenum_insert(tid, "kSuperNovaParticle", &enum_value) < 0)
        H5FNAL_HDF5_ERROR
    enum_value = 4;
    if(H5Tenum_insert(tid, "kSingleParticle", &enum_value) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_origin_enum_type */

h5fnal_v_mc_truth_t
h5fnal_create_v_mc_truth(hid_t loc_id, const char *name)
{
    h5fnal_v_mc_truth_t vector;

    /* Create the top-level group for the vector */
    if((vector.top_level_group_id = H5Gcreate2(loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the datatypes */
    if((vector.neutrino_dtype_id = h5fnal_create_mc_neutrino_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if((vector.particle_dtype_id = h5fnal_create_mc_particle_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")
    if((vector.origin_enum_dtype_id = h5fnal_create_origin_enum_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype")

    return vector;

error:
    H5E_BEGIN_TRY {
        H5Gclose(vector.top_level_group_id);
        H5Tclose(vector.neutrino_dtype_id);
        H5Tclose(vector.particle_dtype_id);
        H5Tclose(vector.origin_enum_dtype_id);
    } H5E_END_TRY;

    vector.top_level_group_id = H5FNAL_BAD_HID_T;
    vector.neutrino_dtype_id = H5FNAL_BAD_HID_T;
    vector.particle_dtype_id = H5FNAL_BAD_HID_T;
    vector.origin_enum_dtype_id = H5FNAL_BAD_HID_T;

    return vector;
} /* h5fnal_create_v_mc_truth */

h5fnal_v_mc_truth_t
h5fnal_open_v_mc_truth(hid_t loc_id, const char *name)
{
    /* NOT IMPLEMENTED */
    h5fnal_v_mc_truth_t vector;
    return vector;
} /* h5fnal_open_v_mc_truth */

herr_t
h5fnal_close_v_mc_truth(h5fnal_v_mc_truth_t vector)
{
    if(H5Gclose(vector.top_level_group_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tclose(vector.neutrino_dtype_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Tclose(vector.particle_dtype_id) < 0)
        H5FNAL_HDF5_ERROR

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Gclose(vector.top_level_group_id);
        H5Tclose(vector.neutrino_dtype_id);
        H5Tclose(vector.particle_dtype_id);
        H5Tclose(vector.origin_enum_dtype_id);
    } H5E_END_TRY;

    vector.top_level_group_id = H5FNAL_BAD_HID_T;
    vector.neutrino_dtype_id = H5FNAL_BAD_HID_T;
    vector.particle_dtype_id = H5FNAL_BAD_HID_T;
    vector.origin_enum_dtype_id = H5FNAL_BAD_HID_T;

    return H5FNAL_FAILURE;
} /* h5fnal_close_v_mc_truth */


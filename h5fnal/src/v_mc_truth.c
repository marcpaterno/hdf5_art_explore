/* v_mc_truth.c */

#include "h5fnal.h"

/************/
/* MC TRUTH */
/************/

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


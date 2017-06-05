/* v_mc_truth.c */

#include "h5fnal.h"

/************/
/* MC TRUTH */
/************/

hid_t
h5fnal_create_mc_particle_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_particle_t));

    if(H5Tinsert(tid, "fSignalTime", HOFFSET(h5fnal_mc_hit_t, signal_time), H5T_NATIVE_FLOAT) < 0)
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

    tid = H5Tcreate(H5T_COMPOUND, sizeof(h5fnal_mc_particle_t));

    if(H5Tinsert(tid, "fSignalTime", HOFFSET(h5fnal_mc_hit_t, signal_time), H5T_NATIVE_FLOAT) < 0)
        H5FNAL_HDF5_ERROR

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* h5fnal_create_mc_neutrino_type */


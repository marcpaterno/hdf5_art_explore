/* h5fnal.h
 *
 * Public header file for Fermi data products library.
 */

#ifndef H5FNAL_H
#define H5FNAL_H

#include <hdf5.h>

/* INTERNAL ERROR MACROS
 *
 * The debug HDF5 library will dump the HDF5 function stack on errors
 * so we can guess what went wrong from that and don't need to add an
 * error string.
 *
 * Non-HDF5 general program errors emit a helpful error string.
 */
#define H5FNAL_ERROR_MSG        {fprintf(stderr, "***ERROR*** at line %d in function %s()...\n", __LINE__, __FUNCTION__);}
#define H5FNAL_HDF5_ERROR       {H5FNAL_ERROR_MSG goto error;}
#define H5FNAL_PROGRAM_ERROR(s) {H5FNAL_ERROR_MSG fprintf(stderr, "%s\n", (s)); goto error;}

/* Error type and values */
#define H5FNAL_SUCCESS      0
#define H5FNAL_FAILURE      (-1)

/* An invalid HDF5 ID */
#define H5FNAL_BAD_HID_T    (-1)

/* Product ID - Uniquely identifies data products */
typedef struct h5fnal_product_id_t {
    int         process_index;
    int         product_index;
} h5fnal_product_id_t;

/* Data type headers */
#include "v_mc_hit_collection.h"
#include "v_mc_truth.h"
#include "association.h"

/* h5fnal API */

#ifdef __cplusplus
extern "C" {
#endif

/* Run */
hid_t h5fnal_create_run(hid_t loc_id, const char *name);
hid_t h5fnal_open_run(hid_t loc_id, const char *name);
herr_t h5fnal_close_run(hid_t loc_id);

/* Event */
hid_t h5fnal_create_event(hid_t loc_id, const char *name);
hid_t h5fnal_open_event(hid_t loc_id, const char *name);
herr_t h5fnal_close_event(hid_t loc_id);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_H */

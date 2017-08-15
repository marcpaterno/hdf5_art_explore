/* util.h
 *
 * Header for internal utilities functions.
 */

#ifndef H5FNAL_UTIL_H
#define H5FNAL_UTIL_H

#include "h5fnal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Add and get string attributes to an HDF5 object */
herr_t h5fnal_add_string_attribute(hid_t loc_id, const char *name, const char *value);
herr_t h5fnal_get_string_attribute(hid_t loc_id, const char *name, char **value);

/* Append data to a dataset */
herr_t h5fnal_append_data(hid_t did, hid_t tid, hsize_t n_elements, const void *data);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_UTIL_H */


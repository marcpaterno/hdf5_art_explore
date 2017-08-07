/* util.h
 *
 * Header for internal utilities functions.
 */

#ifndef H5FNAL_UTIL_H
#define H5FNAL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif
herr_t h5fnal_add_string_attribute(hid_t loc_id, const char *name, const char *value);
herr_t h5fnal_get_string_attribute(hid_t loc_id, const char *name, char **value);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_UTIL_H */


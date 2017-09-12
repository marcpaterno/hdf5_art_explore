/* string_dictionary.h
 *
 * Header for an internal string dictionary.
 */

#ifndef H5FNAL_STRING_DICTIONARY_H
#define H5FNAL_STRING_DICTIONARY_H

#include "h5fnal.h"

typedef struct dict_index_t {
    hsize_t     start;
    hsize_t     end;
} dict_index_t;

typedef struct string_dictionary_t {
    /* HDF5 IDs */
    hid_t strings_dset_id;
    hid_t indices_dset_id;
    hid_t strings_dtype_id;
    hid_t indices_dtype_id;

    /* indices */
    unsigned n_strings;
    unsigned n_allocated;
    dict_index_t *indices;

    /* concatenated strings */
    size_t total_string_size;
    size_t total_string_alloc;
    char *concat_strings;

    /* The string collection is (cheaply) implements as write-once,
     * read-many, so we keep a 'created' flag to know if we need
     * to store the data on close.
     */
    hbool_t save_on_close;
} string_dictionary_t;

#ifdef __cplusplus
extern "C" {
#endif

herr_t create_string_dictionary(hid_t loc_id, string_dictionary_t *dict);
herr_t open_string_dictionary(hid_t loc_id, string_dictionary_t *dict);
herr_t close_string_dictionary(string_dictionary_t *dict);
herr_t add_string_to_dictionary(const char *s, string_dictionary_t *dict);
herr_t get_string_index(const char *s, string_dictionary_t *dict, /*OUT*/ hbool_t *found, /*OUT*/ unsigned *index);
herr_t get_string(string_dictionary_t *dict, unsigned index, /*OUT*/ char **s);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_STRING_DICTIONARY_H */


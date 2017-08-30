/* assns.h
 *
 * Public header file for the Assns (associations)
 * data product.
 */

#ifndef H5FNAL_ASSNS_H
#define H5FNAL_ASSNS_H

#include "h5fnal.h"

/* Pair data struct */
typedef struct h5fnal_association_t {
    uint16_t    left_process_index;
    uint16_t    left_product_index;
    uint64_t    left_key;

    uint16_t    right_process_index;
    uint16_t    right_product_index;
    uint64_t    right_key;
} h5fnal_pair_t;

/* In-memory Assns data.
 *
 * Used to hold data when performing dataset I/O. Data packing
 * and unpacking to the C++ Assns must be done by the caller
 * (presumably in a higher-level C++ library).
 *
 * Note that the pairs and data arrays have the same number
 * of elements.
 */
typedef struct h5fnal_assns_data_t {
    h5fnal_pair_t       *pairs;
    void                *data;
    hsize_t             n;
} h5fnal_assns_data_t;

/* Assns HDF5 data and related
 *
 * Contains HDF5 IDs for file objects that are a part of this
 * data product.
 *
 * left and right are the names of the data products on each side
 * of the pair.
 */
typedef struct h5fnal_assns_t {
    hid_t       top_level_group_id;
    hid_t       pair_dset_id;
    hid_t       pair_dtype_id;
    hid_t       data_dset_id;
    hid_t       data_dtype_id;
    char       *left;
    char       *right;
} h5fnal_assns_t;


#ifdef __cplusplus
extern "C" {
#endif
hid_t h5fnal_create_pair_type(void);

herr_t h5fnal_create_assns(hid_t loc_id, const char *name, const char *left, const char *right,
        hid_t data_datatype_id, h5fnal_assns_t *assns);
herr_t h5fnal_open_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns);
herr_t h5fnal_close_assns(h5fnal_assns_t *assns);

herr_t h5fnal_append_assns(h5fnal_assns_t *assns, h5fnal_assns_data_t *data);
herr_t h5fnal_read_all_assns(h5fnal_assns_t *assns, h5fnal_assns_data_t *data);

herr_t h5fnal_free_assns_mem_data(h5fnal_assns_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_ASSNS_H */


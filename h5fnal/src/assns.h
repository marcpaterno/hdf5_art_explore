/* assns.h
 *
 * Public header file for the Assns (associations)
 * data product.
 */

#ifndef H5FNAL_ASSNS_H
#define H5FNAL_ASSNS_H

/* Association Type */
typedef struct h5fnal_association_t {
    uint16_t    left_process_index;
    uint16_t    left_product_index;
    uint64_t    left_key;

    uint16_t    right_process_index;
    uint16_t    right_product_index;
    uint64_t    right_key;
} h5fnal_association_t;

/* Assns Type */
typedef struct h5fnal_assns_t {
    hid_t       top_level_group_id;
    hid_t       association_dataset_id;
    hid_t       association_datatype_id;
    hid_t       data_dataset_id;
    hid_t       data_datatype_id;
} h5fnal_assns_t;


#ifdef __cplusplus
extern "C" {
#endif
hid_t h5fnal_create_association_type(void);

herr_t h5fnal_create_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns, hid_t data_datatype_id);
herr_t h5fnal_open_assns(hid_t loc_id, const char *name, h5fnal_assns_t *assns);
herr_t h5fnal_close_assns(h5fnal_assns_t *assns);

herr_t h5fnal_append_assns(h5fnal_assns_t *assns, size_t n_assns, h5fnal_association_t *associations, void *data);
hssize_t h5fnal_get_assns_count(h5fnal_assns_t *assns);
herr_t h5fnal_read_all_assns(h5fnal_assns_t *assns, h5fnal_association_t *associations, void *data);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_ASSNS_H */


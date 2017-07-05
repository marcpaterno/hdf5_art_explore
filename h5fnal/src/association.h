/* association.h
 *
 * Public header file for the Association
 * data product.
 */

#ifndef ASSOCIATION_H
#define ASSOCIATION_H

typedef struct h5fnal_association_t {
    hid_t       top_level_group_id;
} h5fnal_association_t;


#ifdef __cplusplus
extern "C" {
#endif

herr_t h5fnal_create_association(hid_t loc_id, const char *name, h5fnal_association_t *assn);
herr_t h5fnal_open_association(hid_t loc_id, const char *name, h5fnal_association_t *assn);
herr_t h5fnal_close_association(h5fnal_association_t *assn);

#ifdef __cplusplus
}
#endif

#endif /* ASSOCIATION_H */


/* v_mc_hit_collection.h
 *
 * Public header file for the Vector of MC Hit Collection
 * data product.
 */

#ifndef V_MC_HIT_COLLECTION_H
#define V_MC_HIT_COLLECTION_H


/* MC Hit type
 *
 * The struct members correspond to the private data of the
 * MCHit class.
 */
typedef struct h5fnal_hit_t {
    float       signal_time;
    float       signal_width;
    float       peak_amp;
    float       charge;
    float       part_vertex_x;
    float       part_vertex_y;
    float       part_vertex_z;
    float       part_energy;
    int         part_track_id;
} h5fnal_hit_t;


/* MC Hit Collection type
 *
 * channel corresponds to fChannel in the MCHitCollection class.
 *
 * start is the index into the corresponding hit array. This will
 * be fixed up in the append() call when writing to a non-empty
 * data product.
 *
 * Set both start and count to zero to represent a hit collection
 * that contains zero hits for that channel.
 */
typedef struct h5fnal_hitcoll_t {
    unsigned    channel;
    hsize_t     start;
    hsize_t     count;
} h5fnal_hitcoll_t;


/* In-memory Vector of MC Hit Collection data.
 *
 * Used to hold data when performing dataset I/O. Data packing
 * and unpacking to the vector<MCHitCollection> must be done
 * by the caller (presumably in a higher-level C++ library).
 */
typedef struct h5fnal_vect_hitcoll_data_t {
    h5fnal_hit_t        *hits;
    hsize_t             n_hits;
    h5fnal_hitcoll_t    *hit_collections;
    hsize_t             n_hit_collections;
} h5fnal_vect_hitcoll_data_t;


/* Vector of MC Hit Collection  HDF5 data and related
 *
 * Contains HDF5 IDs for file objects that are a part of this
 * data product.
 */
typedef struct h5fnal_vect_hitcoll_t {
    hid_t       top_level_group_id;
    hid_t       hit_dset_id;
    hid_t       hit_dtype_id;
    hid_t       hitcoll_dset_id;
    hid_t       hitcoll_dtype_id;
} h5fnal_vect_hitcoll_t;


#ifdef __cplusplus
extern "C" {
#endif

hid_t h5fnal_create_hit_type(void);
hid_t h5fnal_create_hitcoll_type(void);

herr_t h5fnal_create_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_vect_hitcoll_t *vector);
herr_t h5fnal_open_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_vect_hitcoll_t *vector);
herr_t h5fnal_close_v_mc_hit_collection(h5fnal_vect_hitcoll_t *vector);

herr_t h5fnal_append_hits(h5fnal_vect_hitcoll_t *vector, h5fnal_vect_hitcoll_data_t *data);
herr_t h5fnal_read_all_hits(h5fnal_vect_hitcoll_t *vector, h5fnal_vect_hitcoll_data_t *data);

herr_t h5fnal_free_hitcoll_mem_data(h5fnal_vect_hitcoll_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* V_MC_HIT_COLLECTION_H */


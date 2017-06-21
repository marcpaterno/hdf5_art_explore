/* v_mc_hit_collection.h
 *
 * Public header file for the Vector of MC Hit Collection
 * data product.
 */

#ifndef V_MC_HIT_COLLECTION_H
#define V_MC_HIT_COLLECTION_H

/* MC Hit Type */
typedef struct h5fnal_mc_hit_t {
    float       signal_time;
    float       signal_width;
    float       peak_amp;
    float       charge;
    float       part_vertex_x;
    float       part_vertex_y;
    float       part_vertex_z;
    float       part_energy;
    int         part_track_id;
    unsigned    channel;
} h5fnal_mc_hit_t;

/* Vector of MC Hit Collection Type */
typedef struct h5fnal_v_mc_hit_coll_t {
    hid_t       top_level_group_id;
    hid_t       dataset_id;
    hid_t       datatype_id;
} h5fnal_v_mc_hit_coll_t;


#ifdef __cplusplus
extern "C" {
#endif

hid_t h5fnal_create_mc_hit_type(void);

herr_t h5fnal_create_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_v_mc_hit_coll_t *vector);
herr_t h5fnal_open_v_mc_hit_collection(hid_t loc_id, const char *name, h5fnal_v_mc_hit_coll_t *vector);
herr_t h5fnal_close_v_mc_hit_collection(h5fnal_v_mc_hit_coll_t *vector);

herr_t h5fnal_write_hits(h5fnal_v_mc_hit_coll_t *vector, size_t n_hits, h5fnal_mc_hit_t *hits);
hssize_t h5fnal_get_hits_count(h5fnal_v_mc_hit_coll_t *vector);
herr_t h5fnal_read_all_hits(h5fnal_v_mc_hit_coll_t *vector, h5fnal_mc_hit_t *hits);

#ifdef __cplusplus
}
#endif

#endif /* V_MC_HIT_COLLECTION_H */


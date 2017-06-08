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
    hid_t       dataset_id;
    hid_t       datatype_id;
} h5fnal_v_mc_hit_coll_t;

/* MC Neutrino Type */
typedef struct h5fnal_mc_neutrino_t {
    int         mode;
    int         interaction_type;
    int         ccnc;
    int         target;
    int         hit_nuc;
    int         hit_quark;
    double      w;
    double      x;
    double      y;
    double      q_sqr;
} h5fnal_mc_neutrino_t;

/* MC Particle Type */
typedef struct h5fnal_mc_particle_t {
    int         status;
    int         track_id;
    int         pdg_code;
    int         mother;
    char        *process;
    char        *end_process;
    double      mass;
    double      polarization_x;
    double      polarization_y;
    double      polarization_z;
    double      weight;
    double      gvtx_e;
    double      gvtx_x;
    double      gvtx_y;
    double      gvtx_z;
    int         rescatter;
} h5fnal_mc_particle_t;

/* Vector of MC Hit Collection Type */
typedef struct h5fnal_v_mc_truth_t {
    hid_t       neutrino_dtype_id;
    hid_t       particle_dtype_id;
    hid_t       origin_enum_dtype_id;
    hid_t       top_level_group_id;
} h5fnal_v_mc_truth_t;

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

/* MC Hit Collection */
hid_t h5fnal_create_v_mc_hit_collection_type(void);
h5fnal_v_mc_hit_coll_t h5fnal_create_v_mc_hit_collection(hid_t loc_id, const char *name);
h5fnal_v_mc_hit_coll_t h5fnal_open_v_mc_hit_collection(hid_t loc_id, const char *name);
herr_t h5fnal_close_v_mc_hit_collection(h5fnal_v_mc_hit_coll_t vector);

herr_t h5fnal_write_hits(h5fnal_v_mc_hit_coll_t vector, size_t n_hits, h5fnal_mc_hit_t *hits);
hssize_t h5fnal_get_hits_count(h5fnal_v_mc_hit_coll_t vector);
herr_t h5fnal_read_all_hits(h5fnal_v_mc_hit_coll_t vector, h5fnal_mc_hit_t *hits);

/* MC Truth */
hid_t h5fnal_create_mc_neutrino_type(void);
hid_t h5fnal_create_mc_particle_type(void);
hid_t h5fnal_create_origin_enum_type(void);
h5fnal_v_mc_truth_t h5fnal_create_v_mc_truth(hid_t loc_id, const char *name);
h5fnal_v_mc_truth_t h5fnal_open_v_mc_truth(hid_t loc_id, const char *name);
herr_t h5fnal_close_v_mc_truth(h5fnal_v_mc_truth_t vector);


#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_H */

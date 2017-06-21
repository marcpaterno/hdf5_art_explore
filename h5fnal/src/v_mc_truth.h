/* v_mc_hit_truth.h
 *
 * Public header file for the Vector of MC Truth
 * data product.
 */

#ifndef V_MC_TRUTH_H
#define V_MC_TRUTH_H

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

/* Neutrino origin enum */
typedef enum h5fnal_origin_t {
    UNKNOWN             = 0,
    BEAM_NEUTRINO       = 1,
    COSMIC_RAY          = 2,
    SUPERNOVA_NEUTRINO  = 3,
    SINGLE_PARTICLE     = 4
} h5fnal_origin_t;

/* Vector of MC Hit Truth Type */
typedef struct h5fnal_v_mc_truth_t {
    hid_t       top_level_group_id;
    hid_t       neutrino_dtype_id;
    hid_t       particle_dtype_id;
    hid_t       origin_enum_dtype_id;
} h5fnal_v_mc_truth_t;

#ifdef __cplusplus
extern "C" {
#endif

hid_t h5fnal_create_mc_neutrino_type(void);
hid_t h5fnal_create_mc_particle_type(void);
hid_t h5fnal_create_origin_enum_type(void);

herr_t h5fnal_create_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector);
herr_t h5fnal_open_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector);
herr_t h5fnal_close_v_mc_truth(h5fnal_v_mc_truth_t *vector);

#ifdef __cplusplus
}
#endif

#endif /* V_MC_HIT_TRUTH_H */


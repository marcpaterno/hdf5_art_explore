/* v_mc_hit_truth.h
 *
 * Public header file for the Vector of MC Truth
 * data product.
 */

#ifndef V_MC_TRUTH_H
#define V_MC_TRUTH_H

#include "h5fnal.h"

/* Strings used in this data product */
typedef struct h5fnal_truth_strings_t {
    char      **strings;
    hsize_t     n_strings;
    hsize_t     n_allocated;
} h5fnal_truth_strings_t;

/* Neutrino origin enum */
typedef enum h5fnal_origin_t {
    UNKNOWN             = 0,
    BEAM_NEUTRINO       = 1,
    COSMIC_RAY          = 2,
    SUPERNOVA_NEUTRINO  = 3,
    SINGLE_PARTICLE     = 4
} h5fnal_origin_t;

/* MC Neutrino Type */
typedef struct h5fnal_neutrino_t {
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
    hsize_t     nu;     // Index into particles
    hsize_t     lepton; // Index into particles
} h5fnal_neutrino_t;

/* MC Particle Type */
typedef struct h5fnal_particle_t {
    int         status;
    int         track_id;
    int         pdg_code;
    int         mother;
    hsize_t     process_index;      /* into h5fnal_truth_strings_t */
    hsize_t     endprocess_index;   /* into h5fnal_truth_strings_t */
    double      mass;
    double      polarization_x;
    double      polarization_y;
    double      polarization_z;
    double      weight;
    double      gvtx_x;
    double      gvtx_y;
    double      gvtx_z;
    double      gvtx_t;
    int         rescatter;
} h5fnal_particle_t;

/* Daughters type (for parent-child relationships) */
typedef struct h5fnal_daughter_t {
    hsize_t     parent_index;
    hsize_t     child_index;
} h5fnal_daughter_t;

/* Trajectory type */
typedef struct h5fnal_trajectory_t {
    double      Vx;
    double      Vy;
    double      Vz;
    double      T;
    double      Px;
    double      Py;
    double      Pz;
    double      E;
    hsize_t     particle_index;
} h5fnal_trajectory_t;

/* MC Truth Type */
typedef struct h5fnal_truth_t {
    h5fnal_origin_t     origin;
    hssize_t    neutrino_index;     /* -1 == no neutrino */
    hsize_t     particle_start_index;
    hsize_t     particle_end_index;
    hsize_t     trajectory_start_index;
    hsize_t     trajectory_end_index;
    hsize_t     daughters_start_index;
    hsize_t     daughters_end_index;
} h5fnal_truth_t;

/* Vector of MC Truth Type */
typedef struct h5fnal_vect_truth_t {
    hid_t       top_level_group_id;

    hid_t       origin_dtype_id;

    hid_t       neutrino_dtype_id;
    hid_t       neutrino_dset_id;

    hid_t       particle_dtype_id;
    hid_t       particle_dset_id;

    hid_t       daughter_dtype_id;
    hid_t       daughter_dset_id;

    hid_t       trajectory_dtype_id;
    hid_t       trajectory_dset_id;

    hid_t       truth_dtype_id;
    hid_t       truth_dset_id;

    string_dictionary_t dict;
} h5fnal_vect_truth_t;

/* In-memory data container for I/O calls */
typedef struct h5fnal_vect_truth_data_t {
    hsize_t                 n_truths;
    hsize_t                 n_trajectories;
    hsize_t                 n_daughters;
    hsize_t                 n_particles;
    hsize_t                 n_neutrinos;
    h5fnal_truth_t         *truths;
    h5fnal_trajectory_t    *trajectories;
    h5fnal_daughter_t      *daughters;
    h5fnal_particle_t      *particles;
    h5fnal_neutrino_t      *neutrinos;
    h5fnal_truth_strings_t *truth_strings;
} h5fnal_vect_truth_data_t;

#ifdef __cplusplus
extern "C" {
#endif

hid_t h5fnal_create_origin_type(void);
hid_t h5fnal_create_neutrino_type(void);
hid_t h5fnal_create_particle_type(void);
hid_t h5fnal_create_daughter_type(void);
hid_t h5fnal_create_trajectory_type(void);
hid_t h5fnal_create_truth_type(void);

herr_t h5fnal_create_v_mc_truth(hid_t loc_id, const char *name, h5fnal_vect_truth_t *vector);
herr_t h5fnal_open_v_mc_truth(hid_t loc_id, const char *name, h5fnal_vect_truth_t *vector);
herr_t h5fnal_close_v_mc_truth(h5fnal_vect_truth_t *vector);

herr_t h5fnal_append_truths(h5fnal_vect_truth_t *vector, h5fnal_vect_truth_data_t *data);
herr_t h5fnal_read_all_truths(h5fnal_vect_truth_t *vector, h5fnal_vect_truth_data_t *data);

herr_t h5fnal_free_truth_mem_data(h5fnal_vect_truth_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* V_MC_TRUTH_H */


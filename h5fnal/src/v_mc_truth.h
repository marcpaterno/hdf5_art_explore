/* v_mc_hit_truth.h
 *
 * Public header file for the Vector of MC Truth
 * data product.
 */

#ifndef V_MC_TRUTH_H
#define V_MC_TRUTH_H

/* Neutrino origin enum */
typedef enum h5fnal_origin_t {
    UNKNOWN             = 0,
    BEAM_NEUTRINO       = 1,
    COSMIC_RAY          = 2,
    SUPERNOVA_NEUTRINO  = 3,
    SINGLE_PARTICLE     = 4
} h5fnal_origin_t;

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
    hsize_t     process_index;
    hsize_t     endprocess_index;
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

/* Daughters type (for parent-child relationships) */
typedef struct h5fnal_daughter_t {
    hsize_t     parent_index;
    hsize_t     child_index;
} h5fnal_daughter_t;

/* Trajectory type */
typedef struct h5fnal_mc_trajectory_t {
    double      Ec1;
    double      px1;
    double      py1;
    double      pz1;
    double      Ec2;
    double      px2;
    double      py2;
    double      pz2;
    hsize_t     particle_index;
} h5fnal_mc_trajectory_t;

/* MC Truth Type */
typedef struct h5fnal_mc_truth_t {
    hssize_t    neutrino_index;     /* -1 == no neutrino */
    hsize_t     particle_start_index;
    hsize_t     particle_end_index;
    hsize_t     trajectory_start_index;
    hsize_t     trajectory_end_index;
    hsize_t     daughters_start_index;
    hsize_t     daughters_end_index;
} h5fnal_mc_truth_t;

/* Vector of MC Truth Type */
typedef struct h5fnal_v_mc_truth_t {
    hid_t       top_level_group_id;
    hid_t       origin_enum_dtype_id;
    hid_t       neutrino_dtype_id;
    hid_t       particle_dtype_id;
    hid_t       daughter_dtype_id;
    hid_t       trajectory_dtype_id;
    hid_t       truth_dtype_id;
    /* string dictionary pointer goes here */
    hid_t       truth_dataset_id;
    hid_t       neutrino_dataset_id;
    hid_t       particle_dataset_id;
    hid_t       daughter_dataset_id;
    hid_t       trajectory_dataset_id;
} h5fnal_v_mc_truth_t;

/* In-memory data container for I/O calls */
typedef struct h5fnal_truth_mem_t {
    hsize_t                 n_truths;
    h5fnal_mc_truth_t       *truths;
    h5fnal_mc_trajectory_t  *trajectories;
    h5fnal_daughter_t       *daughters;
    h5fnal_mc_particle_t    *particles;
    h5fnal_mc_neutrino_t    *neutrinos;
} h5fnal_truth_mem_t;

#ifdef __cplusplus
extern "C" {
#endif

hid_t h5fnal_create_origin_enum_type(void);
hid_t h5fnal_create_mc_neutrino_type(void);
hid_t h5fnal_create_mc_particle_type(void);
hid_t h5fnal_create_daughter_type(void);
hid_t h5fnal_create_mc_trajectory_type(void);
hid_t h5fnal_create_mc_truth_type(void);

herr_t h5fnal_create_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector);
herr_t h5fnal_open_v_mc_truth(hid_t loc_id, const char *name, h5fnal_v_mc_truth_t *vector);
herr_t h5fnal_close_v_mc_truth(h5fnal_v_mc_truth_t *vector);

herr_t h5fnal_append_truths(h5fnal_v_mc_truth_t *vector, h5fnal_truth_mem_t *truths);
hssize_t h5fnal_get_truths_count(h5fnal_v_mc_truth_t *vector);
herr_t h5fnal_read_all_truths(h5fnal_v_mc_truth_t *vector, h5fnal_truth_mem_t *truths);

#ifdef __cplusplus
}
#endif

#endif /* V_MC_HIT_TRUTH_H */


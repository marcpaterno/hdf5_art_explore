/* Test the vector of MC Truth API */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "v_mc_truth.h5"
#define RUN_NAME    "testrun"
#define SUBRUN_NAME "testsubrun"
#define EVENT_NAME  "testevent"
#define VECTOR_NAME "vomct"


/* generates fake MC Truth data for testing */
static herr_t
generate_fake_truths(h5fnal_mem_truth_t *truths)
{
    if (!truths)
        H5FNAL_PROGRAM_ERROR("truths parameter cannot be NULL");

    srand((unsigned int)time(NULL));

    /* Set the number of elements to create in each dataset */
    truths->n_truths = 2;
    truths->n_trajectories = 10;
    truths->n_daughters = 4;
    truths->n_particles = 10;
    truths->n_neutrinos = 1;

    /* Allocate buffers for data */
    if (NULL == (truths->truths = (h5fnal_mc_truth_t *)calloc(truths->n_truths, sizeof(h5fnal_mc_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory")
    if (NULL == (truths->trajectories = (h5fnal_mc_trajectory_t *)calloc(truths->n_trajectories, sizeof(h5fnal_mc_trajectory_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory")
    if (NULL == (truths->daughters = (h5fnal_daughter_t *)calloc(truths->n_daughters, sizeof(h5fnal_daughter_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory")
    if (NULL == (truths->particles = (h5fnal_mc_particle_t *)calloc(truths->n_particles, sizeof(h5fnal_mc_particle_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory")
    if (NULL == (truths->neutrinos = (h5fnal_mc_neutrino_t *)calloc(truths->n_neutrinos, sizeof(h5fnal_mc_neutrino_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory")

    /* Add data */

    return H5FNAL_SUCCESS;

error:

    if (truths)
        h5fnal_free_mem_truths(truths);

    return H5FNAL_FAILURE;

} /* end generate_fake_truths() */

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_v_mc_truth_t *vector = NULL;
    hssize_t n_truths_out = 0;
    h5fnal_mem_truth_t *truths = NULL;
    h5fnal_mem_truth_t *truths_out = NULL;

    printf("Testing vector of MC Truth operations... ");

    /* Create the file */
    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR
    if ((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the run, sub-run, and event */
    if ((run_id = h5fnal_create_run(fid, RUN_NAME, FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run")
    if ((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME, FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run")
    if ((event_id = h5fnal_create_event(subrun_id, EVENT_NAME, TRUE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event")

    /* Create the vector of MC truth data product */
    if (NULL == (vector = calloc(1, sizeof(h5fnal_v_mc_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector")
    if (h5fnal_create_v_mc_truth(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not create vector of mc truth")

    /* It should have zero size when created */
    if ((n_truths_out = h5fnal_get_truths_count(vector)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get number of truths from dataset")
    if (n_truths_out != 0)
        H5FNAL_PROGRAM_ERROR("got wrong number of truths from dataset after creation")

    /* Generate some fake data */
    if (NULL == (truths = calloc(1, sizeof(h5fnal_mem_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for in-memory truth data container")
    if (generate_fake_truths(truths) < 0)
        H5FNAL_PROGRAM_ERROR("problem generating fake data for testing")

    /* Append truths */
    if (h5fnal_append_truths(vector, truths) < 0)
        H5FNAL_PROGRAM_ERROR("could not write truths to the file")
    if (h5fnal_append_truths(vector, truths) < 0)
        H5FNAL_PROGRAM_ERROR("could not write truths to the file")

    /* Get the number of truths */
    if ((n_truths_out = h5fnal_get_truths_count(vector)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get number of truths")
    if (n_truths_out != 2 * truths->n_truths)
        H5FNAL_PROGRAM_ERROR("got wrong number of truths")

    /* Read the truths */
    if (NULL == (truths_out = calloc(1, sizeof(h5fnal_mem_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for in-memory truth data container")
    if (h5fnal_read_all_truths(vector, truths_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read truths from the file")

    /* Close the vector */
    if (h5fnal_close_v_mc_truth(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")

    /* Re-open the vector */
    if (h5fnal_open_v_mc_truth(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open vector of mc truth")

    /* Get the number of truths */
    if ((n_truths_out = h5fnal_get_truths_count(vector)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get number of truths")
    if (n_truths_out != 2 * truths->n_truths)
        H5FNAL_PROGRAM_ERROR("got wrong number of truths")

    /* Re-read the truths */
    if (h5fnal_free_mem_truths(truths_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up read data")
    if (h5fnal_read_all_truths(vector, truths_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read truths from the file")

    /* Close the vector */
    if (h5fnal_close_v_mc_truth(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")

    /* Close everything else */
    free(vector);
    if (h5fnal_free_mem_truths(truths) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up fake data")
    free(truths);
    if (h5fnal_free_mem_truths(truths_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up read data")
    free(truths_out);
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        if (vector) {
            h5fnal_close_v_mc_truth(vector);
            free(vector);
        }
        if (truths) {
            h5fnal_free_mem_truths(truths);
            free(truths);
        }
        if (truths_out) {
            h5fnal_free_mem_truths(truths_out);
            free(truths_out);
        }
        h5fnal_close_run(subrun_id);
        h5fnal_close_run(run_id);
        h5fnal_close_event(event_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


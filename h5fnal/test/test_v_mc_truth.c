/* Test the vector of MC Truth API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "v_mc_truth.h5"
#define RUN_NAME    "testrun"
#define SUBRUN_NAME "testsubrun"
#define EVENT_NAME  "testevent"
#define VECTOR_NAME "vomct"


/* generates random MC Truth data for testing */
static herr_t
generate_test_truths(h5fnal_vect_truth_data_t *data)
{
    if (!data)
        H5FNAL_PROGRAM_ERROR("data parameter cannot be NULL");

    /* Initialize the data struct */
    memset(data, 0, sizeof(h5fnal_vect_truth_data_t));

    /* If you want to get deterministic behavior, change NULL to an integer */
    srand((unsigned int)time(NULL));

    /* Set the number of elements to create in each dataset */
    data->n_truths = 2;
    data->n_trajectories = 10;
    data->n_daughters = 4;
    data->n_particles = 10;
    data->n_neutrinos = 1;

    /* Allocate buffers for data */
    if (NULL == (data->truths = (h5fnal_truth_t *)calloc(data->n_truths, sizeof(h5fnal_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");
    if (NULL == (data->trajectories = (h5fnal_trajectory_t *)calloc(data->n_trajectories, sizeof(h5fnal_trajectory_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");
    if (NULL == (data->daughters = (h5fnal_daughter_t *)calloc(data->n_daughters, sizeof(h5fnal_daughter_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");
    if (NULL == (data->particles = (h5fnal_particle_t *)calloc(data->n_particles, sizeof(h5fnal_particle_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");
    if (NULL == (data->neutrinos = (h5fnal_neutrino_t *)calloc(data->n_neutrinos, sizeof(h5fnal_neutrino_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory");

    /* Add data */

    return H5FNAL_SUCCESS;

error:

    if (data)
        h5fnal_free_truth_mem_data(data);

    return H5FNAL_FAILURE;

} /* end generate_test_truths() */

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_vect_truth_t *vector = NULL;
    h5fnal_vect_truth_data_t *data = NULL;
    h5fnal_vect_truth_data_t *data_out = NULL;

    printf("Testing vector of MC Truth operations... ");

    /* Create the file */
    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR;
    if ((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the run, sub-run, and event */
    if ((run_id = h5fnal_create_run(fid, RUN_NAME, FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run");
    if ((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME, FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");
    if ((event_id = h5fnal_create_event(subrun_id, EVENT_NAME, TRUE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event");

    /* Create the vector of MC truth data product */
    if (NULL == (vector = (h5fnal_vect_truth_t *)calloc(1, sizeof(h5fnal_vect_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector");
    if (h5fnal_create_v_mc_truth(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not create vector of mc truth");

    /* Generate some test data */
    if (NULL == (data = (h5fnal_vect_truth_data_t *)calloc(1, sizeof(h5fnal_vect_truth_data_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for in-memory truth data container");
    if (generate_test_truths(data) < 0)
        H5FNAL_PROGRAM_ERROR("problem generating data for testing");

    /* Append truths */
    if (h5fnal_append_truths(vector, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write truths to the file");
    if (h5fnal_append_truths(vector, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write truths to the file");

    /* Read the truths */
    if (NULL == (data_out = (h5fnal_vect_truth_data_t *)calloc(1, sizeof(h5fnal_vect_truth_data_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for in-memory truth data container");
    if (h5fnal_read_all_truths(vector, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read truths from the file");

    /* Close the vector */
    if (h5fnal_close_v_mc_truth(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector");

    /* Re-open the vector */
    if (h5fnal_open_v_mc_truth(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open vector of mc truth");

    /* Re-read the truths */
    if (h5fnal_free_truth_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up test data");
    if (h5fnal_read_all_truths(vector, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read truths from the file");

    /* Close the vector */
    if (h5fnal_close_v_mc_truth(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector");

    /* Close everything else */
    free(vector);

    if (h5fnal_free_truth_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up test data");
    free(data);

    if (h5fnal_free_truth_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not clean up read data");
    free(data_out);

    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run");
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run");
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run");
    if (H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR;

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        if (vector) {
            h5fnal_close_v_mc_truth(vector);
            free(vector);
        }
        if (data) {
            h5fnal_free_truth_mem_data(data);
            free(data);
        }
        if (data_out) {
            h5fnal_free_truth_mem_data(data_out);
            free(data_out);
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


/* Test the Assns API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "assns.h5"
#define RUN_NAME    "testrun"
#define SUBRUN_NAME "testsubrun"
#define EVENT_NAME  "testevent"
#define ASSNS_NAME  "assns"

h5fnal_assns_t *
generate_fake_assns(unsigned int n_assns)
{
    unsigned int u;
    h5fnal_assns_t *assns = NULL;

    srand((unsigned int)time(NULL));

    if (NULL == (assns = (h5fnal_assns_t *)calloc(n_assns, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could allocate memory for hits")

    for (u = 0; u < n_assns; u++) {
        assns[u].left_process_index = (uint16_t)rand();
        assns[u].left_product_index = (uint16_t)rand();
        assns[u].left_key = (uint64_t)rand();

        assns[u].right_process_index = (uint16_t)rand();
        assns[u].right_product_index = (uint16_t)rand();
        assns[u].right_key = (uint64_t)rand();
    }

    return assns;

error:

    free(assns);
    return NULL;

} /* end generate_fake_assns() */

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_assns_t *assns = NULL;

    printf("Testing Assns operations... ");

    /* Create the file */
    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR
    if ((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the run, sub-run, and event */
    if ((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run")
    if ((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run")
    if ((event_id = h5fnal_create_event(subrun_id, EVENT_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event")

    /* Create the assns data product */
    if (NULL == (assns = calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns")
    if (h5fnal_create_assns(event_id, ASSNS_NAME, assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not create assns data product")

    /* Close the assns data product */
    if (h5fnal_close_assns(assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns")

    /* Re-open the assns data product */
    if (h5fnal_open_assns(event_id, ASSNS_NAME, assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not open assns data product")

    /* Close the assns data product */
    if (h5fnal_close_assns(assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns")

    /* Close boilerplate */
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event")
    if (H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR
    if (H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


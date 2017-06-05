/* Test the vector of MC Truth API */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "v_mc_truth.h5"
#define RUN_NAME    "testrun"
#define EVENT_NAME  "testevent"
#define VECTOR_NAME "vomct"

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   event_id = -1;

    printf("Testing vector of MC Truth operations... ");

    /* Create the file */
    if((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR
    if((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the run and event */
    if((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run")
    if((event_id = h5fnal_create_event(run_id, EVENT_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event")

    /* Close everything */
    if(h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if(h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if(H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        h5fnal_close_run(run_id);
        h5fnal_close_event(event_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


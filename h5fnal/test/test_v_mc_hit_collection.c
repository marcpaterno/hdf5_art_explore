/* Test the vector of MC Hit Collection API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "v_mc_hc.h5"
#define RUN_NAME    "testrun"
#define SUBRUN_NAME "testsubrun"
#define EVENT_NAME  "testevent"
#define VECTOR_NAME "vomchc"

h5fnal_mc_hit_t *
generate_fake_hits(size_t n_hits)
{
    size_t i;
    h5fnal_mc_hit_t *hits = NULL;

    srand((unsigned int)time(NULL));

    if(NULL == (hits = (h5fnal_mc_hit_t *)calloc(n_hits, sizeof(h5fnal_mc_hit_t))))
        H5FNAL_PROGRAM_ERROR("could allocate memory for hits")

    for(i=0; i < n_hits; i++) {
        hits[i].signal_time = (float)rand();
        hits[i].signal_width = (float)rand();
        hits[i].peak_amp = (float)rand();
        hits[i].charge = (float)rand();
        hits[i].part_vertex_x = (float)rand();
        hits[i].part_vertex_y = (float)rand();
        hits[i].part_vertex_z = (float)rand();
        hits[i].part_energy = (float)rand();
        hits[i].part_track_id = (int)rand();
        hits[i].channel = (unsigned)rand();
    }

    return hits;

error:

    free(hits);
    return NULL;

} /* end generate_fake_hits() */

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_v_mc_hit_coll_t *vector = NULL;
    size_t n_hits;
    h5fnal_mc_hit_t *hits = NULL;
    h5fnal_mc_hit_t *hits_out = NULL;
    hssize_t n_hits_out = 0;
    size_t u;

    printf("Testing vector of MC Hit Collection operations... ");

    /* Create the file */
    if((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR
    if((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR

    /* Create the run, sub-run, and event */
    if((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run")
    if((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run")
    if((event_id = h5fnal_create_event(subrun_id, EVENT_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event")

    /* Create the vector of MC hit collection data product */
    if(NULL == (vector = calloc(1, sizeof(h5fnal_v_mc_hit_coll_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector")
    if(h5fnal_create_v_mc_hit_collection(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not create vector of mc hit collection")

    /* Generate some fake data */
    n_hits = 16384;
    if(NULL == (hits = generate_fake_hits(n_hits)))
        H5FNAL_PROGRAM_ERROR("unable to create fake hit data")

    /* Write some hits to it */
    if(h5fnal_write_hits(vector, n_hits, hits) < 0)
        H5FNAL_PROGRAM_ERROR("could not write hits to the file")
    if(h5fnal_write_hits(vector, n_hits, hits) < 0)
        H5FNAL_PROGRAM_ERROR("could not write hits to the file")

    /* Get the number of hits */
    if((n_hits_out = h5fnal_get_hits_count(vector)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get number of hits from dataset")
    if(n_hits_out != 2 * n_hits)
        H5FNAL_PROGRAM_ERROR("got wrong number of hits from dataset")

    /* Generate a buffer for reading the hits */
    if(NULL == (hits_out = (h5fnal_mc_hit_t *)calloc(n_hits_out, sizeof(h5fnal_mc_hit_t))))
        H5FNAL_PROGRAM_ERROR("could allocate memory for hits_out")

    /* Read the hits */
    if(h5fnal_read_all_hits(vector, hits_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read hits from the file")

    /* Compare the written and read data */
    for(u = 0; u < n_hits; u++)
        if(hits[u].part_track_id != hits_out[u + n_hits].part_track_id)
            H5FNAL_PROGRAM_ERROR("bad read data")

    /* Close the vector */
    if(h5fnal_close_v_mc_hit_collection(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")

    /* Re-open the vector */
    if(h5fnal_open_v_mc_hit_collection(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open vector of mc hit collection")

    /* Re-read the hits */
    memset(hits_out, 0, n_hits_out * sizeof(h5fnal_mc_hit_t));
    if(h5fnal_read_all_hits(vector, hits_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read hits from the file")

    /* Compare the written and read data */
    for(u = 0; u < n_hits; u++)
        if(hits[u].part_track_id != hits_out[u + n_hits].part_track_id)
            H5FNAL_PROGRAM_ERROR("bad read data")

    /* Close the vector */
    if(h5fnal_close_v_mc_hit_collection(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")

    /* Close everything */
    free(hits);
    free(hits_out);
    free(vector);
    if(h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if(h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if(h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event")
    if(H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR
    if(H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        free(hits);
        free(hits_out);
        if(vector) {
            h5fnal_close_v_mc_hit_collection(vector);
            free(vector);
        }
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


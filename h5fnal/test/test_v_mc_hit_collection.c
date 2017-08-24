/* Test the vector of MC Hit Collection API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME   "vmchc.h5"
#define RUN_NAME    "test_run"
#define SUBRUN_NAME "test_subrun"
#define EVENT_NAME  "test_event"
#define VECTOR_NAME "test_hit_collection"

h5fnal_vect_hitcoll_data_t *
generate_test_hit_collections(hsize_t n_hit_collections)
{
    unsigned start_channel;
    hsize_t hc;
    hsize_t first_hit;
    h5fnal_vect_hitcoll_data_t *data = NULL;

    /* If you want to get deterministic behavior, change NULL to an integer */
    srand((unsigned int)time(NULL));

    /* Pick a starting channel */
    start_channel = (int)(rand() % 4096);

    /* Get memory for the top-level data container */
    if (NULL == (data = (h5fnal_vect_hitcoll_data_t *)calloc((size_t)1, sizeof(h5fnal_vect_hitcoll_data_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for test data container");

    /* Get memory for the hit collections */
    if (NULL == (data->hit_collections = (h5fnal_hitcoll_t *)calloc((size_t)n_hit_collections, sizeof(h5fnal_hitcoll_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for hit collection data");

    data->n_hits = 0;
    data->n_hit_collections = n_hit_collections;

    /* Loop over the hit collections, adding data */
    first_hit = 0;
    for (hc = 0; hc < n_hit_collections; hc++) {

        hsize_t u;
        hsize_t this_hits;
        hsize_t end;

        /* Get the number of hits 'stored' in this hit collection.
         * 50% of hit collections will be of size zero.
         * The others will be of size 0 to 1024.
         */
        if (((double)rand() / RAND_MAX) < 0.5)
            this_hits = 0;
        else
            this_hits = (int)(rand() % 1024);

        /* Write the hit collection data to the buffer */
        data->hit_collections[hc].channel = start_channel + hc;
        data->hit_collections[hc].start = (this_hits > 0) ? first_hit : 0;
        data->hit_collections[hc].count = this_hits;

        if (this_hits > 0) {
            /* resize the hits buffer */
            data->n_hits += this_hits;
            if (NULL == (data->hits = (h5fnal_hit_t *)realloc((void *)data->hits, data->n_hits * sizeof(h5fnal_hit_t))))
                H5FNAL_PROGRAM_ERROR("could not (re)allocate memory for hit data");

            /* Write random hit data to the buffer */
            end = first_hit + this_hits;
            for (u = first_hit; u < end; u++) {
                data->hits[u].signal_time   = (float)rand();
                data->hits[u].signal_width  = (float)rand();
                data->hits[u].peak_amp      = (float)rand();
                data->hits[u].charge        = (float)rand();
                data->hits[u].part_vertex_x = (float)rand();
                data->hits[u].part_vertex_y = (float)rand();
                data->hits[u].part_vertex_z = (float)rand();
                data->hits[u].part_energy   = (float)rand();
                data->hits[u].part_track_id = (int)rand();
            } 

            /* Update the first hit index for the next hit collection */
            first_hit += this_hits;
        }
    } /* end loop over hit collections */

    return data;

error:
    if (data) {
        free(data->hits);
        free(data->hit_collections);
    }
    free(data);

    return NULL;

} /* end generate_test_hit_collectionss() */

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_vect_hitcoll_t *vector = NULL;
    hsize_t n_hit_collections;
    h5fnal_vect_hitcoll_data_t *data = NULL;
    h5fnal_vect_hitcoll_data_t *data_out = NULL;

    printf("Testing Vector of MC Hit Collection operations... ");

    /* Create the file */
    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR;
    if ((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create the run, sub-run, and event */
    if ((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run");
    if ((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");
    if ((event_id = h5fnal_create_event(subrun_id, EVENT_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event");

    /* Create the vector of MC hit collection data product */
    if (NULL == (vector = (h5fnal_vect_hitcoll_t *)calloc(1, sizeof(h5fnal_vect_hitcoll_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector");
    if (h5fnal_create_v_mc_hit_collection(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not create vector of mc hit collection");

    /* Generate some test data */
    n_hit_collections = 16384;
    if (NULL == (data = generate_test_hit_collections(n_hit_collections)))
        H5FNAL_PROGRAM_ERROR("unable to create test hit collection data");

    /* Append hits */
    if (h5fnal_append_hits(vector, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write hit collections to the file");

    /* Read the hits */
    if (NULL == (data_out = (h5fnal_vect_hitcoll_data_t *)calloc((size_t)1, sizeof(h5fnal_vect_hitcoll_data_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for read data container");
    if (h5fnal_read_all_hits(vector, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read hit collections from the file");

    /* Compare the written and read data */
    if (memcmp(data->hits, data_out->hits, data->n_hits * sizeof(h5fnal_hit_t)) != 0)
        H5FNAL_PROGRAM_ERROR("bad read data (hits)");
    if (memcmp(data->hit_collections, data_out->hit_collections, data->n_hit_collections * sizeof(h5fnal_hitcoll_t)) != 0)
        H5FNAL_PROGRAM_ERROR("bad read data (hit collections)");

    /* Close the vector */
    if (h5fnal_close_v_mc_hit_collection(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector");

    /* Re-open the vector */
    if (h5fnal_open_v_mc_hit_collection(event_id, VECTOR_NAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open vector of mc hit collection");

    /* Re-read the hits */
    if (h5fnal_free_hitcoll_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free in-memory hit collection data");
    if (h5fnal_read_all_hits(vector, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read hit collections from the file");

    /* Compare the written and read data */
    if (memcmp(data->hits, data_out->hits, data->n_hits * sizeof(h5fnal_hit_t)) != 0)
        H5FNAL_PROGRAM_ERROR("bad read data (hits)");
    if (memcmp(data->hit_collections, data_out->hit_collections, data->n_hit_collections * sizeof(h5fnal_hitcoll_t)) != 0)
        H5FNAL_PROGRAM_ERROR("bad read data (hit collections)");

    /* Close the vector */
    if(h5fnal_close_v_mc_hit_collection(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector");

    /* Close everything */
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run");
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run");
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event");
    if (H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR;
    if (h5fnal_free_hitcoll_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not free in-memory hit collection data");
    if (h5fnal_free_hitcoll_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free in-memory hit collection data");
    free(vector);
    free(data);
    free(data_out);

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;
    if(vector)
        h5fnal_close_v_mc_hit_collection(vector);
    if (data)
        h5fnal_free_hitcoll_mem_data(data);
    if (data_out)
        h5fnal_free_hitcoll_mem_data(data_out);
    h5fnal_close_run(run_id);
    h5fnal_close_run(subrun_id);
    h5fnal_close_event(event_id);
    free(vector);
    free(data);
    free(data_out);

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


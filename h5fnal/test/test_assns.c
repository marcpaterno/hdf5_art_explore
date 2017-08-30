/* Test the Assns API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"

#define FILE_NAME           "assns.h5"
#define RUN_NAME            "testrun"
#define SUBRUN_NAME         "testsubrun"
#define EVENT_NAME          "testevent"
#define ASSNS_NAME          "assns"
#define ASSNS_DATA_NAME     "assns_data"
#define LEFT_NAME           "left_data_product"
#define RIGHT_NAME          "right_data_product"

h5fnal_assns_data_t *
generate_test_assns(size_t n)
{
    size_t u;
    int64_t *int_data = NULL;
    h5fnal_assns_data_t *assns_data = NULL;

    /* If you want to get deterministic behavior, change NULL to an integer */
    srand((unsigned int)time(NULL));

    /* Get memory for the top-level data container */
    if (NULL == (assns_data = (h5fnal_assns_data_t *)calloc((size_t)1, sizeof(h5fnal_assns_data_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for test data container");

    /* Allocate memory for pair and data arrays */
    assns_data->n = n;
    if (NULL == (assns_data->pairs = (h5fnal_pair_t *)calloc(n, sizeof(h5fnal_pair_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for test pairs");
    if (NULL == (int_data = (int64_t *)calloc(n, sizeof(int64_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for test data");

    /* Fill in random data */
    for (u = 0; u < n; u++) {
        assns_data->pairs[u].left_process_index = (uint16_t)rand();
        assns_data->pairs[u].left_product_index = (uint16_t)rand();
        assns_data->pairs[u].left_key = (uint64_t)rand();

        assns_data->pairs[u].right_process_index = (uint16_t)rand();
        assns_data->pairs[u].right_product_index = (uint16_t)rand();
        assns_data->pairs[u].right_key = (uint64_t)rand();

        int_data[u] = (uint64_t)rand();
    }

    assns_data->data = (void *)int_data;

    return assns_data;

error:

    if (assns_data)
        h5fnal_free_assns_mem_data(assns_data);

    free(assns_data);
    return NULL;

} /* end generate_test_assns() */


/************************************************************************
 * Function:    main()
 *
 * Purpose:     Tests the Assns data product API calls.
 *
 * Returns:     EXIT_SUCCESS / EXIT_FAILURE
 *
 ************************************************************************/
int
main(void)
{
    /* HDF5 IDs */
    hid_t                   fid = -1;
    hid_t                   fapl_id = -1;
    hid_t                   run_id = -1;
    hid_t                   subrun_id = -1;
    hid_t                   event_id = -1;

    /* Assns */
    h5fnal_assns_t         *assns = NULL;
    h5fnal_assns_t         *assns_data = NULL;
    hsize_t                 n;

    /* Data */
    h5fnal_assns_data_t    *data = NULL;
    h5fnal_assns_data_t    *data_out = NULL;
    size_t                  size;

    printf("Testing Assns operations... ");

    /********************/
    /* FILE BOILERPLATE */
    /********************/

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

    /***********************/
    /* CREATE DATA PRODUCT */
    /***********************/

    /* Create the assns data product */
    if (NULL == (assns = calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns");
    if (h5fnal_create_assns(event_id, ASSNS_NAME, LEFT_NAME, RIGHT_NAME, H5FNAL_BAD_HID_T, assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not create assns data product");

    /* Create the assns data product that uses 'extra' data */
    if (NULL == (assns_data = calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns_data");
    if (h5fnal_create_assns(event_id, ASSNS_DATA_NAME, LEFT_NAME, RIGHT_NAME, H5T_STD_I64LE, assns_data) < 0)
        H5FNAL_PROGRAM_ERROR("could not create assns_data data product");

    /* Make sure we are getting the names of the left and right data products out */
    if (!assns->right)
        H5FNAL_PROGRAM_ERROR("right data product name in struct is NULL")
    if (!assns->left)
        H5FNAL_PROGRAM_ERROR("left data product name in struct is NULL")
    if (strcmp(RIGHT_NAME, assns->right) != 0)
        H5FNAL_PROGRAM_ERROR("incorrect name stored for right data product")
    if (strcmp(LEFT_NAME, assns->left) != 0)
        H5FNAL_PROGRAM_ERROR("incorrect name stored for left data product")

    /**********************/
    /* GENERATE TEST DATA */
    /**********************/

    /* The test data contains 'extra' data, which will be ignored when
     * writing to the non-extra-data Assns data product.
     */
    n = 16384;
    if (NULL == (data = generate_test_assns(n)))
        H5FNAL_PROGRAM_ERROR("unable to create test assns");

    /*******************************/
    /* WRITE ASSOCIATIONS AND DATA */
    /*******************************/

    /* Write some assns */
    if (h5fnal_append_assns(assns, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");
    if (h5fnal_append_assns(assns, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");

    /* Write some assns with 'extra' data */
    if (h5fnal_append_assns(assns_data, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns (w/ data) to the file");
    if (h5fnal_append_assns(assns_data, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns (w/ data) to the file");

    /******************************************/
    /* READ AND COMPARE ASSOCIATIONS AND DATA */
    /******************************************/

    /* Get memory for the top-level data container */
    if (NULL == (data_out = (h5fnal_assns_data_t *)calloc((size_t)1, sizeof(h5fnal_assns_data_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for assns data container");

    /* Read the Assns */
    if (h5fnal_read_all_assns(assns, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare sizes and check for NULL data array */
    if (data_out->n != 2 * data->n)
        H5FNAL_PROGRAM_ERROR("got wrong number of Assns from data product");
    if (data_out->data != NULL)
        H5FNAL_PROGRAM_ERROR("data array was not NULL");

    /* Compare output buffers */
    size = data->n * sizeof(h5fnal_pair_t);
    if (0 != memcmp(data->pairs, data_out->pairs, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (1)");
    if (0 != memcmp(data->pairs, data_out->pairs + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (2)");

    /* Clean up */
    if (h5fnal_free_assns_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");
    

    /* Read the Assns w/ extra data*/
    if (h5fnal_read_all_assns(assns_data, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare sizes */
    if (data_out->n != 2 * data->n)
        H5FNAL_PROGRAM_ERROR("got wrong number of Assns from data product");

    /* Compare output buffers */
    size = data->n * sizeof(h5fnal_pair_t);
    if (0 != memcmp(data->pairs, data_out->pairs, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (1)");
    if (0 != memcmp(data->pairs, data_out->pairs + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (2)");
    size = data->n * sizeof(int64_t);
    if (0 != memcmp(data->data, data_out->data, size))
        H5FNAL_PROGRAM_ERROR("Assns data buffer incorrect (1)");
    if (0 != memcmp(data->data, (int64_t *)data_out->data + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns data buffer incorrect (2)");

    /* Clean up */
    if (h5fnal_free_assns_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");

    /**********************************/
    /* CLOSE AND RE-OPEN DATA PRODUCT */
    /**********************************/

    /* Close the assns data products */
    if (h5fnal_close_assns(assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns");
    if (h5fnal_close_assns(assns_data) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns_data");

    /* Re-open the assns data products */
    if (h5fnal_open_assns(event_id, ASSNS_NAME, assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not open assns data product");
    if (h5fnal_open_assns(event_id, ASSNS_DATA_NAME, assns_data) < 0)
        H5FNAL_PROGRAM_ERROR("could not open assns_data data product");
    /* Make sure we are getting the names of the left and right data products out */
    if (!assns->right)
        H5FNAL_PROGRAM_ERROR("right data product name in struct is NULL")
    if (!assns->left)
        H5FNAL_PROGRAM_ERROR("left data product name in struct is NULL")
    if (0 != strcmp(RIGHT_NAME, assns->right))
        H5FNAL_PROGRAM_ERROR("incorrect name stored for right data product")
    if (0 != strcmp(LEFT_NAME, assns->left))
        H5FNAL_PROGRAM_ERROR("incorrect name stored for left data product")

    /*******************************/
    /* RE-READ DATA AND RE-COMPARE */
    /*******************************/

    /* Read the Assns */
    if (h5fnal_read_all_assns(assns, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare sizes and check for NULL data array */
    if (data_out->n != 2 * data->n)
        H5FNAL_PROGRAM_ERROR("got wrong number of Assns from data product");
    if (data_out->data != NULL)
        H5FNAL_PROGRAM_ERROR("data array was not NULL");

    /* Compare output buffers */
    size = data->n * sizeof(h5fnal_pair_t);
    if (0 != memcmp(data->pairs, data_out->pairs, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (1)");
    if (0 != memcmp(data->pairs, data_out->pairs + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (2)");

    /* Clean up */
    if (h5fnal_free_assns_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");
    

    /* Read the Assns w/ extra data*/
    if (h5fnal_read_all_assns(assns_data, data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare sizes */
    if (data_out->n != 2 * data->n)
        H5FNAL_PROGRAM_ERROR("got wrong number of Assns from data product");

    /* Compare output buffers */
    size = data->n * sizeof(h5fnal_pair_t);
    if (0 != memcmp(data->pairs, data_out->pairs, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (1)");
    if (0 != memcmp(data->pairs, data_out->pairs + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns pair buffer incorrect (2)");
    size = data->n * sizeof(int64_t);
    if (0 != memcmp(data->data, data_out->data, size))
        H5FNAL_PROGRAM_ERROR("Assns data buffer incorrect (1)");
    if (0 != memcmp(data->data, (int64_t *)data_out->data + data->n, size))
        H5FNAL_PROGRAM_ERROR("Assns data buffer incorrect (2)");

    /* Clean up */
    if (h5fnal_free_assns_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");

    /********************/
    /* CLOSE EVERYTHING */
    /********************/

    /* Close the assns data products */
    if (h5fnal_close_assns(assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns");
    if (h5fnal_close_assns(assns_data) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns_data");

    /* Close boilerplate */
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

    free(assns);
    free(assns_data);

    if (h5fnal_free_assns_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");
    free(data);
    if (h5fnal_free_assns_mem_data(data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns memory");
    free(data_out);

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        if(assns)
            h5fnal_close_assns(assns);
        if(assns_data)
            h5fnal_close_assns(assns_data);
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    free(assns);
    free(assns_data);

    if (data) {
        h5fnal_free_assns_mem_data(data);
        free(data);
    }
    if (data_out) {
        h5fnal_free_assns_mem_data(data_out);
        free(data_out);
    }

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
} /* end main() */


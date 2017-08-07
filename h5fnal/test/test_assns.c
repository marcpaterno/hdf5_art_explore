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

/************************************************************************
 * Function:    generate_fake_associations()
 *
 * Purpose:     Generate associations to test Assns data product I/O.
 *
 *              The fields of the generated associations are just random data
 *              and do not correspond to any real data product.
 *
 * Parameters:
 *  n_associationns:    The number of associations to create.
 *
 * Returns:     The buffer of associations. The caller is responsible for
 *              freeing this data using free().
 *
 *              NULL on failure.
 ************************************************************************/
h5fnal_association_t *
generate_fake_associations(size_t n_associations)
{
    size_t u;
    h5fnal_association_t *associations = NULL;

    srand((unsigned int)time(NULL));

    if (NULL == (associations = (h5fnal_association_t *)calloc(n_associations, sizeof(h5fnal_association_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for fake associations");

    for (u = 0; u < n_associations; u++) {
        associations[u].left_process_index = (uint16_t)rand();
        associations[u].left_product_index = (uint16_t)rand();
        associations[u].left_key = (uint64_t)rand();

        associations[u].right_process_index = (uint16_t)rand();
        associations[u].right_product_index = (uint16_t)rand();
        associations[u].right_key = (uint64_t)rand();
    }

    return associations;

error:

    free(associations);
    return NULL;

} /* end generate_fake_associations() */


/************************************************************************
 * Function:    generate_fake_data()
 *
 * Purpose:     Generate random data to test Assns data product I/O.
 *
 * Parameters:
 *  n_data:     The number of data elements to create.
 *
 * Returns:     A buffer of data. The caller is responsible for freeing this
 *              data with free().
 *
 *              NULL on failure.
 ************************************************************************/
int64_t *
generate_fake_data(size_t n_data)
{
    size_t u;
    int64_t *data = NULL;

    srand((unsigned int)time(NULL));

    if (NULL == (data = (int64_t *)calloc(n_data, sizeof(int64_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for fake data");

    for (u = 0; u < n_data; u++)
        data[u] = (uint64_t)rand();

    return data;

error:

    free(data);
    return NULL;

} /* end generate_fake_data() */


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
    size_t                  n_assns;
    hssize_t                n_assns_out = 0;

    /* associations */
    size_t                  associations_size;
    h5fnal_association_t   *associations = NULL;
    h5fnal_association_t   *associations_out = NULL;

    /* 'extra' data */
    size_t                  data_size;
    int64_t                *data = NULL;
    int64_t                *data_out = NULL;


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
    if ((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run");
    if ((subrun_id = h5fnal_create_run(run_id, SUBRUN_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");
    if ((event_id = h5fnal_create_event(subrun_id, EVENT_NAME)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event");

    /***********************/
    /* CREATE DATA PRODUCT */
    /***********************/

    /* Create the assns data product */
    if (NULL == (assns = calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns");
    if (h5fnal_create_assns(event_id, ASSNS_NAME, LEFT_NAME, RIGHT_NAME, assns, H5FNAL_BAD_HID_T) < 0)
        H5FNAL_PROGRAM_ERROR("could not create assns data product");

    /* Create the assns data product that uses 'extra' data */
    if (NULL == (assns_data = calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns_data");
    if (h5fnal_create_assns(event_id, ASSNS_DATA_NAME, LEFT_NAME, RIGHT_NAME, assns_data, H5T_STD_I64LE) < 0)
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

    /* The number of Assns to create in each product */
    n_assns = 16384;

    /************************/
    /* GENERATE (FAKE) DATA */
    /************************/

    /* Generate some fake associations */
    if (NULL == (associations = generate_fake_associations(n_assns)))
        H5FNAL_PROGRAM_ERROR("unable to create fake associations");

    /* Generate some fake 'extra' data */
    if (NULL == (data = generate_fake_data(n_assns)))
        H5FNAL_PROGRAM_ERROR("unable to create fake associations");

    /*******************************/
    /* WRITE ASSOCIATIONS AND DATA */
    /*******************************/

    /* Write some assns */
    if (h5fnal_append_assns(assns, n_assns, associations, NULL) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");
    if (h5fnal_append_assns(assns, n_assns, associations, NULL) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");

    /* Write some assns with 'extra' data */
    if (h5fnal_append_assns(assns_data, n_assns, associations, (void *)data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");
    if (h5fnal_append_assns(assns_data, n_assns, associations, (void *)data) < 0)
        H5FNAL_PROGRAM_ERROR("could not write assns to the file");

    /******************************************/
    /* READ AND COMPARE ASSOCIATIONS AND DATA */
    /******************************************/

    /* Get the number of assns */
    if ((n_assns_out = h5fnal_get_assns_count(assns)) < 0)
        H5FNAL_PROGRAM_ERROR("could not get number of assns from data product");
    if (n_assns_out != 2 * n_assns)
        H5FNAL_PROGRAM_ERROR("got wrong number of assns from data product");

    /* Generate buffers for reading the associations and data */
    if (NULL == (associations_out = (h5fnal_association_t *)calloc(n_assns_out, sizeof(h5fnal_association_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for associations_out");
    if (NULL == (data_out = (int64_t *)calloc(n_assns_out, sizeof(int64_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for data_out");

    /* Calculate sizes for memory compare */
    associations_size = n_assns * sizeof(h5fnal_association_t);
    data_size = n_assns * sizeof(int64_t);

    /* Read the assns */
    if (h5fnal_read_all_assns(assns, associations_out, NULL) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare the associations */
    if (0 != memcmp(associations, associations_out, associations_size))
        H5FNAL_PROGRAM_ERROR("association read buffer incorrect (1)");
    if (0 != memcmp(associations, associations_out + n_assns, associations_size))
        H5FNAL_PROGRAM_ERROR("association read buffer incorrect (2)");

    memset(associations_out, 0, n_assns_out * sizeof(h5fnal_association_t));
    memset(data_out, 0, n_assns_out * sizeof(int64_t));

    /* Read the assns that use 'extra' data */
    if (h5fnal_read_all_assns(assns_data, associations_out, (void *)data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare the associations */
    if (0 != memcmp(associations, associations_out, associations_size))
        H5FNAL_PROGRAM_ERROR("association read buffer incorrect (1)");
    if (0 != memcmp(associations, associations_out + n_assns, associations_size))
        H5FNAL_PROGRAM_ERROR("association read buffer incorrect (2)");

    /* Compare the 'extra' data */
    if (0 != memcmp(data, data_out, data_size))
        H5FNAL_PROGRAM_ERROR("data read buffer incorrect (1)");
    if (0 != memcmp(data, data_out + n_assns, data_size))
        H5FNAL_PROGRAM_ERROR("data read buffer incorrect (2)");

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

    memset(associations_out, 0, n_assns_out * sizeof(h5fnal_association_t));

    /* Re-read the assns */
    if (h5fnal_read_all_assns(assns, associations_out, NULL) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare the associations */
    if (0 != memcmp(associations, associations_out, associations_size))
        H5FNAL_PROGRAM_ERROR("association (re-)read buffer incorrect (1)");
    if (0 != memcmp(associations, associations_out + n_assns, associations_size))
        H5FNAL_PROGRAM_ERROR("association (re-)read buffer incorrect (2)");

    memset(associations_out, 0, n_assns_out * sizeof(h5fnal_association_t));
    memset(data_out, 0, n_assns_out * sizeof(int64_t));

    /* Re-read the assns with 'extra' data */
    if (h5fnal_read_all_assns(assns_data, associations_out, (void *)data_out) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns from the file");

    /* Compare the associations */
    if (0 != memcmp(associations, associations_out, associations_size))
        H5FNAL_PROGRAM_ERROR("association (re-)read buffer incorrect (1)");
    if (0 != memcmp(associations, associations_out + n_assns, associations_size))
        H5FNAL_PROGRAM_ERROR("association (re-)read buffer incorrect (2)");

    /* Compare the 'extra' data */
    if (0 != memcmp(data, data_out, data_size))
        H5FNAL_PROGRAM_ERROR("data (re-)read buffer incorrect (1)");
    if (0 != memcmp(data, data_out + n_assns, data_size))
        H5FNAL_PROGRAM_ERROR("data (re-)read buffer incorrect (2)");

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

    free(data);
    free(data_out);
    free(associations);
    free(associations_out);
    free(assns);

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

    free(data);
    free(data_out);
    free(associations);
    free(associations_out);
    free(assns);

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
} /* end main() */


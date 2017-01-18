/* ref_writer.c
 *
 * Creates a large 1-D dataset of all values and a second dataset
 * of region references into it.
 *
 * Usage: ref_writer <# dsets> <# elements per dset>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hdf5.h"

#define USAGE       "ref_writer <# datasets> <# elements per dataset>"

#define FILENAME	"ref.h5"

/* All datasets are 1-D */
#define NDIMS       1

#define DATA_DSET_NAME  "data"
#define REF_DSET_NAME   "refs"
#define MAX_REGION_NAME_LEN   64

/* ERROR MACROS
 *
 * HDF5 errors will dump the HDF5 function stack (in debug mode at least)
 * so we can guess what went wrong from that.
 *
 * Other errors should emit a helpful error string.
 */
#define SUCCESS             0
#define FAILURE             (-1)
#define ERROR_MSG           fprintf(stderr, "***ERROR*** at line %d in function %s()...\n", __LINE__, __FUNCTION__);
#define HDF5_ERROR          {ERROR_MSG goto error;}
#define PROGRAM_ERROR(s)    {ERROR_MSG fprintf(stderr, "%s\n", (s)); goto error;}

herr_t
create_data_dataset(hid_t fid, hid_t *data_did)
{
    hid_t did = -1;             /* dataset ID                               */
    hid_t dcpl_id = -1;         /* dataset creation plist ID                */
    hid_t sid = -1;             /* dataspace ID                             */
    hsize_t init_dims[NDIMS];   /* initial size of dataset                  */
    hsize_t max_dims[NDIMS];    /* maximum size to which dataset can grow   */
    hsize_t chunk_dims[NDIMS];  /* size of each chunk                       */

    assert(data_did);

    /* Set up chunking and compression.
     * The chunk size is set to an arbitrary value for now.
     */
    chunk_dims[0] = 128;
    if((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        HDF5_ERROR
    if(H5Pset_chunk(dcpl_id, NDIMS, chunk_dims) < 0)
        HDF5_ERROR
    if(H5Pset_deflate(dcpl_id, 6) < 0)
        HDF5_ERROR

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if((sid = H5Screate_simple(NDIMS, init_dims, max_dims)) < 0)
        HDF5_ERROR

    /* Create the dataset
     * H5Dcreate2() is the newer function that takes three property lists.
     */
    if((did = H5Dcreate2(fid, DATA_DSET_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Close everything */
    if(H5Pclose(dcpl_id) < 0)
        HDF5_ERROR
    if(H5Sclose(sid) < 0)
        HDF5_ERROR

    *data_did = did;

    return SUCCESS;

error:
    *data_did = -1;
    H5E_BEGIN_TRY {
        H5Pclose(dcpl_id);
        H5Sclose(sid);
    } H5E_END_TRY;

    return FAILURE;
} /* end create_data_dataset */

herr_t
create_ref_dataset(hid_t fid, hid_t *ref_did)
{
    hid_t did = -1;             /* dataset ID                               */
    hid_t dcpl_id = -1;         /* dataset creation plist ID                */
    hid_t sid = -1;             /* dataspace ID                             */
    hsize_t init_dims[NDIMS];   /* initial size of dataset                  */
    hsize_t max_dims[NDIMS];    /* maximum size to which dataset can grow   */
    hsize_t chunk_dims[NDIMS];  /* size of each chunk                       */

    assert(ref_did);

    /* Set up chunking and compression.
     * The chunk size is set to an arbitrary value for now.
     */
    chunk_dims[0] = 128;
    if((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        HDF5_ERROR
    if(H5Pset_chunk(dcpl_id, NDIMS, chunk_dims) < 0)
        HDF5_ERROR
    if(H5Pset_deflate(dcpl_id, 6) < 0)
        HDF5_ERROR

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = 0;
    max_dims[0] = H5S_UNLIMITED;
    if((sid = H5Screate_simple(NDIMS, init_dims, max_dims)) < 0)
        HDF5_ERROR

    /* Create the dataset
     * H5Dcreate2() is the newer function that takes three property lists.
     */
    if((did = H5Dcreate2(fid, REF_DSET_NAME, H5T_STD_REF_DSETREG, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Close everything */
    if(H5Pclose(dcpl_id) < 0)
        HDF5_ERROR
    if(H5Sclose(sid) < 0)
        HDF5_ERROR

    *ref_did = did;

    return SUCCESS;

error:
    *ref_did = -1;
    H5E_BEGIN_TRY {
        H5Pclose(dcpl_id);
        H5Sclose(sid);
    } H5E_END_TRY;

    return FAILURE;
} /* end create_ref_dataset */

herr_t
add_data(hid_t fid, hid_t data_did, hid_t ref_did, hsize_t n_elements)
{
    hid_t data_sid = -1;        /* dataspace ID for data dataset            */
    hid_t ref_sid = -1;         /* dataspace ID for reference dataset       */
    hid_t mem_sid = -1;         /* dataspace ID for memory                  */
    hsize_t data_dims[NDIMS];   /* size of data dataset                     */
    hsize_t ref_dims[NDIMS];    /* size of reference dataset                */
    hsize_t mem_dims[NDIMS];    /* size of memory buffer                    */
    hsize_t new_data_dims[NDIMS];   /* new size of data dataset             */
    hsize_t new_ref_dims[NDIMS];    /* new size of reference dataset        */
    hsize_t start[NDIMS];
    hsize_t stride[NDIMS];
    hsize_t count[NDIMS];
    hsize_t block[NDIMS];
    int *data = NULL;           /* data buffer                              */
    hsize_t u;                  /* iterator                                 */
    hdset_reg_ref_t region;     /* dataset region reference                 */

    /* DATA DATASET */

    /* Get a dataspace that represent the current size of the dataset */
    if((data_sid = H5Dget_space(data_did)) < 0)
        HDF5_ERROR

    /* Get the size of the dataset */
    if(H5Sget_simple_extent_dims(data_sid, data_dims, NULL) < 0)
        HDF5_ERROR

    /* extend the dataset */
    new_data_dims[0] = data_dims[0] + n_elements;
    if(H5Dset_extent(data_did, new_data_dims) < 0)
        HDF5_ERROR

    /* Update the file space. As per the API docs, you *must* do this
     * when the dataset is already open.
     */
    if(H5Sclose(data_sid) < 0)
        HDF5_ERROR
    if((data_sid = H5Dget_space(data_did)) < 0)
        HDF5_ERROR

    /* Set file dataspace hyperslab for the dataset write */
    start[0] = data_dims[0];
    stride[0] = 1;
    count[0] = n_elements;
    block[0] = 1;
    if(H5Sselect_hyperslab(data_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        HDF5_ERROR

    /* create a new simple dataspace to describe the data buffer. */
    mem_dims[0] = n_elements;
    if((mem_sid = H5Screate_simple(NDIMS, mem_dims, mem_dims)) < 0)
        HDF5_ERROR

    /* Fill data dataset with artificial data */
    srand(time(NULL));
    if(NULL == (data = (int *)calloc((size_t)n_elements, sizeof(int))))
        PROGRAM_ERROR("Could not allocate memory for data buffer")
    for(u = 0; u < n_elements; u++)
        data[u] = rand();
    if(H5Dwrite(data_did, H5T_NATIVE_INT, mem_sid, data_sid, H5P_DEFAULT, data) < 0)
        HDF5_ERROR

    /* REGION REFERENCE DATASET */

    /* Get a dataspace that represent the current size of the dataset */
    if((ref_sid = H5Dget_space(ref_did)) < 0)
        HDF5_ERROR

    /* Get the size of the dataset */
    if(H5Sget_simple_extent_dims(ref_sid, ref_dims, NULL) < 0)
        HDF5_ERROR

    /* extend the dataset */
    new_ref_dims[0] = ref_dims[0] + 1;
    if(H5Dset_extent(ref_did, new_ref_dims) < 0)
        HDF5_ERROR

    /* Update the file space. As per the API docs, you *must* do this
     * when the dataset is already open.
     */
    if(H5Sclose(ref_sid) < 0)
        HDF5_ERROR
    if((ref_sid = H5Dget_space(ref_did)) < 0)
        HDF5_ERROR

    /* Set file dataspace hyperslab for the dataset write */
    start[0] = ref_dims[0];
    stride[0] = 1;
    count[0] = 1;
    block[0] = 1;
    if(H5Sselect_hyperslab(ref_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
        HDF5_ERROR

    /* create a new simple dataspace to describe the data buffer. */
    if(H5Sclose(mem_sid) < 0)
        HDF5_ERROR
    mem_dims[0] = 1;
    if((mem_sid = H5Screate_simple(NDIMS, mem_dims, mem_dims)) < 0)
        HDF5_ERROR

    /* Create the region reference */
    if(H5Rcreate(&region, fid, DATA_DSET_NAME, H5R_DATASET_REGION, data_sid) < 0)
        HDF5_ERROR

    /* Add a dataset region reference to the reference dataset */
    if(H5Dwrite(ref_did, H5T_STD_REF_DSETREG, mem_sid, ref_sid, H5P_DEFAULT, &region) < 0)
        HDF5_ERROR

    /* Close everything */
    free(data);
    if(H5Sclose(mem_sid) < 0)
        HDF5_ERROR
    if(H5Sclose(data_sid) < 0)
        HDF5_ERROR
    if(H5Sclose(ref_sid) < 0)
        HDF5_ERROR

    return SUCCESS;

error:
    free(data);
    H5E_BEGIN_TRY {
        H5Sclose(mem_sid);
        H5Sclose(data_sid);
        H5Sclose(ref_sid);
    } H5E_END_TRY;

    return FAILURE;
} /* end add_data */

int
main(int argc, char *argv[])
{
    /* The library returns IDs which are then used to refer to HDF5 objects.
     * It is good practice to set them to -1 in case they get used before
     * they are set.
     */
    hid_t fid = -1;         /* HDF5 file ID                                 */
    hid_t fapl_id = -1;     /* file access property list ID                 */
    hid_t data_did = -1;    /* data dataset ID                              */
    hid_t ref_did = -1;     /* reference dataset ID                         */

    unsigned long   n_datasets = 0; /* The number of datasets to create in the file */
    hsize_t         n_elements = 0; /* The number of data elements per dataset      */

    unsigned long   u;      /* iterator                                     */

    /* Get command-line parameters */
    if(argc != 3) {
        fprintf(stderr, "***ERROR***    Wrong number of command-line arguments\n");
        fprintf(stderr, "USAGE: %s\n", USAGE);
        exit(EXIT_FAILURE);
    }
    n_datasets = strtoul(argv[1], NULL, 10);
    n_elements = (hsize_t)strtoull(argv[2], NULL, 10);

    /* Create a file access property list and tell it to use the latest
     * HDF5 file format, which uses more efficient data structures by
     * default than the most backward-compatible one.
     *
     * NB: A property list is just a collection of what would normally
     *     be function call parameters. They let us keep the API
     *     consistent as we add features.
     */
    if((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        HDF5_ERROR
    if(H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        HDF5_ERROR

    /* Create the HDF5 file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        HDF5_ERROR

    /* Create the datasets */
    if(create_data_dataset(fid, &data_did) != SUCCESS)
        PROGRAM_ERROR("Unable to create data dataset")
    if(create_ref_dataset(fid, &ref_did) != SUCCESS)
        PROGRAM_ERROR("Unable to create reference dataset")

    /* Write data to the datasets */
    for(u = 0; u < n_datasets; u++)
        if(add_data(fid, data_did, ref_did, n_elements) != SUCCESS)
            PROGRAM_ERROR("Unable to add data to the datasets")

    /* Close all the open IDs.
     * Close order does not matter unless you tell the library to
     * care about it.
     */
    if(H5Dclose(data_did) < 0)
        HDF5_ERROR
    if(H5Dclose(ref_did) < 0)
        HDF5_ERROR
    if(H5Pclose(fapl_id) < 0)
        HDF5_ERROR
    if(H5Fclose(fid) < 0)
        HDF5_ERROR

    exit(EXIT_SUCCESS);

error:
    /* It's best practice to try to close everything at the end, even
     * if there were errors.
     */
    H5E_BEGIN_TRY {
        H5Dclose(data_did);
        H5Dclose(ref_did);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    exit(EXIT_FAILURE);
} /* end main */


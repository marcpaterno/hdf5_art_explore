/* dset_writer.c
 *
 * Usage: dset_writer <# dsets> <# elements per dset>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hdf5.h"

#define USAGE       "dset_writer <# datasets> <# elements per dataset>"

#define FILENAME	"dset.h5"

#define GROUP_NAME  "indexed_group"

/* All datasets are 1-D */
#define NDIMS       1

/* The ith dset has the name "i" so this just needs to hold a ulong as string */
#define MAX_DSET_NAME_LEN   32

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
create_dataset(hid_t loc_id, unsigned long n, hsize_t n_elements)
{
    hid_t did = -1;             /* dataset ID                               */
    hid_t dcpl_id = -1;         /* dataset creation plist ID                */
    hid_t sid = -1;             /* dataspace ID                             */
    hsize_t init_dims[NDIMS];   /* initial size of dataset                  */
    hsize_t max_dims[NDIMS];    /* maximum size to which dataset can grow   */
    hsize_t chunk_dims[NDIMS];  /* size of each chunk                       */
    char dsetname[MAX_DSET_NAME_LEN];   /* dataset name                     */
    int *data = NULL;           /* data buffer                              */
    hsize_t u;                  /* iterator                                 */

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

    /* Create the dataset name */
    memset(dsetname, 0, MAX_DSET_NAME_LEN);
    if(snprintf(dsetname, MAX_DSET_NAME_LEN, "%lu", n) < 0)
        PROGRAM_ERROR("Could not create dataset name string")

    /* Create the dataspace (set of points describing the data size, etc.) */
    init_dims[0] = n_elements;
    max_dims[0] = H5S_UNLIMITED;
    if((sid = H5Screate_simple(NDIMS, init_dims, max_dims)) < 0)
        HDF5_ERROR

    /* Create the dataset
     * H5Dcreate2() is the newer function that takes three property lists.
     */
    if((did = H5Dcreate2(loc_id, dsetname, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Fill dataset with artificial data */
    srand(time(NULL));
    if(NULL == (data = (int *)calloc((size_t)n_elements, sizeof(int))))
        PROGRAM_ERROR("Could not allocate memory for data buffer")
    for(u = 0; u < n_elements; u++)
        data[u] = rand();
    if(H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
        HDF5_ERROR

    /* Close everything */
    free(data);
    if(H5Dclose(did) < 0)
        HDF5_ERROR
    if(H5Pclose(dcpl_id) < 0)
        HDF5_ERROR
    if(H5Sclose(sid) < 0)
        HDF5_ERROR

    return SUCCESS;

error:
    free(data);
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Pclose(dcpl_id);
        H5Sclose(sid);
    } H5E_END_TRY;

    return FAILURE;
} /* end create_dataset */

int
main(int argc, char *argv[])
{
    /* The library returns IDs which are then used to refer to HDF5 objects.
     * It is good practice to set them to -1 in case they get used before
     * they are set.
     */
    hid_t fid = -1;         /* HDF5 file ID                                 */
    hid_t fapl_id = -1;     /* file access property list ID                 */
    hid_t gcpl_id = -1;     /* group creation property list ID              */
    hid_t gid = -1;         /* group ID                                     */

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

    /* Create a group to contain the datasets.
     * We want to index the group by creation order since that will
     * allow more efficient and straightforward iteration with H5Literat()
     * later and we can't set this up in the root group.
     */
    if((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) , 0)
        HDF5_ERROR
    if(H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0)
        HDF5_ERROR
    if((gid = H5Gcreate2(fid, GROUP_NAME, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Create the datasets */
    for(u = 0; u < n_datasets; u++)
        if(create_dataset(gid, u, n_elements) != SUCCESS)
            PROGRAM_ERROR("Unable to create dataset")

    /* Close all the open IDs.
     * Close order does not matter unless you tell the library to care
     * about it but it's still good practice to close the file last.
     */
    if(H5Gclose(gid) < 0)
        HDF5_ERROR
    if(H5Pclose(gcpl_id) < 0)
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
        H5Gclose(gid);
        H5Pclose(gcpl_id);
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;

    exit(EXIT_FAILURE);
} /* end main */


/* dset_reader.c
 *
 * Usage: dset_writer
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hdf5.h"

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
read_dataset(hid_t loc_id, const char *dataset_name, const H5L_info_t *info, void *op_data)
{
    int *n_iterations = (int *)op_data;     /* user data                    */
    hid_t did = -1;             /* dataset ID                               */
    hid_t sid = -1;             /* dataspace ID                             */
    hssize_t n_elements = 0;    /* # of data elements per dataset           */
    int *data = NULL;           /* data buffer                              */

    /* Open the dataset */
    if((did = H5Dopen2(loc_id, dataset_name, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Get the number of elements in the dataset (via the dataset's dataspace) */
    if((sid = H5Dget_space(did)) < 0)
        HDF5_ERROR
    if((n_elements = H5Sget_simple_extent_npoints(sid)) < 0)
        HDF5_ERROR

    /* Create a memory buffer to contain the data */
    if(NULL == (data = (int *)calloc((size_t)n_elements, sizeof(int))))
        PROGRAM_ERROR("Could not allocate memory for data buffer")

    /* Read the data */
    if(H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
        HDF5_ERROR

    /* Close everything we created */
    free(data);
    if(H5Dclose(did) < 0)
        HDF5_ERROR
    if(H5Sclose(sid) < 0)
        HDF5_ERROR

    /* Increment the counter (in the user data) */
    (*n_iterations)++;

    return SUCCESS;

error:
    free(data);
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Sclose(sid);
    } H5E_END_TRY;

    return FAILURE;

} /* end read_dataset */

int
main(void)
{
    /* The library returns IDs which are then used to refer to HDF5 objects.
     * It is good practice to set them to -1 in case they get used before
     * they are set.
     */
    hid_t fid = -1;         /* HDF5 file ID                                 */
    hid_t gid = -1;         /* group ID                                     */
    int n_iterations = 0;   /* # of datasets inspected                      */

    /* Open the HDF5 file */
    if((fid = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Open the group which contains the datasets */
    if((gid = H5Gopen2(fid, GROUP_NAME, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    /* Open all the datasets in the file and read their data.
     * We do this in creation order since that is how the Fermi data will
     * most likely be processed.
     */
    if(H5Literate(gid, H5_INDEX_CRT_ORDER, H5_ITER_INC, NULL, read_dataset, (void *)(&n_iterations)) < 0)
        HDF5_ERROR

    /* Close all the open IDs.
     * Close order does not matter unless you tell the library to
     * care about it.
     */
    if(H5Gclose(gid) < 0)
        HDF5_ERROR
    if(H5Fclose(fid) < 0)
        HDF5_ERROR

    printf("Inspected %d datasets\n", n_iterations);

    exit(EXIT_SUCCESS);

error:
    /* It's best practice to try to close everything at the end, even
     * if there were errors.
     */
    H5E_BEGIN_TRY {
        H5Gclose(gid);
        H5Fclose(fid);
    } H5E_END_TRY;

    exit(EXIT_FAILURE);
} /* end main */


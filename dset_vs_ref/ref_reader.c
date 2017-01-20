/* ref_reader.c
 *
 * Usage: ref_writer
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdf5.h"

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
read_all_datasets(hid_t fid, unsigned *n_datasets_inspected)
{
    hid_t data_did = -1;
    hid_t data_sid = -1;
    hid_t ref_did = -1;
    hid_t ref_sid = -1;         /* dataspace ID for ref dataset             */
    hid_t mem_sid = -1;         /* dataspace ID for memory                  */
    hssize_t n_references = 0;  /* # of references in the dataset           */
    hsize_t mem_dims[NDIMS];    /* size of memory buffer                    */
    int i;                      /* iterator                                 */
    hdset_reg_ref_t region;     /* dataset region reference                 */
    int *data = NULL;           /* data buffer                              */
    hsize_t start[NDIMS];
    hsize_t stride[NDIMS];
    hsize_t count[NDIMS];
    hsize_t block[NDIMS];
    hssize_t n_elements = -1;

    /* Open the reference dataset and get its size */
    if((ref_did = H5Dopen2(fid, REF_DSET_NAME, H5P_DEFAULT)) < 0)
        HDF5_ERROR
    if((ref_sid = H5Dget_space(ref_did)) < 0)
        HDF5_ERROR
    if((n_references = H5Sget_simple_extent_npoints(ref_sid)) < 0)
        HDF5_ERROR
    if(H5Dclose(ref_did) < 0)
        HDF5_ERROR
    if(H5Sclose(ref_sid) < 0)
        HDF5_ERROR

    /* The loop repeats all operations, which is probably closer to the
     * actual operations that would take place in a pipeline as opposed
     * to cacheing IDs, etc.
     */
    for(i = 0; i < (int)n_references; i++) {

        /* Open the reference dataset */
        if((ref_did = H5Dopen2(fid, REF_DSET_NAME, H5P_DEFAULT)) < 0)
            HDF5_ERROR
        if((ref_sid = H5Dget_space(ref_did)) < 0)
            HDF5_ERROR

        /* create a new simple dataspace to describe the region buffer in memory. */
        mem_dims[0] = 1;
        if((mem_sid = H5Screate_simple(NDIMS, mem_dims, mem_dims)) < 0)
            HDF5_ERROR

        /* Set up hyperslab for read */
        start[0] = i;
        stride[0] = 1;
        count[0] = 1;
        block[0] = 1;
        if(H5Sselect_hyperslab(ref_sid, H5S_SELECT_SET, start, stride, count, block) < 0)
            HDF5_ERROR

        /* Read the region */
        if(H5Dread(ref_did, H5T_STD_REF_DSETREG, mem_sid, ref_sid, H5P_DEFAULT, &region) < 0)
            HDF5_ERROR

        /* Dereference the region */
        if((data_did = H5Rdereference2(ref_did, H5P_DEFAULT, H5R_DATASET_REGION, &region)) < 0)
            HDF5_ERROR

        /* Get a dataspace that represents the region in the data dataset */
        if((data_sid = H5Rget_region(ref_did, H5R_DATASET_REGION, &region)) < 0)
            HDF5_ERROR

        /* Get the region size and create the data buffer */
        if((n_elements = H5Sget_select_npoints(data_sid)) < 0)
            HDF5_ERROR
        if(0 == i)
            printf("number of elements per read: %llu\n", (long long unsigned)n_elements);
        if(NULL == (data = (int *)calloc((size_t)n_elements, sizeof(int))))
            PROGRAM_ERROR("Could not allocate memory for data buffer")

        /* create a new simple dataspace to describe the region buffer in memory. */
        if(H5Sclose(mem_sid) < 0)
            HDF5_ERROR
        mem_dims[0] = n_elements;
        if((mem_sid = H5Screate_simple(NDIMS, mem_dims, mem_dims)) < 0)
            HDF5_ERROR

        /* Read the points */
        if(H5Dread(data_did, H5T_NATIVE_INT, mem_sid, data_sid, H5P_DEFAULT, data) < 0)
            HDF5_ERROR

        /* Close everything for this iteration */
        free(data);
        if(H5Dclose(data_did) < 0)
            HDF5_ERROR
        if(H5Dclose(ref_did) < 0)
            HDF5_ERROR
        if(H5Sclose(data_sid) < 0)
            HDF5_ERROR
        if(H5Sclose(ref_sid) < 0)
            HDF5_ERROR
        if(H5Sclose(mem_sid) < 0)
            HDF5_ERROR
    }

    *n_datasets_inspected = i;
    
    return SUCCESS;

error:
    free(data);
    H5E_BEGIN_TRY {
        H5Dclose(data_did);
        H5Dclose(ref_did);
        H5Sclose(data_sid);
        H5Sclose(ref_sid);
        H5Sclose(mem_sid);
    } H5E_END_TRY;

    return FAILURE;

} /* read all datasets */

int
main(void)
{
    /* The library returns IDs which are then used to refer to HDF5 objects.
     * It is good practice to set them to -1 in case they get used before
     * they are set.
     */
    hid_t fid = -1;         /* HDF5 file ID                                 */
    unsigned n_dsets = 0;   /* # of datasets inspected                      */

    /* Open the HDF5 file */
    if((fid = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
        HDF5_ERROR

    if(read_all_datasets(fid, &n_dsets) != SUCCESS)
        PROGRAM_ERROR("Unable to read data in datasets")

    /* Close all the open IDs.
     * Close order does not matter unless you tell the library to
     * care about it.
     */
    if(H5Fclose(fid) < 0)
        HDF5_ERROR

    printf("Inspected %d datasets\n", n_dsets);

    exit(EXIT_SUCCESS);

error:
    /* It's best practice to try to close everything at the end, even
     * if there were errors.
     */
    H5E_BEGIN_TRY {
        H5Fclose(fid);
    } H5E_END_TRY;

    exit(EXIT_FAILURE);
} /* end main */

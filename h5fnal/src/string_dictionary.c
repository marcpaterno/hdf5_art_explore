/* string_dictionary.c
 *
 * A (cheap) implementation of a dictionary of strings. This code just
 * demonstrates the basic technique and would probably need to be
 * generalized and made more efficient for production code.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "h5fnal.h"

#define INITIAL_N_STRINGS   16
#define CONCAT_STRING_INCR  4096

/* Dataset names for the string collection */
#define H5FNAL_STRINGS_DATASET_NAME     "dict_strings"
#define H5FNAL_INDICES_DATASET_NAME     "dict_indices"


/************************************************************************
 * create_index_type()
 ************************************************************************/
static hid_t
create_index_type(void)
{
    hid_t tid = H5FNAL_BAD_HID_T;

    if ((tid = H5Tcreate(H5T_COMPOUND, sizeof(dict_index_t))) < 0)
        H5FNAL_HDF5_ERROR;

    if (H5Tinsert(tid, "start", HOFFSET(dict_index_t, start),  H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tinsert(tid, "end",   HOFFSET(dict_index_t, end),    H5T_NATIVE_HSIZE) < 0)
        H5FNAL_HDF5_ERROR;

    return tid;

error:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
    } H5E_END_TRY;

    return H5FNAL_BAD_HID_T;
} /* create_index_type */


/************************************************************************
 * close_dict_on_err()
 ************************************************************************/
static void
close_dict_on_err(string_dictionary_t *dict)
{
    if (dict) {
        H5E_BEGIN_TRY {
            H5Dclose(dict->strings_dset_id);
            H5Dclose(dict->indices_dset_id);
            H5Tclose(dict->strings_dtype_id);
            H5Tclose(dict->indices_dtype_id);
        } H5E_END_TRY;

        dict->strings_dset_id   = H5FNAL_BAD_HID_T;
        dict->indices_dset_id   = H5FNAL_BAD_HID_T;
        dict->strings_dtype_id   = H5FNAL_BAD_HID_T;
        dict->indices_dtype_id   = H5FNAL_BAD_HID_T;
    }

    return;
} /* end close_dict_on_err() */

/************************************************************************
 * close_dict_hdf5_ids()
 ************************************************************************/
static herr_t
close_dict_hdf5_ids(string_dictionary_t *dict)
{
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");

    /* Close HDF5 objects */
    if (H5Dclose(dict->strings_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dclose(dict->indices_dset_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(dict->strings_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Tclose(dict->indices_dtype_id) < 0)
        H5FNAL_HDF5_ERROR;

    /* Reset the struct to avoid accidental reuse */
    dict->strings_dset_id   = H5FNAL_BAD_HID_T;
    dict->indices_dset_id   = H5FNAL_BAD_HID_T;
    dict->strings_dtype_id  = H5FNAL_BAD_HID_T;
    dict->indices_dtype_id  = H5FNAL_BAD_HID_T;

    return H5FNAL_SUCCESS;

error:
    if (dict)
        close_dict_on_err(dict);

    return H5FNAL_FAILURE;
} /* end close_dict_hdf5_ids() */

/************************************************************************
 * create_string_dictionary()
 ************************************************************************/
herr_t
create_string_dictionary(hid_t loc_id, string_dictionary_t *dict)
{
    hsize_t chunk_dim;

    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("loc_id parameter cannot be negative");
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");

    /* Initialize the data product struct */
    memset(dict, 0, sizeof(string_dictionary_t));

    /* Create the in-memory structures */
    /* indices */
    if (NULL == (dict->indices = (dict_index_t *)calloc((size_t)INITIAL_N_STRINGS, sizeof(dict_index_t))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for indices");
    dict->n_allocated = INITIAL_N_STRINGS;
    /* concatenated strings */
    if (NULL == (dict->concat_strings = (char *)calloc((size_t)CONCAT_STRING_INCR, sizeof(char))))
        H5FNAL_PROGRAM_ERROR("couldn't allocate memory for strings");
    dict->total_string_alloc = CONCAT_STRING_INCR;

    /* Create HDF5 types */
    if ((dict->strings_dtype_id = H5Tcopy(H5T_NATIVE_CHAR)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((dict->indices_dtype_id = create_index_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");

    /* Create the HDF5 datasets that will store the string data */
    chunk_dim = 1024; /* arbitrary */
    if (h5fnal_create_1D_dset(loc_id, H5FNAL_STRINGS_DATASET_NAME, dict->strings_dtype_id, chunk_dim, &(dict->strings_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create dataset");
    if (h5fnal_create_1D_dset(loc_id, H5FNAL_INDICES_DATASET_NAME, dict->indices_dtype_id, chunk_dim, &(dict->indices_dset_id)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create dataset");

    /* Add the empty string as the first string */
    if (add_string_to_dictionary("", dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not add string to dictionary");

    /* Set the 'save on close' flag */
    dict->save_on_close = TRUE;

    return H5FNAL_SUCCESS;

error:
    if (dict)
        close_dict_on_err(dict);
    return H5FNAL_FAILURE;
} /* end create_string_dictionary() */

/************************************************************************
 * read_all_strings()
 ************************************************************************/
static herr_t
read_all_strings(string_dictionary_t *dict)
{
    hid_t       sid     = H5FNAL_BAD_HID_T;

    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL")

    /* Get the size of the indices dataset */
    if ((sid = H5Dget_space(dict->indices_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((dict->n_strings = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Get the size of the strings dataset */
    if ((sid = H5Dget_space(dict->strings_dset_id)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((dict->total_string_size = H5Sget_simple_extent_npoints(sid)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Sclose(sid) < 0)
        H5FNAL_HDF5_ERROR;

    /* Generate buffers for reading the indices and strings */
    dict->n_allocated = dict->n_strings;
    dict->total_string_alloc = dict->total_string_size;
    if (NULL == (dict->indices = (dict_index_t *)calloc(dict->n_allocated, sizeof(dict_index_t))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for indices");
    if (NULL == (dict->concat_strings = (char *)calloc(dict->total_string_alloc, sizeof(char))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for strings");

    /* Read the data from the datasets */
    if (H5Dread(dict->indices_dset_id, dict->indices_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, dict->indices) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Dread(dict->strings_dset_id, dict->strings_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, dict->concat_strings) < 0)
        H5FNAL_HDF5_ERROR;

    return H5FNAL_SUCCESS;

error:
    H5E_BEGIN_TRY {
        H5Sclose(sid);
    } H5E_END_TRY;

    return H5FNAL_FAILURE;
    
} /* end read_all_strings() */

herr_t
open_string_dictionary(hid_t loc_id, string_dictionary_t *dict)
{
    if (loc_id < 0)
        H5FNAL_PROGRAM_ERROR("loc_id parameter cannot be negative");
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");

    /* Initialize the data product struct */
    memset(dict, 0, sizeof(string_dictionary_t));

    /* Create HDF5 types */
    if ((dict->strings_dtype_id = H5Tcopy(H5T_NATIVE_CHAR)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((dict->indices_dtype_id = create_index_type()) < 0)
        H5FNAL_PROGRAM_ERROR("could not create datatype");

    /* Open the HDF5 datasets that store the string data */
    if ((dict->strings_dset_id = H5Dopen2(loc_id, H5FNAL_STRINGS_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;
    if ((dict->indices_dset_id = H5Dopen2(loc_id, H5FNAL_INDICES_DATASET_NAME, H5P_DEFAULT)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Read all the string data */
    if (read_all_strings(dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not strings after open");

    /* Set the 'save on close' flag */
    dict->save_on_close = FALSE;

    return H5FNAL_SUCCESS;

error:
    if (dict)
        close_dict_on_err(dict);
    return H5FNAL_FAILURE;
} /* end open_string_dictionary() */



herr_t
close_string_dictionary(string_dictionary_t *dict)
{
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");

    if (dict->save_on_close) {
        /* Write out the indices */
        if (h5fnal_append_data(dict->indices_dset_id, dict->indices_dtype_id, dict->n_strings, (const void *)dict->indices) < 0)
            H5FNAL_PROGRAM_ERROR("could not write indices");

        /* Write the strings to the file */
        if (h5fnal_append_data(dict->strings_dset_id, dict->strings_dtype_id, (hsize_t)dict->total_string_size, (const void *)dict->concat_strings) < 0)
            H5FNAL_PROGRAM_ERROR("could not write strings");
    }

    /* Shut down the HDF5 IDs */
    if (close_dict_hdf5_ids(dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not close string dictionary HDF5 IDs");
     return H5FNAL_SUCCESS;

error:
    if (dict)
        close_dict_on_err(dict);

    return H5FNAL_FAILURE;
} /* end close_string_dictionary() */

herr_t
add_string_to_dictionary(const char *s, string_dictionary_t *dict)
{
    unsigned u;
    size_t len;

    if (!s)
        H5FNAL_PROGRAM_ERROR("s parameter cannot be NULL");
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");

    /* Get the length of the new string.
     * We'll be storing the terminal \0, so add one for that.
     */
    len = strlen(s) + 1;

    /* Increase the indices array size, if necessary */
    if (dict->n_strings == dict->n_allocated) {
        dict->n_allocated += INITIAL_N_STRINGS;
        if (NULL == (dict->indices = (dict_index_t *)realloc((void *)dict->indices, dict->n_allocated * sizeof(dict_index_t))))
            H5FNAL_PROGRAM_ERROR("could not reallocate memory for string indices expansion");
    }

    /* Increase the strings array size, if necessary */
    if (dict->total_string_size + len > dict->total_string_alloc) {
        dict->total_string_alloc += CONCAT_STRING_INCR;
        if (NULL == (dict->concat_strings = (char *)realloc((void *)dict->concat_strings, dict->total_string_alloc * sizeof(char))))
            H5FNAL_PROGRAM_ERROR("could not reallocate memory for string array expansion");
    }


    /* Add the string index info */
    u = dict->n_strings;
    dict->indices[u].start = dict->total_string_size;
    dict->indices[u].end = dict->total_string_size + len - 1;
    dict->n_strings++;

    /* Copy the string */
    u = dict->total_string_size;
    strcpy(&(dict->concat_strings[u]), s);
    dict->total_string_size += len;

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end add_string_to_dictionary() */

herr_t
get_string_index(const char *s, string_dictionary_t *dict, /*OUT*/ hbool_t *found, /*OUT*/ unsigned *index)
{
    unsigned u;

    if (!s)
        H5FNAL_PROGRAM_ERROR("s parameter cannot be NULL (the string dictionary never contains NULL)");
    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");
    if (!found)
        H5FNAL_PROGRAM_ERROR("found parameter cannot be NULL");
    if (!index)
        H5FNAL_PROGRAM_ERROR("index parameter cannot be NULL");

    *found = FALSE;
    *index = dict->n_strings; // Where the next string will go if not found.

    /* Wildly inefficient. Should be a hash table. */
    for (u = 0; u < dict->n_strings; u++) {
        /* Point to the beginning of the next string */
        char *dict_s = dict->concat_strings + dict->indices[u].start;

        /* Same? */
        if (!strcmp(s, dict_s)) {
            *found = TRUE;
            *index = u;
            break;
        }
    }

    return H5FNAL_SUCCESS;

error:
    return H5FNAL_FAILURE;
} /* end get_string_index() */

herr_t
get_string(string_dictionary_t *dict, unsigned index, /*OUT*/ char **s)
{
    size_t i;
    size_t len;
    char *out = NULL;

    if (!dict)
        H5FNAL_PROGRAM_ERROR("dict parameter cannot be NULL");
    if (index > dict->n_strings - 1)
        H5FNAL_PROGRAM_ERROR("index is larger than the number of strings in the dictionary");
    if (!s)
        H5FNAL_PROGRAM_ERROR("s parameter cannot be NULL");

    /* Allocate a buffer for the string */
    len = 1 + (dict->indices[index].end - dict->indices[index].start);
    if (NULL == (out = (char *)calloc(len, sizeof(char))))
        H5FNAL_PROGRAM_ERROR("could not allocate memory for string");

    /* Copy the string data */
    i = dict->indices[index].start;
    strcpy(out, &(dict->concat_strings[i]));

    *s = out;

    return H5FNAL_SUCCESS;

error:
    free(out);

    return H5FNAL_FAILURE;
} /* end get_string() */


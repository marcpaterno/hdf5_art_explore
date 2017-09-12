/* Test the string dictionary API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "h5fnal.h"
#include "string_dictionary.h"

#define FILE_NAME   "string_dictionary.h5"

#define STRING_NOT_FOUND    "this string is not stored"
#define STRING_EMPTY        ""
#define STRING_SHORT        "foo"
#define STRING_LONG         "This is a longer string with spaces!\n"

int
main(void)
{
    hid_t   fid = -1;
    hid_t   fapl_id = -1;
    string_dictionary_t *dict = NULL;
    hbool_t found;
    unsigned index;
    char *s = NULL;

    printf("Testing string dictionary operations... ");

    /* Create the file */
    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        H5FNAL_HDF5_ERROR;
    if ((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
        H5FNAL_HDF5_ERROR;

    /* Create a string dictionary */
    if (NULL == (dict = (string_dictionary_t *)calloc(1, sizeof(string_dictionary_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for dictionary");
    if (create_string_dictionary(fid, dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not create string dictionary");

    /* Add some strings */
    if (add_string_to_dictionary(STRING_SHORT, dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not add string to dictionary");
    if (add_string_to_dictionary(STRING_LONG, dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not add string to dictionary");

    /* Close (and save) the dictionary */
    if(close_string_dictionary(dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not close string dictionary");

    /* Re-open the dictionary */
    if (open_string_dictionary(fid, dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not open string dictionary");

    /* Get the indexes of the strings */
    if (get_string_index(STRING_NOT_FOUND, dict, &found, &index) < 0)
        H5FNAL_PROGRAM_ERROR("problem checking for string");
    if(found)
        H5FNAL_PROGRAM_ERROR("should not have found this string");

    if (get_string_index(STRING_EMPTY, dict, &found, &index) < 0)
        H5FNAL_PROGRAM_ERROR("problem checking for string");
    if(!found || index != 0)
        H5FNAL_PROGRAM_ERROR("wrong string index returned");

    if (get_string_index(STRING_SHORT, dict, &found, &index) < 0)
        H5FNAL_PROGRAM_ERROR("problem checking for string");
    if(!found || index != 1)
        H5FNAL_PROGRAM_ERROR("wrong string index returned");

    if (get_string_index(STRING_LONG, dict, &found, &index) < 0)
        H5FNAL_PROGRAM_ERROR("problem checking for string");
    if(!found || index != 2)
        H5FNAL_PROGRAM_ERROR("wrong string index returned");

    /* Check the string values */
    if (get_string(dict, 0, &s) < 0)
        H5FNAL_PROGRAM_ERROR("problem getting string");
    if (strcmp(STRING_EMPTY, s))
        H5FNAL_PROGRAM_ERROR("wrong string returned");
    free(s);

    if (get_string(dict, 1, &s) < 0)
        H5FNAL_PROGRAM_ERROR("problem getting string");
    if (strcmp(STRING_SHORT, s))
        H5FNAL_PROGRAM_ERROR("wrong string returned");
    free(s);

    if (get_string(dict, 2, &s) < 0)
        H5FNAL_PROGRAM_ERROR("problem getting string");
    if (strcmp(STRING_LONG, s))
        H5FNAL_PROGRAM_ERROR("wrong string returned");
    free(s);

    /* Close everything */
    if(close_string_dictionary(dict) < 0)
        H5FNAL_PROGRAM_ERROR("could not close string dictionary");

    if (H5Pclose(fapl_id) < 0)
        H5FNAL_HDF5_ERROR;
    if (H5Fclose(fid) < 0)
        H5FNAL_HDF5_ERROR;
    free(dict);

    printf("SUCCESS!\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY {
        H5Pclose(fapl_id);
        H5Fclose(fid);
    } H5E_END_TRY;
    if (dict)
        close_string_dictionary(dict);
    free(dict);
    free(s);

    printf("*** FAILURE ***\n");

    exit(EXIT_FAILURE);
}


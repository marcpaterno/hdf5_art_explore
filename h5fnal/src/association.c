/* association.c */

#include "h5fnal.h"

herr_t
h5fnal_create_association(hid_t loc_id, const char *name, h5fnal_association_t *assn)
{
    if(loc_id < 0)
        H5FNAL_PROGRAM_ERROR("loc_id parameter cannot be negative")
    if(NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL")
    if(NULL == assn)
        H5FNAL_PROGRAM_ERROR("assn parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_create_association */

herr_t
h5fnal_open_association(hid_t loc_id, const char *name, h5fnal_association_t *assn)
{
    if(loc_id < 0)
        H5FNAL_PROGRAM_ERROR("loc_id parameter cannot be negative")
    if(NULL == name)
        H5FNAL_PROGRAM_ERROR("name parameter cannot be NULL")
    if(NULL == assn)
        H5FNAL_PROGRAM_ERROR("assn parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_open_association */

herr_t
h5fnal_close_association(h5fnal_association_t *assn)
{
    if(NULL == assn)
        H5FNAL_PROGRAM_ERROR("assn parameter cannot be NULL")

error:
    return H5FNAL_FAILURE;
} /* h5fnal_close_association */


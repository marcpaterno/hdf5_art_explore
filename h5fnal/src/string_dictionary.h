/* string_dictionary.h
 *
 * Header for an internal string dictionary.
 */

#ifndef H5FNAL_STRING_DICTIONARY_H
#define H5FNAL_STRING_DICTIONARY_H


typedef struct string_dictionary_t {
    unsigned n_strings;
    char **strings;
} string_dictionary_t;

#ifdef __cplusplus
extern "C" {
#endif

herr_t create_string_dict(string_dictionary_t *dict);
herr_t destroy_string_dict(string_dictionary_t *dict);
unsigned add_string_to_dict(const char *s);
unsigned get_string_index(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* H5FNAL_STRING_DICTIONARY_H */


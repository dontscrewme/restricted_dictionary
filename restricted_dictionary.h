#ifndef RESTRICTED_DICTIONARY_H
#define RESTRICTED_DICTIONARY_H

struct dictionary;
struct restricted_dictionary;

struct restricted_dictionary *restricted_dictionary_new(struct dictionary* base_dict);
void restricted_dictionary_del(struct restricted_dictionary *r_dict);
int restricted_dictionary_set(struct restricted_dictionary *r_dict,
                              const char *key, const char *val);

int restricted_dictionary_restrict(struct restricted_dictionary *r_dict,
                                   char *slave_pair, char *master_pair);
int restricted_dictionary_multiRestrict(struct restricted_dictionary *r_dict,
                                        char *slave_pair, char **master_pairs,
                                        unsigned int num_masters);

int restricted_dictionary_unrestrict(struct restricted_dictionary *r_dict,
                                     char *slave_pair, char *master_pair);
int restricted_dictionary_unrestrict_all(struct restricted_dictionary *r_dict,
                                         char *slave_pair);

int restricted_dictionary_get_restrictions(struct restricted_dictionary *r_dict,
                                           const char *slave_pair,
                                           char ***restrictions,
                                           unsigned int *count);


#endif // RESTRICTED_DICTIONARY_H

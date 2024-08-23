#include "restricted_dictionary.h"
#include "hierarchy_list.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void restricted_dictionary_set_error_callback(int (*errback)(const char *,
                                                             ...)) {
  dictionary_set_error_callback(errback);
}

struct restricted_dictionary {
  struct dictionary *base;
  struct list_head parent_head;
};

static int has_restriction(struct restricted_dictionary *r_dict,
                           const char *key, const char *val) {

  struct parent *slave = find_parent(&r_dict->parent_head, key, val);
  if (!slave) {
    return 0;
  }

  struct child *current_master;
  list_for_each_entry(current_master, &slave->child_head, list) {
    const char *value_in_dict =
        dictionary_get(r_dict->base, current_master->key, "NULL");
    if (strcmp("NULL", value_in_dict) != 0 &&
        strcmp(value_in_dict, current_master->value) == 0) {
      return 1;
    }
  }

  return 0;
}

struct restricted_dictionary *restricted_dictionary_new(unsigned int size) {
  struct restricted_dictionary *r_dict =
      malloc(sizeof(struct restricted_dictionary));
  if (!r_dict) {
    error_callback("%s: malloc() failed\n", __func__);
    return NULL;
  }

  r_dict->base = dictionary_new(size);
  if (!r_dict->base) {
    error_callback("%s: dictionary_new() failed\n", __func__);
    free(r_dict);
    return NULL;
  }

  INIT_LIST_HEAD(&r_dict->parent_head);

  return r_dict;
}

void restricted_dictionary_del(struct restricted_dictionary *r_dict) {
  if (!r_dict) {
    return;
  }

  if (r_dict->base) {
    dictionary_del(r_dict->base);
  }

  unset_parents(&r_dict->parent_head);

  free(r_dict);
}

int restricted_dictionary_set(struct restricted_dictionary *r_dict,
                              const char *key, const char *val) {
  if (!r_dict || !key || !val) {
    error_callback("%s: invalid input\n", __func__);
    return -1;
  }

  if (has_restriction(r_dict, key, val)) {
    error_callback("%s: restriction prevents setting key=%s, val=%s\n",
                   __func__, key, val);
    return -1;
  }

  return dictionary_set(r_dict->base, key, val);
}

static int is_valid_pair(const char *pair) {

  char *equal_sign = strchr(pair, '=');
  if (!equal_sign || equal_sign == pair || *(equal_sign + 1) == '\0') {
    return 0;
  }

  return 1;
}

static int split_pair(const char *pair, char **key, char **value) {

  char *equal_sign = strchr(pair, '=');
  if (!equal_sign) {
    error_callback("%s: strchr() failed\n", __func__);
    return -1;
  }

  *key = strndup(pair, equal_sign - pair);
  if (!*key) {
    error_callback("%s: strndup() failed\n", __func__);
    return -1;
  }
  *value = strdup(equal_sign + 1);
  if (!*value) {
    error_callback("%s: strdup() failed\n", __func__);
    return -1;
  }

  return 0;
}

static int add_restriction(struct parent *slave_parent,
                           const char *master_pair) {
  char *master_key = NULL;
  char *master_value = NULL;
  if (split_pair(master_pair, &master_key, &master_value) == -1) {
    error_callback("%s: split_pair(master) failed\n", __func__);
    return -1;
  }

  struct child *child = set_child(slave_parent, master_key, master_value);
  if (!child) {
    error_callback("%s: set_child() failed\n", __func__);
    free(master_key);
    free(master_value);
    return -1;
  }

  free(master_key);
  free(master_value);

  return 0;
}

static struct parent *split_and_set_parent(struct restricted_dictionary *r_dict,
                                           const char *slave_pair,
                                           char **slave_key,
                                           char **slave_value) {
  if (!is_valid_pair(slave_pair)) {
    error_callback("%s: invalid slave pair format, expected 'A=B'\n", __func__);
    return NULL;
  }

  if (split_pair(slave_pair, slave_key, slave_value) == -1) {
    error_callback("%s: split_pair(slave) failed\n", __func__);
    return NULL;
  }

  struct parent *slave_parent =
      set_parent(&r_dict->parent_head, *slave_key, *slave_value);
  if (!slave_parent) {
    error_callback("%s: set_parent() failed\n", __func__);
    free(*slave_key);
    free(*slave_value);
    return NULL;
  }

  return slave_parent;
}

int restricted_dictionary_restrict(struct restricted_dictionary *r_dict,
                                   char *slave_pair, char *master_pair) {
  if (!r_dict || !slave_pair || !master_pair) {
    error_callback("%s: invalid input\n", __func__);
    return -1;
  }

  char *slave_key = NULL;
  char *slave_value = NULL;
  struct parent *slave_parent =
      split_and_set_parent(r_dict, slave_pair, &slave_key, &slave_value);
  if (!slave_parent) {
    error_callback("%s: split_and_set_parent() failed\n", __func__);
    return -1;
  }

  if (add_restriction(slave_parent, master_pair) == -1) {
    error_callback("%s: add_restriction() failed\n", __func__);
    free(slave_key);
    free(slave_value);
    return -1;
  }

  free(slave_key);
  free(slave_value);

  return 0;
}

int restricted_dictionary_multiRestrict(struct restricted_dictionary *r_dict,
                                        char *slave_pair, char **master_pairs,
                                        unsigned int num_masters) {
  if (!r_dict || !slave_pair || !master_pairs || num_masters <= 0) {
    error_callback("%s: invalid input\n", __func__);
    return -1;
  }

  char *slave_key = NULL;
  char *slave_value = NULL;
  struct parent *slave_parent =
      split_and_set_parent(r_dict, slave_pair, &slave_key, &slave_value);
  if (!slave_parent) {
    error_callback("%s: split_and_set_parent() failed\n", __func__);
    return -1;
  }

  int ret = 0;

  for (int i = 0; i < num_masters; i++) {
    if (!is_valid_pair(master_pairs[i])) {
      error_callback(
          "%s: invalid master pair format at index %d, expected 'A=B'\n",
          __func__, i);
      ret = -1;
      continue;
    }

    if (add_restriction(slave_parent, master_pairs[i]) != 0) {
      error_callback("%s: add_restriction(%s) failed, keep adding next one\n",
                     __func__, master_pairs[i]);
      ret = -1;
    }
  }

  free(slave_key);
  free(slave_value);

  return ret;
}

int restricted_dictionary_unrestrict(struct restricted_dictionary *r_dict,
                                     char *slave_pair, char *master_pair) {
  if (!r_dict || !slave_pair || !master_pair) {
    error_callback("%s: invalid input\n", __func__);
    return -1;
  }

  if (!is_valid_pair(slave_pair) || !is_valid_pair(master_pair)) {
    error_callback("%s: invalid pair format, expected 'A=B'\n", __func__);
    return -1;
  }

  char *slave_key = NULL;
  char *slave_value = NULL;
  if (split_pair(slave_pair, &slave_key, &slave_value) == -1) {
    error_callback("%s: split_pair(slave) failed\n", __func__);
    return -1;
  }

  struct parent *slave_parent =
      find_parent(&r_dict->parent_head, slave_key, slave_value);
  if (!slave_parent) {
    error_callback("%s: slave parent not found\n", __func__);
    free(slave_key);
    free(slave_value);
    return -1;
  }

  char *master_key = NULL;
  char *master_value = NULL;
  if (split_pair(master_pair, &master_key, &master_value) == -1) {
    error_callback("%s: split_pair(master) failed\n", __func__);
    free(slave_key);
    free(slave_value);
    return -1;
  }

  // Find and remove the master pair as a child of the slave parent
  struct child *child = find_child(slave_parent, master_key, master_value);
  if (!child) {
    error_callback("%s: master pair not found\n", __func__);
    free(slave_key);
    free(slave_value);
    free(master_key);
    free(master_value);
    return -1;
  }

  unset_child(slave_parent, master_key, master_value);

  unsigned int num_children = 0;
  if (getNumOfChildren(slave_parent, &num_children) == -1) {
    error_callback("%s: getNumOfChildren() failed\n", __func__);
    free(slave_key);
    free(slave_value);
    free(master_key);
    free(master_value);
    return -1;
  }

  if (num_children == 0) {
    unset_parent(&r_dict->parent_head, slave_key, slave_value);
  }

  free(slave_key);
  free(slave_value);
  free(master_key);
  free(master_value);

  return 0;
}

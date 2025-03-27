#ifndef HIERARCHY_LIST_H
#define HIERARCHY_LIST_H

void hierarchy_list_set_error_callback(int (*errback)(const char *, ...));

#include "list.h"

struct parent {
	char *key;
	char *value;
	struct list_head list;
	struct list_head child_head;
};

struct child {
	char *key;
	char *value;
	struct list_head list;
};

struct parent *set_parent(struct list_head *parent_head, const char *key,
													const char *value);
struct parent *find_parent(const struct list_head *parent_head, const char *key,
													 const char *value);

void unset_parents(struct list_head *parent_head);
void unset_parent(struct list_head *parent_head, const char *key,
									const char *value);

struct child *set_child(struct parent *parent, const char *key, const char *value);
struct child *find_child(struct parent *parent, const char *key,
												 const char *value);
void unset_children(struct parent *parent);
void unset_child(struct parent *parent, const char *key, const char *value);

int getNumOfChildren(struct parent *parent, unsigned int *output);

void print_all(struct list_head *parent_head);
void print_parents(struct list_head *parent_head);
void print_children(struct parent *parent);

#endif
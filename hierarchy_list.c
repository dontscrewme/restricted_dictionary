#include "hierarchy_list.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static int default_error_callback(const char *format, ...) {
	int ret;
	va_list argptr;
	va_start(argptr, format);
	ret = vfprintf(stderr, format, argptr);
	va_end(argptr);
	return ret;
}

static int (*error_callback)(const char *, ...) = default_error_callback;

void hierarchy_list_set_error_callback(int (*errback)(const char *, ...)) {
	if (errback) {
		error_callback = errback;
	}
}

static struct parent *init_parent(const char *key, const char *value) {

	struct parent *parent = (struct parent *)malloc(sizeof(struct parent));
	if (!parent) {
		error_callback("%s: malloc() failed\n", __func__);
		return NULL;
	}
	parent->key = strdup(key);
	if (!parent->key) {
		error_callback("%s: strdup() failed\n", __func__);
		free(parent);
		return NULL;
	}
	parent->value = strdup(value);
	if (!parent->value) {
		error_callback("%s: strdup() failed\n", __func__);
		free(parent->key);
		free(parent);
		return NULL;
	}

	INIT_LIST_HEAD(&parent->list);
	INIT_LIST_HEAD(&parent->child_head);
	return parent;
}

static struct child *init_child(const char *key, const char *value) {

	struct child *child = (struct child *)malloc(sizeof(struct child));
	if (!child) {
		error_callback("%s: malloc() failed\n", __func__);
		return NULL;
	}
	child->key = strdup(key);
	if (!child->key) {
		error_callback("%s: strdup() failed\n", __func__);
		free(child);
		return NULL;
	}
	child->value = strdup(value);
	if (!child->value) {
		error_callback("%s: strdup() failed\n", __func__);
		free(child->key);
		free(child);
		return NULL;
	}

	INIT_LIST_HEAD(&child->list);
	return child;
}

static void add_parent(struct parent *parent, struct list_head *head) {

	if (!list_empty(&parent->list)) {
		error_callback("%s: parent already in a list\n", __func__);
		return;
	}

	list_add(&parent->list, head);
}

static void add_child(struct child *child, struct list_head *head) {

	if (!list_empty(&child->list)) {
		error_callback("%s: child already in a list\n", __func__);
		return;
	}

	list_add(&child->list, head);
}

#define UNSET_AND_FREE(entry)                                                  \
	do {                                                                         \
		list_del(&(entry)->list);                                                  \
		free((entry)->key);                                                        \
		free((entry)->value);                                                      \
		free(entry);                                                               \
	} while (0)

struct parent *set_parent(struct list_head *parent_head, const char *key,
													const char *value) {
	if (!parent_head || !key || !value) {
		error_callback("%s: invalid input\n", __func__);
		return NULL;
	}

	struct parent *parent = find_parent(parent_head, key, value);
	if (parent) {
		return parent;
	}

	parent = init_parent(key, value);
	if (!parent) {
		error_callback("%s: init_parent() failed\n", __func__);
		return NULL;
	}

	add_parent(parent, parent_head);
	return parent;
}

struct parent *find_parent(const struct list_head *parent_head, const char *key,
													 const char *value) {
	if (!parent_head || !key || !value) {
		error_callback("%s: invalid input\n", __func__);
		return NULL;
	}

	struct parent *current_parent;
	list_for_each_entry(current_parent, parent_head, list) {
		if (strcmp(current_parent->key, key) == 0 &&
				strcmp(current_parent->value, value) == 0) {
			return current_parent;
		}
	}

	return NULL;
}

void unset_parents(struct list_head *parent_head) {
	if (!parent_head) {
		error_callback("%s: invalid input\n", __func__);
		return;
	}

	while (!list_empty(parent_head)) {
		struct parent *parent_to_unset =
				list_entry(parent_head->next, struct parent, list);
		unset_children(parent_to_unset);
		UNSET_AND_FREE(parent_to_unset);
	}
}

void unset_parent(struct list_head *parent_head, const char *key,
									const char *value) {
	if (!parent_head) {
		error_callback("%s: invalid input\n", __func__);
		return;
	}

	struct parent *parent_to_unset = find_parent(parent_head, key, value);
	if (!parent_to_unset) {
		return;
	}

	unset_children(parent_to_unset);
	UNSET_AND_FREE(parent_to_unset);
}

struct child *set_child(struct parent *parent, const char *key,
												const char *value) {
	if (!parent || !key || !value) {
		error_callback("%s: invalid input\n", __func__);
		return NULL;
	}

	struct child *child = find_child(parent, key, value);
	if (child) {
		return child;
	}

	child = init_child(key, value);
	if (!child) {
		error_callback("%s: init_child() failed\n", __func__);
		return NULL;
	}

	add_child(child, &parent->child_head);
	return child;
}

struct child *find_child(struct parent *parent, const char *key,
												 const char *value) {

	if (!parent || !key || !value) {
		error_callback("%s: invalid input\n", __func__);
		return NULL;
	}

	struct list_head *child_head = &parent->child_head;

	struct child *current_child;
	list_for_each_entry(current_child, child_head, list) {
		if (strcmp(current_child->key, key) == 0 &&
				strcmp(current_child->value, value) == 0) {
			return current_child;
		}
	}

	return NULL;
}

void unset_child(struct parent *parent, const char *key, const char *value) {
	if (!parent || !key || !value) {
		error_callback("%s: invalid input\n", __func__);
		return;
	}

	struct child *child_to_unset = find_child(parent, key, value);
	if (!child_to_unset) {
		return;
	}

	UNSET_AND_FREE(child_to_unset);
}

void unset_children(struct parent *parent) {
	if (!parent) {
		error_callback("%s: invalid input\n", __func__);
		return;
	}

	struct list_head *child_head = &parent->child_head;
	while (!list_empty(child_head)) {
		struct child *child_to_unset =
				list_entry(child_head->next, struct child, list);
		UNSET_AND_FREE(child_to_unset);
	}
}

int getNumOfChildren(struct parent *parent, unsigned int *output) {
	if (!parent) {
		error_callback("%s: invalid input\n", __func__);
		return -1;
	}

	unsigned int count = 0;
	struct child *current_child;
	list_for_each_entry(current_child, &parent->child_head, list) { count++; }

	*output = count;

	return 0;
}

void print_all(struct list_head *parent_head) {
	if (!parent_head) {
		error_callback("%s: invalid input\n", __func__);
		return;
	}

	if (list_empty(parent_head)) {
		error_callback("No parents to print.\n");
		return;
	}

	struct parent *current_parent;
	list_for_each_entry(current_parent, parent_head, list) {
		print_children(current_parent);
		printf("↓\n");
	}
}

void print_parents(struct list_head *parent_head) {
	if (list_empty(parent_head)) {
		error_callback("No parents to print.\n");
		return;
	}

	struct parent *tmp;
	list_for_each_entry(tmp, parent_head, list) {
		printf("Parent(%s=%s) -> ", tmp->key, tmp->value);
	}
	printf("\n");
}

void print_children(struct parent *parent) {
	if (!parent) {
		error_callback("%s: invalid input\n", __func__);
	}

	printf("Parent(%s=%s) : ", parent->key, parent->value);

	struct list_head *child_head = &parent->child_head;
	if (list_empty(child_head)) {
		error_callback("no child\n");
		return;
	}

	struct child *current_child;
	list_for_each_entry(current_child, &parent->child_head, list) {
		printf("Child(%s=%s) -> ", current_child->key, current_child->value);
	}
	printf("\n");
}
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "restricted_dictionary.h"

void test_new()
{
  // Test Case 1: Create a restricted dictionary with a valid size
  struct dictionary *d = dictionary_new(10);
  struct restricted_dictionary *r_dict = restricted_dictionary_new(d);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);
  dictionary_del(d);

  // Test Case 2: Create a restricted dictionary with zero size
  d = dictionary_new(0);
  r_dict = restricted_dictionary_new(d);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);
  dictionary_del(d);

  // Test Case 3: Create a restricted dictionary with a very large size
  d = dictionary_new(1000000);
  r_dict = restricted_dictionary_new(d);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);
  dictionary_del(d);
}

void test_del()
{
  // Test Case 1: Delete a NULL restricted dictionary
  restricted_dictionary_del(NULL);
}

void test_set()
{
  struct restricted_dictionary *r_dict = NULL;
  struct dictionary *d = dictionary_new(10);

  // Test Case 1: Set a NULL key
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_set(r_dict, NULL, "Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Set a NULL value
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_set(r_dict, "company", NULL) == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Set a pair
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Set pair twice
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  assert(restricted_dictionary_set(r_dict, "company", "Google Inc.") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 5: Set two pairs
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  assert(restricted_dictionary_set(r_dict, "company", "Yahoo") == 0);
  restricted_dictionary_del(r_dict);

  dictionary_del(d);
}

void test_restrict()
{
  struct restricted_dictionary *r_dict = NULL;
  struct dictionary *d = dictionary_new(10);

  // Test Case 1: Add a restriction with NULL slave pair
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, NULL, "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Add a restriction with NULL master pair
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", NULL) == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Add a restriction with invalid format slave pair
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee-Andy", "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Add a restriction with invalid format master pair
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company-Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 5: One slave with one master
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 6: One slave with two masters
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Yahoo") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 7: Two slaves with each one master
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy", "company=Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 8: Two slaves with each two masters
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Yahoo") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy", "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy", "company=Yahoo") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 9: Add multiple restrictions with multiRestrict
  r_dict = restricted_dictionary_new(d);
  char *master_pairs[] = {"company=Google", "location=USA"};
  assert(restricted_dictionary_multiRestrict(r_dict, "employee=Andy", master_pairs, 2) == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 10: Remove a restriction with unrestrict
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy", "company=Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 11: Remove a non-existing restriction with unrestrict
  r_dict = restricted_dictionary_new(d);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy", "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 12: Unrestrict all with unrestrict_all
  r_dict = restricted_dictionary_new(d);  // Create the restricted dictionary
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_unrestrict_all(r_dict, "employee=Andy") == 0);
  restricted_dictionary_del(r_dict);


  dictionary_del(d);
}

int main()
{
  test_new();
  test_del();
  test_set();
  test_restrict();
  printf("All test cases passed!\n");

  return 0;
}

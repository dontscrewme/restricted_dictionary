#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "restricted_dictionary.h"

void test_new() {
  // Test Case 1: Create a restricted dictionary with a valid size
  struct restricted_dictionary *r_dict = NULL;
  r_dict = restricted_dictionary_new(10);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Create a restricted dictionary with zero size
  r_dict = restricted_dictionary_new(0);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Create a restricted dictionary with a very large size
  r_dict = restricted_dictionary_new(1000000);
  assert(r_dict != NULL);
  restricted_dictionary_del(r_dict);
}
void test_del() {
  // Test Case 1: Delete a NULL restricted dictionary
  restricted_dictionary_del(NULL);
}

void test_set() {
  struct restricted_dictionary *r_dict = NULL;

  // Test Case 1: Set a NULL key
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_set(r_dict, NULL, "Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Set a NULL value
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_set(r_dict, "company", NULL) == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Set a pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Set pair twice
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  assert(restricted_dictionary_set(r_dict, "company", "Google Inc.") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 5: Set two pairs
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  assert(restricted_dictionary_set(r_dict, "company", "Yahoo") == 0);
  restricted_dictionary_del(r_dict);
}

void test_restrict() {
  struct restricted_dictionary *r_dict = NULL;

  // Test Case 1: Add a restriction with NULL slave pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, NULL, "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Add a restriction with NULL master pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", NULL) == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Add a restriction with invalid format slave pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee-Andy",
                                        "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Add a restriction with invalid format master pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company-Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 5: One slave with one master
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 6: One slave with two masters
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Yahoo") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 7: Two slaves with each one master
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy",
                                        "company=Google") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 8: Two slaves with each two masters
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Yahoo") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_restrict(r_dict, "employee=Billy",
                                        "company=Yahoo") == 0);
  restricted_dictionary_del(r_dict);
}

void test_unrestrict() {

  struct restricted_dictionary *r_dict = NULL;

  // Test Case 1: Remove a restriction with NULL slave pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_unrestrict(r_dict, NULL, "company=Google") ==
         -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Remove a restriction with NULL master pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy", NULL) == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Remove a restriction with invalid format slave pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_unrestrict(r_dict, "employee-Andy",
                                          "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Remove a restriction with invalid format slave pair
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy",
                                          "company-Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 5: Remove a restriction that does not exist
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy",
                                          "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 6: Remove a restriction that slave does not exist
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Billy",
                                          "company=Google") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 7: Remove a restriction that master does not exist
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy",
                                          "company=Yahoo") == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 8: Remove a restriction
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_unrestrict(r_dict, "employee=Andy",
                                          "company=Google") == 0);
  restricted_dictionary_del(r_dict);
}

void test_restrict_then_set() {

  struct restricted_dictionary *r_dict = NULL;

  // Test Case 8: restrict then set
  r_dict = restricted_dictionary_new(10);
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy",
                                        "company=Google") == 0);
  assert(restricted_dictionary_set(r_dict, "company", "Google") == 0);
  assert(restricted_dictionary_set(r_dict, "employee", "Andy") == -1);
  restricted_dictionary_del(r_dict);
}

void test_multiRestrict() {

  char *master_pairs[] = {"company=Google", "location=USA"};
  char *invalid_master_pairs[] = {"company=Google", "invalid-master-pair"};
  char *single_master_pair[] = {"company=Google"};
  char *empty_master_pairs[] = {"", ""};
  struct restricted_dictionary *r_dict = NULL;
  int result = 0;

  // Test Case 1: Add multiple restrictions with a NULL slave pair
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, NULL, master_pairs, 2);
  assert(result == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 2: Add multiple restrictions with NULL master pairs
  r_dict = restricted_dictionary_new(10);
  result =
      restricted_dictionary_multiRestrict(r_dict, "employee=Andy", NULL, 2);
  assert(result == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 3: Add multiple restrictions with invalid slave pair format
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "invalid-slave-pair",
                                               master_pairs, 2);
  assert(result == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 4: Add multiple restrictions with zero num_masters
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "employee=Andy",
                                               master_pairs, 0);
  assert(result == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 5: Add multiple restrictions with empty master pairs
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "employee=Andy",
                                               empty_master_pairs, 2);
  assert(result == -1);
  restricted_dictionary_del(r_dict);

  // Test Case 6: Add a restriction with one valid master pair
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "employee=Andy",
                                               single_master_pair, 1);
  assert(result == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 7: Add multiple restrictions with invalid master pair format
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "employee=Andy",
                                               invalid_master_pairs, 2);
  assert(result == -1); // Should continue even though one master pair is
                       // invalid, check continuation behavior
  restricted_dictionary_del(r_dict);

  // Test Case 8: Add multiple valid restrictions
  r_dict = restricted_dictionary_new(10);
  result = restricted_dictionary_multiRestrict(r_dict, "employee=Andy",
                                               master_pairs, 2);
  assert(result == 0);
  restricted_dictionary_del(r_dict);
}

int main() {
  test_new();
  test_del();
  test_set();
  test_restrict();
  test_unrestrict();
  test_restrict_then_set();
  test_multiRestrict();
  printf("All test cases passed!\n");

  return 0;
}

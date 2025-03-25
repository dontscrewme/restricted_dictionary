#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "restricted_dictionary.h"

void test_new(void)
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

void test_del(void)
{
  // Test Case 1: Delete a NULL restricted dictionary
  restricted_dictionary_del(NULL);
}

void test_set(void)
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

void test_restrict(void)
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
  r_dict = restricted_dictionary_new(d); // Create the restricted dictionary
  assert(restricted_dictionary_restrict(r_dict, "employee=Andy", "company=Google") == 0);
  assert(restricted_dictionary_unrestrict_all(r_dict, "employee=Andy") == 0);
  restricted_dictionary_del(r_dict);

  // Test Case 13: No Restrictions
  r_dict = restricted_dictionary_new(d);
  char **restrictions = NULL;
  unsigned int count = 0;
  int ret = restricted_dictionary_get_restrictions(r_dict, "role=admin", &restrictions, &count);
  assert(ret == 0);
  assert(count == 0);
  assert(restrictions == NULL);
  restricted_dictionary_del(r_dict);

  // Test Case 14: Single Restriction
  r_dict = restricted_dictionary_new(d);
  restricted_dictionary_restrict(r_dict, "role=admin", "access=all");
  restrictions = NULL;
  count = 0;
  ret = restricted_dictionary_get_restrictions(r_dict, "role=admin", &restrictions, &count);
  assert(ret == 0);
  assert(count == 1);
  assert(strcmp(restrictions[0], "access=all") == 0);
  for (unsigned i = 0; i < count; i++)
  {
    free(restrictions[i]);
  }
  free(restrictions);
  restricted_dictionary_del(r_dict);

  // Test Case 15: Multiple Restrictions
  r_dict = restricted_dictionary_new(d);
  restricted_dictionary_restrict(r_dict, "role=admin", "access=all");
  restricted_dictionary_restrict(r_dict, "role=admin", "security=high");
  restricted_dictionary_restrict(r_dict, "role=admin", "audit=true");
  restrictions = NULL;
  count = 0;
  ret = restricted_dictionary_get_restrictions(r_dict, "role=admin", &restrictions, &count);
  assert(ret == 0);
  assert(count == 3);

  int found[3] = {0};
  for (unsigned i = 0; i < count; i++)
  {
    if (strcmp(restrictions[i], "access=all") == 0)
      found[0]++;
    if (strcmp(restrictions[i], "security=high") == 0)
      found[1]++;
    if (strcmp(restrictions[i], "audit=true") == 0)
      found[2]++;
  }
  assert(found[0] == 1 && found[1] == 1 && found[2] == 1);

  for (unsigned i = 0; i < count; i++)
  {
    free(restrictions[i]);
  }
  free(restrictions);
  restricted_dictionary_del(r_dict);

  // Test Case 16: Invalid inputs
  r_dict = restricted_dictionary_new(d);
  restrictions = NULL;
  count = 0;
  assert(restricted_dictionary_get_restrictions(NULL, "A=B", &restrictions, &count) == -1);
  assert(restricted_dictionary_get_restrictions(r_dict, "invalid", &restrictions, &count) == -1);
  assert(restricted_dictionary_get_restrictions(r_dict, "A=B", NULL, &count) == -1);
  assert(restricted_dictionary_get_restrictions(r_dict, "A=B", &restrictions, NULL) == -1);
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
  
  struct dictionary *d = dictionary_new(10);
  struct restricted_dictionary *r_dict = restricted_dictionary_new(d);

  // startup code: set restrictions
  restricted_dictionary_restrict(r_dict, "ERPS=ON", "QoS=ON");
  restricted_dictionary_restrict(r_dict, "ERPS=ON", "VLAN=ON");
  restricted_dictionary_restrict(r_dict, "ERPS=ON", "SNTP=ON");

  // startup config: set initial values
  restricted_dictionary_set(r_dict, "ERPS", "OFF");
  restricted_dictionary_set(r_dict, "QoS", "OFF");
  restricted_dictionary_set(r_dict, "VLAN", "OFF");
  restricted_dictionary_set(r_dict, "SNTP", "OFF");

  // some application code
  restricted_dictionary_set(r_dict, "QoS", "ON");
  if (restricted_dictionary_set(r_dict, "ERPS", "ON") == -1)
  {
    unsigned count = 0;
    char** restrictions = NULL;
    restricted_dictionary_get_restrictions(r_dict, "ERPS=ON", &restrictions, &count);
    printf("Found %u restrictions for 'ERPS=ON':\n", count);
    for (unsigned i = 0; i < count; i++) {
        printf("- %s\n", restrictions[i]);
    }
    
    for (unsigned int i = 0; i < count; i++) {
        free(restrictions[i]);
    }
    free(restrictions);
  }

  printf("\ndumping:\n");
  dictionary_dump(d, stdout);

  dictionary_del(d);
  restricted_dictionary_del(r_dict);

  return 0;
}

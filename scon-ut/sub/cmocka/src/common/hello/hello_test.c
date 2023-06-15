#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdbool.h>
#include <cmocka.h>
#include "hello.h"

int setup(void ** state)
{

  return 0;
}

int teardown(void** state)
{
 //   free(*state);

   return 0;
}

static void test_hello(void** state)
{
  (void)state;
  int ret= hello(1,2);
  assert_int_equal(ret,3);
}


int main()
{
    const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_hello),
    };

    //cmocka_set_message_output(CM_OUTPUT_XML);

    return cmocka_run_group_tests(tests, setup, teardown);
}



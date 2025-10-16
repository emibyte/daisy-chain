#include "test.h"
#include "chain.h"
#include "munit/munit.h"
#include "task.h"
#include <string.h>

int main(int argc, char **argv) {
  // TODO: move this into the header file
  static MunitTest tests[] = {
      {"new_task", test_new_task, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"edit_task", test_edit_task, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"task_repr", test_task_repr, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"new_task_node", test_new_task_node, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {"new_chain", test_new_chain, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"add_task", test_add_task, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"delete_task", test_delete_task, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {"find_task", test_find_task, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},

      // Terminator of array
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  };

  // TODO: move this into the header file
  static const MunitSuite suite = {
      .prefix = "daisy-chain tests: ",
      .tests = tests,
      .suites = NULL,
      .iterations = 1,
      .options = MUNIT_SUITE_OPTION_NONE,
  };

  munit_suite_main(&suite, NULL, argc, argv);

  return 0;
}

MunitResult test_new_task(const MunitParameter params[],
                          void *user_data_or_fixture) {
  int seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  task_t *task = new_task(4, "clean up my desk", current_time + seconds_day,
                          MEDIUM, NULL, 0);

  munit_assert_ptr_not_null(task);
  munit_assert_int(task->id, ==, 4);
  munit_assert_string_equal(task->description, "clean up my desk");
  munit_assert_int(task->created, >, 0);
  munit_assert_int(task->due_date, ==, current_time + seconds_day);
  munit_assert_int(task->priority, ==, MEDIUM);
  munit_assert_null(task->tags);
  munit_assert_int(task->count_tags, ==, 0);

  free_task(task);

  return MUNIT_OK;
};

MunitResult test_edit_task(const MunitParameter params[],
                           void *user_data_or_fixture) {
  long seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  time_t initial_due_date = current_time + seconds_day;
  task_t *task =
      new_task(4, "clean up my desk", initial_due_date, MEDIUM, NULL, 0);
  edit_task(task, "this is a changed description",
            task->due_date + 10 * seconds_day, HIGH, NULL);

  munit_assert_ptr_not_null(task);
  munit_assert_string_not_equal(task->description, "clean up my desk");
  munit_assert_string_equal(task->description, "this is a changed description");
  munit_assert_int(task->due_date, >, initial_due_date);
  munit_assert_int(task->due_date, ==, initial_due_date + 10 * seconds_day);
  munit_assert_int(task->priority, ==, HIGH);

  free_task(task);

  return MUNIT_OK;
};

MunitResult test_task_repr(const MunitParameter params[],
                           void *user_data_or_fixture) {
  long seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  task_t *task = new_task(4, "clean up my desk", current_time + seconds_day,
                          MEDIUM, NULL, 0);

  char *expected = "4. - clean up my desk - [ ]";
  char *actual = task_repr(task);

  munit_assert_string_equal(expected, actual);

  free_task(task);

  return MUNIT_OK;
}

MunitResult test_new_task_node(const MunitParameter params[],
                               void *user_data_or_fixture) {
  long seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  task_t *task = new_task(4, "clean up my desk", current_time + seconds_day,
                          MEDIUM, NULL, 0);
  task_node_t *node = new_task_node(NULL, task);

  munit_assert_ptr_not_null(node->val);
  munit_assert_ptr_null(node->next);

  free_task_node(node);

  return MUNIT_OK;
}

MunitResult test_new_chain(const MunitParameter params[],
                           void *user_data_or_fixture) {
  task_chain_t *chain = new_chain();

  munit_assert_int(chain->size, ==, 0);
  munit_assert_ptr_null(chain->head);

  free_chain(chain);

  return MUNIT_OK;
}

MunitResult test_add_task(const MunitParameter params[],
                          void *user_data_or_fixture) {
  time_t current_time = time(NULL);
  task_chain_t *chain = new_chain();
  task_t *task_a = new_task(1, "task 1", current_time, HIGH, NULL, 0);
  task_t *task_b = new_task(2, "task 2", current_time, MEDIUM, NULL, 0);
  task_t *task_c = new_task(3, "task 3", current_time, LOW, NULL, 0);

  add_task(chain, task_a);
  munit_assert_int(chain->size, ==, 1);

  add_task(chain, task_b);
  munit_assert_int(chain->size, ==, 2);

  add_task(chain, task_c);
  munit_assert_int(chain->size, ==, 3);

  munit_assert_ptr_not_null(chain->head);
  munit_assert_ptr_not_null(chain->head->val);
  munit_assert_ptr_not_null(chain->head->next);

  task_node_t *cur = chain->head;
  munit_assert_int(cur->val->id, ==, 1);

  cur = cur->next;
  munit_assert_ptr_not_null(cur->val);
  munit_assert_int(cur->val->id, ==, 2);
  munit_assert_ptr_not_null(cur->next);

  cur = cur->next;
  munit_assert_int(cur->val->id, ==, 3);
  munit_assert_ptr_null(cur->next);

  free_chain(chain);

  return MUNIT_OK;
}

MunitResult test_delete_task(const MunitParameter params[],
                             void *user_data_or_fixture) {
  time_t current_time = time(NULL);
  task_chain_t *chain = new_chain();
  task_t *task_a = new_task(1, "task 1", current_time, HIGH, NULL, 0);
  task_t *task_b = new_task(2, "task 2", current_time, MEDIUM, NULL, 0);
  task_t *task_c = new_task(3, "task 3", current_time, LOW, NULL, 0);

  add_task(chain, task_a);
  add_task(chain, task_b);
  add_task(chain, task_c);

  delete_task(chain, 2);

  munit_assert_int(chain->size, ==, 2);
  munit_assert_ptr_not_null(chain->head->next);
  munit_assert_int(chain->head->next->val->id, ==, 3);

  free_chain(chain);

  return MUNIT_OK;
}

MunitResult test_find_task(const MunitParameter params[], void
                           *user_data_or_fixture) {
  time_t current_time = time(NULL);
  task_chain_t *chain = new_chain();
  task_t *task_a = new_task(1, "task 1", current_time, HIGH, NULL, 0);
  task_t *task_b = new_task(2, "task 2", current_time, MEDIUM, NULL, 0);
  task_t *task_c = new_task(3, "task 3", current_time, LOW, NULL, 0);

  add_task(chain, task_a);
  add_task(chain, task_b);
  add_task(chain, task_c);

  task_t *task_existing = find_task(chain, 3);
  task_t *task_non_existing = find_task(chain, 4);

  munit_assert_ptr_not_null(task_existing);
  munit_assert_ptr_null(task_non_existing);

  return MUNIT_OK;
}

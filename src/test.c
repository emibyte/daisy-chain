#include "test.h"
#include "chain.h"
#include "json-c/json_object.h"
#include "json-c/json_tokener.h"
#include <stdio.h>
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
      {"find_task", test_find_task, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"serialize_task", test_serialize_task, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"deserialize_task", test_deserialize_task, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"serialize_chain", test_serialize_chain, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"deserialize_chain", test_deserialize_chain, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},

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

MunitResult test_find_task(const MunitParameter params[],
                           void *user_data_or_fixture) {
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

MunitResult test_serialize_task(const MunitParameter params[],
                                void *user_data_or_fixture) {
  task_t *task = new_task(4, "clean up my desk", 0, MEDIUM, NULL, 0);
  json_object *json = to_json_task(task);
  const char *json_string = json_object_to_json_string(json);

  char expected[256];
  sprintf(expected,
          "{ \"id\": 4, \"description\": \"clean up my desk\", \"completed\": "
          "false, \"created\": %ld, \"due_date\": 0, \"priority\": 1 }",
          task->created);

  munit_assert_string_equal(json_string, expected);

  return MUNIT_OK;
}

MunitResult test_deserialize_task(const MunitParameter params[],
                                  void *user_data_or_fixture) {

  const char *json_string =
      "{ \"id\": 4, \"description\": \"clean up my desk\", \"completed\": "
      "true, \"created\": 12345, \"due_date\": 65432, \"priority\": 1 }";
  json_object *json_obj = json_tokener_parse(json_string);
  munit_assert_ptr_not_null(json_obj);

  task_t *task = from_json_task(json_obj);
  munit_assert_ptr_not_null(task);
  munit_assert_int(task->id, ==, 4);
  munit_assert_string_equal(task->description, "clean up my desk");
  munit_assert(task->completed);
  munit_assert_int64(task->created, ==, 12345);
  munit_assert_int64(task->due_date, ==, 65432);
  munit_assert_int(task->priority, ==, MEDIUM);

  return MUNIT_OK;
}

MunitResult test_serialize_chain(const MunitParameter params[],
                                 void *user_data_or_fixture) {
  char expected_a[128];
  char expected_b[128];
  char expected_c[128];
  char expected_chain[512];
  task_chain_t *chain = new_chain();
  task_t *task_a = new_task(1, "task 1", 1, HIGH, NULL, 0);
  task_t *task_b = new_task(2, "task 2", 321, LOW, NULL, 0);
  task_t *task_c = new_task(3, "task 3", 69213, MEDIUM, NULL, 0);

  task_b->completed = true;

  add_task(chain, task_a);
  add_task(chain, task_b);
  add_task(chain, task_c);

  json_object *json_chain = to_json_chain(chain);
  sprintf(expected_a,
          "{ \"id\": 1, \"description\": \"task 1\", \"completed\": "
          "false, \"created\": %ld, \"due_date\": 1, \"priority\": 2 }",
          task_a->created);
  sprintf(expected_b,
          "{ \"id\": 2, \"description\": \"task 2\", \"completed\": "
          "true, \"created\": %ld, \"due_date\": 321, \"priority\": 0 }",
          task_a->created);
  sprintf(expected_c,
          "{ \"id\": 3, \"description\": \"task 3\", \"completed\": "
          "false, \"created\": %ld, \"due_date\": 69213, \"priority\": 1 }",
          task_a->created);

  sprintf(expected_chain, "[ %s, %s, %s ]", expected_a, expected_b, expected_c);

  munit_assert_string_equal(expected_chain,
                            json_object_to_json_string(json_chain));

  return MUNIT_OK;
}

MunitResult test_deserialize_chain(const MunitParameter params[],
                                   void *user_data_or_fixture) {
  const char *json_chain_string =
      "[ { \"id\": 1, \"description\": \"task 1\", \"completed\": "
      "false, \"created\": 321, \"due_date\": 555, \"priority\": 2 }, { "
      "\"id\": 2, \"description\": \"task 2\", \"completed\": "
      "true, \"created\": 1345, \"due_date\": 22, \"priority\": 0 }, { \"id\": "
      "3, \"description\": \"task 3\", \"completed\": "
      "false, \"created\": 696, \"due_date\": 111, \"priority\": 1 } ]";

  json_object *json_chain = json_tokener_parse(json_chain_string);
  munit_assert_ptr_not_null(json_chain);

  task_chain_t *chain = from_json_chain(json_chain);
  munit_assert_ptr_not_null(chain);

  munit_assert_ptr_not_null(chain->head);
  munit_assert_int(chain->size, ==, 3);
  munit_assert_int(chain->head->val->id, ==, 1);
  munit_assert_int(chain->head->val->created, ==, 321);
  munit_assert_int(chain->head->val->due_date, ==, 555);
  munit_assert_int(chain->head->val->priority, ==, 2);
  munit_assert_string_equal(chain->head->val->description, "task 1");

  munit_assert_ptr_not_null(chain->head->next);
  task_node_t *second_node = chain->head->next;
  munit_assert_int(second_node->val->id, ==, 2);
  munit_assert_int(second_node->val->created, ==, 1345);
  munit_assert_int(second_node->val->due_date, ==, 22);
  munit_assert_int(second_node->val->priority, ==, 0);
  munit_assert_string_equal(second_node->val->description, "task 2");

  munit_assert_ptr_not_null(second_node->next);
  task_node_t *third_node = second_node->next;
  munit_assert_int(third_node->val->id, ==, 3);
  munit_assert_int(third_node->val->created, ==, 696);
  munit_assert_int(third_node->val->due_date, ==, 111);
  munit_assert_int(third_node->val->priority, ==, 1);
  munit_assert_string_equal(third_node->val->description, "task 3");
  
  return MUNIT_OK;
}

#include "munit/munit.h"
#include "task.h"

int main(void) {
  return 0;
}

MunitResult test_new_task(const MunitParameter params[], void *user_data_or_fixture) {
  int seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  task_t *task = new_task(4, "clean up my desk", current_time + seconds_day, MEDIUM, NULL, 0);

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

MunitResult test_edit_task(const MunitParameter params[], void *user_data_or_fixture) {
  int seconds_day = 60 * 60 * 24;
  time_t current_time = time(NULL);
  time_t initial_due_date = current_time + seconds_day;
  task_t *task = new_task(4, "clean up my desk", initial_due_date, MEDIUM, NULL, 0);
  edit_task(task, "this is a changed description", task->due_date + 10 * seconds_day, HIGH, NULL);
  
  munit_assert_ptr_not_null(task);
  munit_assert_string_not_equal(task->description, "clean up my desk");
  munit_assert_string_equal(task->description, "this is a changed description");
  munit_assert_int(task->due_date, >, initial_due_date);
  munit_assert_int(task->due_date, ==, initial_due_date + 10 * seconds_day);
  munit_assert_int(task->priority, ==, HIGH);

  free_task(task);

  return MUNIT_OK;
};

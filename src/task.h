#ifndef TASK_H
#define TASK_H

#include <json-c/arraylist.h>
#include <json-c/json_object.h>
#include <json-c/json_types.h>
#include <json-c/linkhash.h>
#include <stdbool.h>
#include <time.h>

typedef enum Priority {
  LOW,
  MEDIUM,
  HIGH,
} task_priority;

typedef struct Task {
  char *description;
  int id;
  bool completed;
  time_t created;
  time_t due_date;
  task_priority priority;
  // TODO: this should probably a list that stores size (we are not implementing
  // tags yet)
  char **tags;
  int count_tags;
} task_t;

task_t *new_task(int id, char *description, time_t due_date,
                 task_priority priority, char **tags, int count_tags);
void edit_task(task_t *task, char *new_description, time_t new_due_date,
               task_priority new_priority, char **new_tags);
void free_task(task_t *task);
char *task_repr(task_t *task);

json_object *to_json_task(task_t *task);
task_t *from_json_task(json_object *json);

void handle_int_vals(const char *key, json_object *json_val, task_t *task);

#endif

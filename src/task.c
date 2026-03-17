#include "task.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// prototype for static function (not in header since that makes no sense)
static void
handle_int_vals(const char *key, json_object *json_val, task_t *task);

const char *priority_names[] = {"Low", "Medium", "High"};

task_t *
new_task(int id, char *description, time_t due_date, task_priority priority,
         char **tags, int count_tags) {
  task_t *task = malloc(sizeof(task_t));
  if (task == NULL) {
    return NULL;
  }
  task->id = id;
  task->description = description == NULL ? NULL : strdup(description);
  task->completed = false;
  task->created = time(NULL);
  task->due_date = due_date;
  task->priority = priority;
  task->tags = tags;
  task->count_tags = count_tags;

  return task;
}

void
edit_task(task_t *task, char *new_description, time_t new_due_date,
          task_priority new_priority, char **new_tags) {
  task->description = strdup(new_description);
  task->due_date = new_due_date;
  task->priority = new_priority;

  // NOTE: this totally doesnt work, need sth better
  task->tags = new_tags;
  return;
}

void
free_task(task_t *task) {
  if (task == NULL) {
    return;
  }

  free(task->description);

  if (task->tags != NULL) {
    for (int i = 0; i < task->count_tags; i++) {
      free(task->tags[i]);
    }
    free(task->tags);
  }

  free(task);
}

char *
task_repr(task_t *task, int cur_max_id) {
  int max_size_priority = 6; // "Medium"
  int size_of_priority = strlen(priority_names[task->priority]);
  int align_whitespace_after_prio = max_size_priority - size_of_priority;
  char whitespace_alignment_prio[align_whitespace_after_prio + 1];
  strnconcat(' ', align_whitespace_after_prio, whitespace_alignment_prio,
             align_whitespace_after_prio + 1);

  int max_size_id = get_digit_count(cur_max_id);
  int size_of_id = get_digit_count(task->id);
  int align_whitespace_after_id = max_size_id - size_of_id;
  char whitespace_alignment_id[align_whitespace_after_id + 1];
  strnconcat(' ', align_whitespace_after_id, whitespace_alignment_id,
             align_whitespace_after_id + 1);

  int size_to_allocate = strlen(task->description) + size_of_priority + 80;
  char *display_buffer = malloc(size_to_allocate * sizeof(char));
  if (display_buffer == NULL) {
    return NULL;
  }

  char time_buf[64];
  struct tm *tm_info = localtime(&task->due_date);
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d", tm_info);

  char completed = task->completed ? 'x' : ' ';
  sprintf(display_buffer, "ID: %d%s, Priority: %s%s - Status: [%c] - Deadline: %s - %s",
          task->id, whitespace_alignment_id, priority_names[task->priority],
          whitespace_alignment_prio, completed, time_buf, task->description);
  return display_buffer;
}

json_object *
to_json_task(task_t *task) {
  json_object *json_obj = json_object_new_object();

  json_object_object_add(json_obj, "id", json_object_new_int(task->id));
  json_object_object_add(json_obj, "description",
                         json_object_new_string(task->description));
  json_object_object_add(json_obj, "completed",
                         json_object_new_boolean(task->completed));
  json_object_object_add(json_obj, "created",
                         json_object_new_int64(task->created));
  json_object_object_add(json_obj, "due_date",
                         json_object_new_int64(task->due_date));
  json_object_object_add(json_obj, "priority",
                         json_object_new_int(task->priority));

  return json_obj;
}

task_t *
from_json_task(json_object *json_obj) {
  task_t *task = new_task(0, NULL, 0, 0, NULL, 0);
  if (task == NULL)
    return NULL;

  json_object_object_foreach(json_obj, key, val) {
    switch (json_object_get_type(val)) {
    case json_type_int:
      handle_int_vals(key, val, task);
      break;
    case json_type_string:
      task->description = strdup(json_object_get_string(val));
      break;
    case json_type_boolean:
      task->completed = json_object_get_boolean(val);
      break;
    default:
      break;
    }
  }

  return task;
}

static void
handle_int_vals(const char *key, json_object *json_val, task_t *task) {
  if (strcmp(key, "id") == 0) {
    task->id = json_object_get_int(json_val);
  } else if (strcmp(key, "created") == 0) {
    task->created = json_object_get_int64(json_val);
  } else if (strcmp(key, "due_date") == 0) {
    task->due_date = json_object_get_int64(json_val);
  } else if (strcmp(key, "priority") == 0) {
    task->priority = json_object_get_int(json_val);
  }
}

#include "task.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

task_t *new_task(int id, char *description, time_t due_date, task_priority priority,
                 char **tags) {
  task_t *task = malloc(sizeof(task_t));
  if (task == NULL) {
    return NULL;
  }
  task->id = id;
  task->description = strdup(description);
  task->completed = false;
  task->created = time(NULL);
  task->due_date = due_date;
  task->priority = priority;
  task->tags = tags;

  return task;
}

task_t *edit_task(task_t *task, char *new_description, time_t new_due_date,
                  task_priority new_priority, char **new_tags) {
  return NULL;
}

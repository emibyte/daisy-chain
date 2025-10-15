#include "task.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

task_t *new_task(int id, char *description, time_t due_date,
                 task_priority priority, char **tags, int count_tags) {
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
  task->count_tags = count_tags;

  return task;
}

void edit_task(task_t *task, char *new_description, time_t new_due_date,
               task_priority new_priority, char **new_tags) {
  task->description = strdup(new_description);
  task->due_date = new_due_date;
  task->priority = new_priority;

  // NOTE: this totally doesnt work, need sth better
  task->tags = new_tags;
  return;
}

void free_task(task_t *task) {
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

char *task_repr(task_t *task) {
  int size_to_allocate = strlen(task->description) + 20;
  char *display_buffer = malloc(size_to_allocate * sizeof(char));
  if (display_buffer == NULL) {
    return NULL;
  }
  char completed = task->completed ? 'x' : ' ';
  sprintf(display_buffer, "%d. - %s - [%c]", task->id, task->description,
          completed);
  return display_buffer;
}

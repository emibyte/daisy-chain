#ifndef TASK_H
#define TASK_H

#include <time.h>
#include <stdbool.h>

typedef enum Priority {
  LOW,
  MEDIUM,
  HIGH,
} task_priority;

// TODO: add a list of tags to the task (how do i wanna represent the tags)
typedef struct Task {
  char *description;
  int id;
  bool completed;
  time_t created;
  time_t due_date;
  char **tags; // ?
} task_t;

task_t *new_task(int id, char *description);
task_t *edit_task(task_t *task, char *new_description);

#endif

#ifndef TASK_H
#define TASK_H

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
  // TODO: this should probably a list that stores size
  char **tags;
  int count_tags;
} task_t;

task_t *new_task(int id, char *description, time_t due_date, task_priority priority,
                 char **tags, int count_tags);
task_t *edit_task(task_t *task, char *new_description, time_t new_due_date,
                  task_priority new_priority, char **new_tags);
void free_task(task_t *task);
char *task_repr(task_t *task);

#endif

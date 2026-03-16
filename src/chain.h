#ifndef CHAIN_H
#define CHAIN_H

#include "cmd_parser.h"
#include "task.h"

typedef struct Option cmd_option_t;

typedef struct TaskNode task_node_t;

typedef struct TaskNode {
  task_node_t *next;
  task_t *val;
} task_node_t;

typedef struct TaskChain {
  int size;
  int next_id;
  task_node_t *head;
} task_chain_t;

task_node_t *
new_task_node(task_node_t *next, task_t *val);
void
free_task_node(task_node_t *node);

task_chain_t *
new_chain();
void
free_chain(task_chain_t *chain);
void
add_task(task_chain_t *chain, task_t *task);
bool
delete_task(task_chain_t *chain, int id);
task_t *
find_task(task_chain_t *chain, int id);
char *
chain_repr(task_chain_t *chain);
char *
chain_repr_filtered(task_chain_t *chain, cmd_option_t option);

json_object *
to_json_chain(task_chain_t *chain);
task_chain_t *
from_json_chain(json_object *json_chain);
task_chain_t *
load_task_chain(char *filepath);
bool
save_task_chain(char *filepath, task_chain_t *chain);

#endif

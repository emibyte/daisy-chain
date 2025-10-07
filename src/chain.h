#ifndef CHAIN_H
#define CHAIN_H

#include "task.h"

typedef struct TaskNode task_node_t;

typedef struct TaskNode {
  task_node_t *next;
  task_t *val;
} task_node_t;

typedef struct TaskChain {
  int size; // equals max id
  task_node_t *head;
} task_chain_t;

task_node_t *new_task_node(task_node_t *next, task_t *val);
void free_task_node(task_node_t *next, task_t *val);

task_chain_t *new_chain();
void free_chain(task_chain_t *chain);
void add_task(task_chain_t *chain, task_t *task);
void delete_task(task_chain_t *chain, int id);
task_t *find_task(int id);
#endif

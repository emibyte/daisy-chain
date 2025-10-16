#include "chain.h"
#include "task.h"
#include <stdlib.h>

task_node_t *new_task_node(task_node_t *next, task_t *val) {
  task_node_t *node = malloc(sizeof(task_node_t));
  if (node == NULL) {
    return NULL;
  }
  node->val = val;
  node->next = next;
  return node;
}

void free_task_node(task_node_t *node) {
  free_task(node->val);
  // NOTE: we dont free next since it is still a relevant node
  //       in the list, so we leave it be and just null the value at that point
  node->next = NULL;
  free(node);
}

task_chain_t *new_chain() {
  task_chain_t *chain = malloc(sizeof(task_chain_t));
  if (chain == NULL) {
    return NULL;
  }
  chain->size = 0;
  chain->head = NULL;
  return chain;
}

void add_task(task_chain_t *chain, task_t *task) {
  if (chain == NULL || task == NULL) {
    return;
  }

  task_node_t *node = new_task_node(NULL, task);
  if (node == NULL) {
    return;
  }

  // NOTE: just have a tail in the chain to append to instead of walking the
  // list
  if (chain->head == NULL) {
    chain->head = node;
  } else {
    task_node_t *cur = chain->head;
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = node;
  }

  chain->size++;
}

bool delete_task(task_chain_t *chain, int id) {
  if (chain == NULL || chain->head == NULL || id < 1) {
    return false;
  }

  task_node_t *prev = NULL;
  task_node_t *cur = chain->head;
  while (cur != NULL) {
    if (cur->val->id == id) {
      // found it, delete it
      prev->next = cur->next;
      free_task_node(cur);
      chain->size--;
      return true;
    }
    prev = cur;
    cur = cur->next;
  }

  return false;
}

task_t *find_task(task_chain_t *chain, int id) {
  if (chain == NULL || chain->head == NULL || id == 0) {
    return NULL;
  }
  task_node_t *cur = chain->head;
  while (cur == NULL) {
    if (cur->val->id == id) {
      return cur->val;
    }
  }
  return NULL;
}

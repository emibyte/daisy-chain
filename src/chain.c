#include "chain.h"
#include "cmd_parser.h"
#include "files.h"
#include "json-c/json_object.h"
#include "json-c/json_tokener.h"
#include "task.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

task_node_t *
new_task_node(task_node_t *next, task_t *val) {
  task_node_t *node = malloc(sizeof(task_node_t));
  if (node == NULL) {
    return NULL;
  }
  node->val = val;
  node->next = next;
  return node;
}

void
free_task_node(task_node_t *node) {
  free_task(node->val);
  // NOTE: we dont free next since it is still a relevant node
  //       in the list, so we leave it be and just null the value at that point
  node->next = NULL;
  free(node);
}

task_chain_t *
new_chain() {
  task_chain_t *chain = malloc(sizeof(task_chain_t));
  if (chain == NULL) {
    return NULL;
  }
  chain->size = 0;
  chain->next_id = 1;
  chain->head = NULL;
  return chain;
}

void
add_task(task_chain_t *chain, task_t *task) {
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
  chain->next_id = task->id + 1;
}

bool
delete_task(task_chain_t *chain, int id) {
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

task_t *
find_task(task_chain_t *chain, int id) {
  if (chain == NULL || chain->head == NULL || id == 0) {
    return NULL;
  }
  task_node_t *cur = chain->head;
  while (cur != NULL) {
    if (cur->val->id == id) {
      return cur->val;
    }
    cur = cur->next;
  }
  return NULL;
}

void
free_chain(task_chain_t *chain) {
  task_node_t *cur = chain->head;
  while (cur != NULL) {
    task_node_t *tmp = cur->next;
    free_task_node(cur);
    cur = tmp;
  }
  free(chain);
}

// TODO: aaaaaaaaaaaaaaaaa
//       this is like a whole mess, since task_repr and chain_repr both allocate
//       seperately, i dont like it but for now it works since im not sure how
//       to make it so u just pass in a single buffer into chain repr since u
//       would need to know the size of the tasks beforehand
char *
chain_repr(task_chain_t *chain) {
  if (chain == NULL || chain->size == 0) {
    char *empty_chain = malloc(1);
    empty_chain[0] = '\0';
    return empty_chain;
  }

  size_t size_to_allocate =
      chain->size * 2; // some extra space for newline and maybe extra stuff
  char *task_repr_array[chain->size];
  int idx = 0;
  for (task_node_t *cur = chain->head; cur != NULL; cur = cur->next) {
    char *repr = task_repr(cur->val, chain->next_id - 1);
    size_to_allocate += strlen(repr);
    task_repr_array[idx] = repr;
    idx++;
  }

  char *buffer = malloc(size_to_allocate);
  buffer[0] = '\0'; // need to null out the buffer bcs strcat looks for a null terminator to start appending to!!!
  for (int i = 0; i < idx; i++) {
    strcat(buffer, task_repr_array[i]);
    strcat(buffer, "\n");
  }

  for (int i = 0; i < chain->size; i++) {
    free(task_repr_array[i]);
  }

  return buffer;
}

char *
chain_repr_filtered(task_chain_t *chain, cmd_option_t option) {
  if (chain == NULL || chain->size == 0) {
    char *empty_chain = malloc(1);
    empty_chain[0] = '\0';
    return empty_chain;
  }

  int capacity = chain->size;
  int size = 0;
  task_node_t *filtered_nodes[capacity];
  for (task_node_t *cur = chain->head; cur != NULL; cur = cur->next) {
    if (option.filter.filter_kind == FILTER_PRIORITY) {
      if (cur->val->priority == option.filter.prio) {
        filtered_nodes[size] = cur;
        size++;
      }
    } else if (option.filter.filter_kind == FILTER_DESCRIPTION) {
      bool description_matches =
          strcmp(cur->val->description, option.filter.description) == 0;
      if (!description_matches) {
        description_matches =
            strstr(cur->val->description, option.filter.description) == NULL
                ? false
                : true;
      }
      if (description_matches) {
        filtered_nodes[size] = cur;
        size++;
      }
    }
  }

  size_t size_to_allocate =
      chain->size * 2; // some extra space for newline and maybe extra stuff
  char *task_repr_array[size];
  int idx = 0;
  for (int i = 0; i < size; i++) {
    char *repr = task_repr(filtered_nodes[i]->val, chain->next_id - 1);
    size_to_allocate += strlen(repr);
    task_repr_array[idx] = repr;
    idx++;
  }

  char *buffer = malloc(size_to_allocate);
  buffer[0] = '\0'; // need to null out the buffer bcs strcat looks for a null terminator to start appending to!!!
  for (int i = 0; i < idx; i++) {
    strcat(buffer, task_repr_array[i]);
    strcat(buffer, "\n");
  }

  for (int i = 0; i < size; i++) {
    free(task_repr_array[i]);
  }

  return buffer;
}

json_object *
to_json_chain(task_chain_t *chain) {
  json_object *json_chain = json_object_new_array_ext(chain->size);

  for (task_node_t *node = chain->head; node != NULL; node = node->next) {
    json_object *json_task = to_json_task(node->val);
    json_object_array_add(json_chain, json_task);
  }

  return json_chain;
}

task_chain_t *
from_json_chain(json_object *json_chain) {
  assert(json_object_get_type(json_chain) == json_type_array);

  task_chain_t *chain = new_chain();
  if (chain == NULL) {
    return NULL;
  }

  struct array_list *backing_array = json_object_get_array(json_chain);
  for (int i = 0; i < backing_array->length; i++) {
    json_object *json_task = backing_array->array[i];
    task_t *task = from_json_task(json_task);
    add_task(chain, task);
  }

  return chain;
}

task_chain_t *
load_task_chain(char *filepath) {
  char *contents = read_file(filepath);
  if (contents == NULL) {
    return new_chain();
  }

  json_object *json_chain = json_tokener_parse(contents);
  task_chain_t *chain = from_json_chain(json_chain);
  json_object_put(json_chain);

  free(contents);
  return chain;
}

bool
save_task_chain(char *filepath, task_chain_t *chain) {
  json_object *json_chain = to_json_chain(chain);
  const char *json_str_chain = json_object_to_json_string(json_chain);

  size_t length = strlen(json_str_chain);
  bool result = write_file(filepath, json_str_chain, length);
  json_object_put(json_chain);

  return result;
}

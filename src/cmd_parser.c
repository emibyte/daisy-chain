#include "cmd_parser.h"
#include "chain.h"
#include "task.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *valid_commands[] = {"list", "add", "remove", "help", "edit", "done"};
char *valid_commands_short[] = {"-l", "-a", "-r", "-h", "-e", "-d"};

bool
translate_short_cmd(const char *short_cmd_str, char *cmd_str) {
  for (int i = 0; i < VALID_COMMANDS_COUNT; i++) {
    if (strcmp(valid_commands_short[i], short_cmd_str) == 0) {
      cmd_str = valid_commands[i];
      return true;
    }
  }
  return false;
}

bool
is_cmd_valid(const char *cmd_str) {
  bool cmd_valid = false;
  for (int i = 0; i < VALID_COMMANDS_COUNT; i++) {
    if (strcmp(valid_commands[i], cmd_str) == 0) {
      cmd_valid = true;
    }
  }
  return true;
}

int
parse_property_values(cmd_t cmd, property_value_pair_array_t *props) {
  for (int i = 0; i < cmd.args_count; i++) {
    char *arg = cmd.args[i];
    bool hit_equal = false;
    int cur_idx_prop = 0;
    int cur_idx_value = 0;
    char prop_literal[256];
    char value_literal[256];
    for (char *cur = arg; *cur != '\0'; ++cur) {
      if (cur_idx_prop > 254 || cur_idx_value > 254) {
        fprintf(stderr, "Error: property name or value was too long, limit is "
                        "255 characters.\n");
        return 10;
      }

      if (*cur == '=' && hit_equal) {
        // TODO: multiple equal signs, sth is very wrong!!
        break;
      } else if (*cur == '=' && !hit_equal) {
        hit_equal = true;
        continue;
      }

      if (!hit_equal) {
        prop_literal[cur_idx_prop] = *cur;
        cur_idx_prop++;
      }

      if (*cur != '=' && hit_equal) {
        value_literal[cur_idx_value] = *cur;
        cur_idx_value++;
      }
    }

    prop_literal[cur_idx_prop] = '\0';
    value_literal[cur_idx_value] = '\0';

    if (!hit_equal || cur_idx_value == 0) {
      // NOTE: if just a single value, treat it as an id
      bool is_number = is_integer(prop_literal);
      if (is_number && cmd.args_count == 1) {
        strcpy(value_literal, prop_literal);
        prop_literal[0] = '\0';
      } else {
        return 7;
      }
    }

    property_t prop = DESCRIPTION;
    value_kind_t value = STRING;
    if (strcmp(prop_literal, "due") == 0) {
      prop = DUE_DATE;
      value = INT;
    } else if (strcmp(prop_literal, "prio") == 0) {
      prop = PRIORITY;
      value = INT;
    } else if (strcmp(prop_literal, "id") == 0) {
      prop = ID;
      value = INT;
    } else if (strlen(prop_literal) == 0) {
      // NOTE: we land here if we found only one number
      prop = ID;
      value = INT;
    }

    if (prop == ID) {
      props->pairs[i].id = strtol(value_literal, NULL, 10);
      props->id_index = i;
    } else if (prop == DESCRIPTION) {
      props->pairs[i].description = value_literal;
      props->description_index = i;
    } else if (prop == DUE_DATE) {
      props->pairs[i].due_in_days = strtol(value_literal, NULL, 10);
      props->due_date_index = i;
    } else if (prop == PRIORITY) {
      props->priority_index = i;
      if (strcmp(value_literal, "low") == 0) {
        props->pairs[i].priority = LOW;
      } else if (strcmp(value_literal, "medium") == 0) {
        props->pairs[i].priority = MEDIUM;
      } else if (strcmp(value_literal, "high") == 0) {
        props->pairs[i].priority = HIGH;
      } else {
        printf("Error: non valid value %s for priority (use low/medium/high)\n",
               value_literal);
        return 6;
      }
    }
  }
  return 0;
}

int
parse_cmd(char *cmd_str, char **argv, int argc, cmd_t *cmd) {
  int cmd_arg_count = 0;
  if (strcmp(cmd_str, "add") == 0) {
    cmd_arg_count = ADD_ARGUMENTS_COUNT;
  } else if (strcmp(cmd_str, "remove") == 0) {
    cmd_arg_count = REMOVE_ARGUMENTS_COUNT;
  } else if (strcmp(cmd_str, "edit") == 0) {
    cmd_arg_count = EDIT_ARGUMENTS_COUNT_MAX;
  } else if (strcmp(cmd_str, "help") == 0) {
    cmd_arg_count = HELP_ARGUMENTS_COUNT;
  } else if (strcmp(cmd_str, "done") == 0) {
    cmd_arg_count = DONE_ARGUMENTS_COUNT;
  }

  if (argc > cmd_arg_count + 2) {
    printf("Error: wrong number of arguments provided\n");
    // TODO: Errorcode enum, return an error value from this function!!!
    return 3;
  }

  char **args = malloc((argc - 2) * sizeof(char *));
  for (int i = 2; i < argc; i++) {
    char *arg = argv[i];
    args[i - 2] = arg;
  }
  cmd->command = cmd_str;
  cmd->args_count = cmd_arg_count;
  cmd->args = args;
  return 0;
}

void
run_list_cmd(task_chain_t *chain) {
  char *tasks = chain_repr(chain);
  if (strlen(tasks) == 0) {
    printf("Your chain is still empty, feel free to add some tasks :3\n");
  } else {
    printf("%s", tasks);
  }
  free(tasks);
}
void
run_add_cmd(task_chain_t *chain, property_value_pair_array_t *props) {
  task_t *task = new_task(
      chain->next_id, props->pairs[props->description_index].description,
      props->pairs[props->due_date_index].due_in_days,
      props->pairs[props->priority_index].priority, NULL, 0);
  if (task == NULL) {
    fprintf(stderr, "Could not allocate task\n");
    return;
  }
  add_task(chain, task);
}

int
run_delete_cmd(task_chain_t *chain, property_value_pair_array_t *props) {
  int id_to_remove = props->pairs[props->id_index].id;
  if (id_to_remove == 0) {
    fprintf(stderr, "Error: the supplied id to remove is not an integer\n");
    return 4;
  }
  bool success = delete_task(chain, id_to_remove);
  if (!success) {
    fprintf(stderr, "Error: the supplied id does not exist in the chain\n");
    return 5;
  }
  return 0;
}

void
run_edit_cmd(task_chain_t *chain, property_value_pair_array_t *props) {
  int id_to_edit = props->pairs[props->id_index].id;
  task_t *task = find_task(chain, id_to_edit);
  for (int i = 0; i < props->length; i++) {
    property_value_pair_t prop = props->pairs[i];
    switch (prop.prop) {
    case ID:
      break;
    case DESCRIPTION:
      free(task->description);
      task->description = NULL;
      task->description = strdup(prop.description);
      break;
    case DUE_DATE:
      task->due_date = prop.due_in_days;
      break;
    case PRIORITY:
      task->priority = prop.priority;
      break;
    }
  }
}

void
run_help_cmd() {
  // TODO: implement
  printf("lowkey haven't written the text.\n");
}

int
run_done_cmd(task_chain_t *chain, property_value_pair_array_t *props) {
  int id_to_mark_as_done = props->pairs[props->id_index].id;
  if (id_to_mark_as_done == 0) {
    fprintf(stderr, "Error: the supplied id to remove is not an integer\n");
    return 4;
  }
  task_t *task = find_task(chain, id_to_mark_as_done);
  if (task == NULL) {
    fprintf(stderr, "Error: task with given id does not exist\n");
    return 5;
  }
  if (!task->completed) {
    task->completed = true;
  } else {
    fprintf(stderr, "Error: task with given id is already completed\n");
  }
  return 0;
}

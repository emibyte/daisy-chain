#include "cmd_parser.h"
#include "chain.h"
#include "task.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *valid_commands[] = {"list", "add",  "remove", "help",
                          "edit", "done", "undone"};
char *valid_commands_short[] = {"l", "a", "r", "h", "e", "d", "u"};
char *priority_names_for_filter_id[] = {"low", "medium", "high", "l", "m",
                                        "h",   "0",      "1",    "2"};

int
translate_short_cmd(const char *short_cmd_str) {
  for (int i = 0; i < VALID_COMMANDS_COUNT; i++) {
    if (strcmp(valid_commands_short[i], short_cmd_str) == 0) {
      return i;
    }
  }
  return -1;
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

    props->pairs[i].kind = value;
    props->pairs[i].prop_literal = prop_literal;
    if (prop == ID && value == INT) {
      props->pairs[i].id = strtol(value_literal, NULL, 10);
      props->id_index = i;
    } else if (prop == DESCRIPTION && value == STRING) {
      props->pairs[i].description = value_literal;
      props->description_index = i;
    } else if (prop == DUE_DATE && value == INT) {
      props->pairs[i].due_in_days = strtol(value_literal, NULL, 10);
      props->due_date_index = i;
    } else if (prop == PRIORITY && value == INT) {
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
  int list_opt_argc = 0;
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
  } else if (strcmp(cmd_str, "undone") == 0) {
    cmd_arg_count = UNDONE_ARGUMENTS_COUNT;
  } else if (strcmp(cmd_str, "list") == 0) {
    list_opt_argc = LIST_OPT_ARGC;
  }

  if (argc > cmd_arg_count + 2 + list_opt_argc) {
    printf("Error: wrong number of arguments provided\n");
    // TODO: Errorcode enum, return an error value from this function!!!
    return 3;
  }

  char **args = malloc((argc - 2) * sizeof(char *));
  bool found_opt = false;
  for (int i = 2; i < argc; i++) {
    if (found_opt) {
      found_opt = false;
      continue;
    }

    char *arg = argv[i];
    if (startswith(arg, '-')) {
      // TODO: parse "-f 'prio'" correctly
      found_opt = true;
      if (arg[1] == 'f' && strlen(arg) == 2) {
        cmd->option.kind = FILTER;
        cmd->option.is_valid = true;

        if (i == argc - 1) {
          fprintf(stderr, "Error: did not provide a term to filter by");
          return 1;
        }

        char *filter_by = argv[i + 1];
        int idx = -1;
        for (int i = 0; i < VALID_PRIORITY_FILTER_VALUES; i++) {
          if (strcmp(filter_by, priority_names_for_filter_id[i]) == 0) {
            idx = i;
            break;
          }
        }

        if (idx >= 0) {
          cmd->option.filter.filter_kind = FILTER_PRIORITY;
          cmd->option.filter.prio = (task_priority)(idx % 3);
        } else {
          cmd->option.filter.filter_kind = FILTER_DESCRIPTION;
          cmd->option.filter.description = filter_by;
        }
      }
    }

    if (!found_opt) {
      args[i - 2] = arg;
    }
  }
  cmd->command = cmd_str;
  cmd->args_count = cmd_arg_count;
  cmd->args = args;

  return 0;
}

void
run_list_cmd(task_chain_t *chain, cmd_option_t option) {
  // TODO: fix duplicated code, not sure how right now tho :c
  if (option.kind == FILTER && option.is_valid) {
    char *tasks = chain_repr_filtered(chain, option);
    strlen(tasks) == 0 ? printf("nothing satisfiying the filter was found :3\n")
                       : printf("%s", tasks);
    free(tasks);
  } else {
    char *tasks = chain_repr(chain);
    strlen(tasks) == 0
        ? printf("Your chain is still empty, feel free to add some tasks :3\n")
        : printf("%s", tasks);
    free(tasks);
  }
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
  // TODO: would probably be kinda nice if this was generated from code instead of just straight up written, but that seems like overkill idk, cool tho
  printf("The following commands with he supplied arguments can be used with he daisy chain cli:\n");
  printf("\n");
  printf("list [-f 'h'/'HIGH'/'text']                                         --- list tasks, -f 'text' for filtering\n");
  printf("add    desc='description' due=[days until due] prio=low/medium/high --- adds a task to the chain\n");
  printf("remove id=[number]                                                  --- remove a task\n");
  printf("done   id=[number]                                                  --- mark a task as done\n");
  printf("undone id=[number]                                                  --- mark task as not done\n");
  printf("edit   id=[number] [desc='new desc'] [due=new_due] [prio=new_prio]  --- edit a task\n");
}

int
run_completion_cmd(task_chain_t *chain, property_value_pair_array_t *props,
                   bool val) {
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
  if (task->completed != val) {
    task->completed = val;
  } else {
    fprintf(stderr, "Error: task with given id is already %s\n",
            val ? "completed" : "uncompleted");
  }
  return 0;
}

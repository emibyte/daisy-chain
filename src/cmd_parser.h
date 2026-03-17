#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "task.h"
#include <stdbool.h>

typedef struct TaskChain task_chain_t;

// TODO: eventually list command will also need args (for filter functionality)
#define ADD_ARGUMENTS_COUNT 3      // description, due_date (in days), priority
#define REMOVE_ARGUMENTS_COUNT 1   // id
#define EDIT_ARGUMENTS_COUNT_MIN 2 // id + one prop to change
#define EDIT_ARGUMENTS_COUNT_MAX 4 // id, description, due_date, priority
#define HELP_ARGUMENTS_COUNT 1     // cmd to provide help for
#define DONE_ARGUMENTS_COUNT 1     // id of task to mark as done
#define UNDONE_ARGUMENTS_COUNT 1   // id of task to mark as done

#define LIST_OPT_ARGC                                                          \
  2 * 1 // list cmd is allowed one option, which consists of two args -f and
        // 'filter' filtering!

#define VALID_PRIORITY_FILTER_VALUES 9
#define VALID_COMMANDS_COUNT 7
extern char *valid_commands[];
extern char *valid_commands_short[];

typedef enum OptionKind {
  INVALID,
  FILTER,
} cmd_option_kind;

// NOTE: for now we assume that if a value like l, m, h or low medium high is
// used its meant to filter by priority other wise we assume it meant to filter
// by description
typedef enum FilterBy {
  FILTER_DESCRIPTION,
  FILTER_PRIORITY,
} filter_by;

typedef struct FilterObject {
  filter_by filter_kind;
  union {
    char *description;
    task_priority prio;
  };
} filter_object_t;

typedef struct Option {
  cmd_option_kind kind;
  bool is_valid;
  union {
    filter_object_t filter;
  };
} cmd_option_t;

typedef enum ValueKind {
  STRING,
  INT,
} value_kind_t;

typedef enum Property {
  ID,
  DESCRIPTION,
  DUE_DATE,
  PRIORITY,
} property_t;

typedef struct PropertyValuePair {
  char *prop_literal;
  property_t prop;
  value_kind_t kind;
  union {
    int due_in_days;
    int id;
    task_priority priority;
    char *description;
  };
} property_value_pair_t;

typedef struct {
  property_value_pair_t
      pairs[EDIT_ARGUMENTS_COUNT_MAX]; // chosen because cmd with most
                                       // arguments, not all will be filled in
                                       // depending on what cmd is being
                                       // parsed/executed
  int length;
  int id_index;
  int description_index;
  int due_date_index;
  int priority_index;
} property_value_pair_array_t;

typedef struct Command {
  char *command;
  int args_count;
  char **args;
  cmd_option_t option;
} cmd_t;

int
translate_short_cmd(const char *short_cmd_str);
bool
is_cmd_valid(const char *cmd_str);
int
parse_cmd(char *cmd_str, char **argv, int argc, cmd_t *cmd);
int
parse_property_values(cmd_t cmd, property_value_pair_array_t *props);
void
run_list_cmd(task_chain_t *chain, cmd_option_t option);
void
run_add_cmd(task_chain_t *chain, property_value_pair_array_t *props);
int
run_delete_cmd(task_chain_t *chain, property_value_pair_array_t *props);
void
run_edit_cmd(task_chain_t *chain, property_value_pair_array_t *props);
void
run_help_cmd();
int
run_completion_cmd(task_chain_t *chain, property_value_pair_array_t *props,
                   bool val);
void
free_property_value_pair(property_value_pair_t pair);
void
free_property_value_pair_array(property_value_pair_array_t *array);

#endif

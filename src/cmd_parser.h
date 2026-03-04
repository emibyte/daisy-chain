#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "chain.h"
#include "task.h"
#include <stdbool.h>

// TODO: eventually list command will also need args (for filter functionality)
#define ADD_ARGUMENTS_COUNT 3      // description, due_date (in days), priority
#define REMOVE_ARGUMENTS_COUNT 1   // id
#define EDIT_ARGUMENTS_COUNT_MIN 2 // id + one prop to change
#define EDIT_ARGUMENTS_COUNT_MAX 4 // id, description, due_date, priority
#define HELP_ARGUMENTS_COUNT 1     // cmd to provide help for

#define VALID_COMMANDS_COUNT 5
extern char *valid_commands[];
extern char *valid_commands_short[];

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
} cmd_t;

bool
translate_short_cmd(const char *short_cmd_str, char *cmd_str);
bool
is_cmd_valid(const char *cmd_str);
int
parse_cmd(char *cmd_str, char **argv, int argc, cmd_t *cmd);
int
parse_property_values(cmd_t cmd, property_value_pair_array_t *props);
void
run_list_cmd(task_chain_t *chain);
void
run_add_cmd(task_chain_t *chain, property_value_pair_array_t *props);
int
run_delete_cmd(task_chain_t *chain, property_value_pair_array_t *props);
void
run_edit_cmd(task_chain_t *chain, property_value_pair_array_t *props);
void
run_help_cmd();

#endif

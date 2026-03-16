#include "chain.h"
#include "cmd_parser.h"
#include "files.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char **argv) {
  if (argc == 1) {
    printf("Error: no command provided\n");
    // TODO: Errorcode enum
    return 1;
  }
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  char path[256] = {0};
  char *tasks_path = get_task_file_path(path);

  char cmd_str[256];

  int cmd_trans_idx = translate_short_cmd(argv[1]);
  if (cmd_trans_idx >= 0) {
    strcpy(cmd_str, valid_commands[cmd_trans_idx]);
  } else {
    strcpy(cmd_str, argv[1]);
  }

  for (int i = 0; i < strlen(cmd_str); i++) {
    cmd_str[i] = tolower(cmd_str[i]);
  }

  if (!is_cmd_valid(cmd_str)) {
    printf("Error: command %s is not a valid command, use \'help\' for a list "
           "of valid commands\n",
           cmd_str);
    // TODO: Errorcode enum
    return 2;
  }

  int error_code = 0;
  cmd_t cmd = {0};
  error_code = parse_cmd(cmd_str, argv, argc, &cmd);
  if (error_code != 0) {
    return error_code;
  }

  property_value_pair_array_t props = {0};
  if (strcmp(cmd.command, "help") != 0) {
    props.length = cmd.args_count;
    error_code = parse_property_values(cmd, &props);
    if (error_code != 0) {
      return error_code;
    }
  }

  task_chain_t *chain = load_task_chain(tasks_path);
  // NOTE: execute the command
  if (strcmp(cmd.command, "list") == 0) {
    run_list_cmd(chain, cmd.option);
  } else if (strcmp(cmd.command, "add") == 0) {
    run_add_cmd(chain, &props);
  } else if (strcmp(cmd.command, "remove") == 0) {
    run_delete_cmd(chain, &props);
  } else if (strcmp(cmd.command, "edit") == 0) {
    // NOTE: daisy-chain edit id=1 desc="task description" due=10 prio=medium
    run_edit_cmd(chain, &props);
  } else if (strcmp(cmd.command, "help") == 0) {
    run_help_cmd();
  } else if (strcmp(cmd.command, "done") == 0) {
    run_completion_cmd(chain, &props, true);
  } else if (strcmp(cmd.command, "undone") == 0) {
    run_completion_cmd(chain, &props, false);
  }

  printf("error code: %d\n", error_code);
  bool success = save_task_chain(tasks_path, chain);
  free_chain(chain);
  return error_code;
}

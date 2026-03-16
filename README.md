# daisy-chain
a cli tool to manage tasks

## build
If building on a system with nix installed you can just run
```sh
nix build
```

When not building with nix use the following commands to build the main executable:
```bash
gcc -Wall -O2 -c src/main_cli.c
gcc -Wall -O2 -c src/task.c
gcc -Wall -O2 -c src/chain.c
gcc -Wall -O2 -c src/files.c
gcc -Wall -O2 -c src/cmd_parser.c
gcc -Wall -O2 -c src/utils.c
gcc -o daisy-chain task.o chain.o files.o cmd_parser.o utils.o main_cli.o -ljson-c
```

The library `json-c` is used by the program and thus needs to be installed on ur system.

Tests are written using `munit` and the test executable can be build via the following commands:
```bash
gcc -Wall -O2 -c ${src}/includes/munit/munit.c -I./includes
gcc -Wall -O2 -c src/test.c -I./includes
gcc -Wall -O2 -c src/task.c -I./includes
gcc -Wall -O2 -c src/chain.c -I./includes
gcc -Wall -O2 -c src/files.c -I./includes
gcc -Wall -O2 -c src/cmd_parser.c -I./includes
gcc -Wall -O2 -c src/utils.c -I./includes

gcc -o test-daisy-chain munit.o task.o chain.o files.o cmd_parser.o utils.o test.o -ljson-c
```

Building has only been tested on linux and since the files module uses `sys/stat.h` building on windows probably wont work yet.

## usage
Use the help command for an overview of the different commands.

## todos
- [ ] make due_date actually usable (and think about how to make it usable)
- [ ] build on windows and arm64 (linux)
- [ ] make description search not search for an exact match but instead do a contains search

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../include/gpg.h"
#include "../include/cli.h"

void usage();
void ok(const char *message) { printf(GREEN "[OK]" RESET " %s\n", message); }
void log_cli(const char *message) { printf(PURPLE "[LOG]" RESET " %s\n", message); }
void error(const char *message) { printf(RED "[ERROR]" RESET " %s\n", message); }

int check_args(int argc, int expected, char *message)
{
    if (argc < expected) {
        log_cli("Information:");
        error(message);
        return -1;
    }

    return 0;
}

void cmd_add(int argc, char *argv[]) { 
    if (check_args(argc, 3, "Usage: add <name>.") != 0) return;

    gpg_create(argv[2]); 
}

void cmd_get(int argc, char *argv[]) { 
    if (check_args(argc, 3, "Usage: get <name>.") != 0) return;
    
    gpg_get(argv[2]);
}

void cmd_list(int argc, char *argv[]) { (void) argc; (void) argv; gpg_list(); }

void cmd_help(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    usage();
}

typedef struct
{
    const char *name;
    const char *alias;
    const char *usage;
    const char *description;
    void (*handler)(int argc, char *argv[]);
} Command;

const Command commands[] = {
    {"add",      "-a", "add <name>",      "Creates a new password store",        cmd_add},
    {"get",      "-g", "get <name>",      "Gets the password",                   cmd_get},
    {"list",     "-l", "list",            "Lists all the passwords",             cmd_list},
    {"help",     "-h", "help",            "Shows this screen",                   cmd_help},
};

void usage()
{
    int n = sizeof(commands) / sizeof(commands[0]);

    printf(BOLD "Usage:\n" RESET "  ./alfred <command> <arguments>\n\n");
    printf(BOLD "COMMANDS:\n" RESET);

    for (int i = 0; i < n; i++)
        printf("  %-5s %-10s %-20s  %s\n",
               commands[i].alias,
               commands[i].name,
               commands[i].usage,
               commands[i].description);
}

int verify_args(int argc, char *args[])
{
    if (check_args(argc, 2, "You need to pass one argument.") != 0) return -1;

    int n = sizeof(commands) / sizeof(commands[0]);

    for (int i = 0; i < n; i++) {
        if (strcmp(args[1], commands[i].name) == 0 ||
            strcmp(args[1], commands[i].alias) == 0)
        {
            commands[i].handler(argc, args);
            return 0;
        }
    }

    error("Invalid command.\n");
    return -1;
}

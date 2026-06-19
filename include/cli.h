#ifndef CLI_H
#define CLI_H

#define RED "\033[31m"
#define GREEN "\033[32m"
#define PURPLE "\033[35m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

int verify_args(int argc, char *args[]);
void ok(const char *message);
void error(const char *message);
void usage();

#endif
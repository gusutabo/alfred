#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <termios.h>

#define MAX 512

static char store[MAX];

static void init(void) {
    snprintf(store, MAX, "%s/.password-store", getenv("HOME"));
    mkdir(store, 0700);
}

static void build_path(const char *name, char *out) {
    if (strchr(name, '/') || strstr(name, "..")) {
        fprintf(stderr, "Invalid name.\n");
        exit(1);
    }

    snprintf(out, MAX, "%s/%s.gpg", store, name);
}

static void read_pass(char *buf, size_t n) {
    FILE *tty = fopen("/dev/tty", "r+");
    if (!tty) {
        perror("fopen /dev/tty");
        exit(1);
    }

    struct termios old, raw;
    tcgetattr(fileno(tty), &old);

    raw = old;
    raw.c_lflag &= ~(tcflag_t)ECHO;
    tcsetattr(fileno(tty), TCSAFLUSH, &raw);

    fputs("Password: ", tty);
    fflush(tty);

    fgets(buf, n, tty);
    buf[strcspn(buf, "\n")] = '\0';

    tcsetattr(fileno(tty), TCSAFLUSH, &old);
    fputs("\n", tty);

    fclose(tty);
}

static void cmd_add(const char *name) {
    char p[MAX], pass[256];

    build_path(name, p);
    read_pass(pass, sizeof(pass));

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        dup2(fd[0], STDIN_FILENO);

        close(fd[0]);
        close(fd[1]);

        char *key = getenv("ALFRED_GPG_KEY");
        if (!key) {
            fprintf(stderr, "Missing ALFRED_GPG_KEY\n");
            _exit(1);
        }

        char *args[] = {
            "gpg",
            "-e",
            "-r",
            key,
            "-o",
            p,
            NULL
        };

        execvp("gpg", args);
        perror("execvp failed");
        _exit(1);
    }

    close(fd[0]);

    write(fd[1], pass, strlen(pass));
    write(fd[1], "\n", 1);

    close(fd[1]);

    memset(pass, 0, sizeof(pass));

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        puts("Saved.");
    else
        puts("Error.");
}

static void cmd_get(const char *name) {
    char p[MAX];

    build_path(name, p);

    pid_t pid = fork();

    if (pid == 0) {
        char *args[] = {
            "gpg",
            "-d",
            p,
            NULL
        };

        execvp("gpg", args);
        perror("execvp failed");
        _exit(1);
    }

    waitpid(pid, NULL, 0);
}

static void cmd_list(void) {
    DIR *d = opendir(store);
    if (!d) {
        perror("opendir");
        return;
    }

    struct dirent *e;

    while ((e = readdir(d))) {
        size_t l = strlen(e->d_name);

        if (l > 4 && !strcmp(e->d_name + l - 4, ".gpg")) {
            printf("%.*s\n", (int)(l - 4), e->d_name);
        }
    }

    closedir(d);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage:");
        puts("  alfred add <name>");
        puts("  alfred get <name>");
        puts("  alfred list");
        return 1;
    }

    init();

    if (!strcmp(argv[1], "add") && argc == 3)
        cmd_add(argv[2]);

    else if (!strcmp(argv[1], "get") && argc == 3)
        cmd_get(argv[2]);

    else if (!strcmp(argv[1], "list"))
        cmd_list();

    else {
        puts("Usage:");
        puts("  alfred add <name>");
        puts("  alfred get <name>");
        puts("  alfred list");
        return 1;
    }

    return 0;
}

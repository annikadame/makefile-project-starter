#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "lab.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>




// Parses command-line arguments
void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                printf("lab version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Usage: %s [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

// Initializes the shell environment
void sh_init(struct shell *sh) {
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);
    if (sh->prompt) {
        free(sh->prompt);
        sh->prompt = NULL;
    }

    sh->prompt = get_prompt("MY_PROMPT");
}

// Cleanup shell resources
void sh_destroy(struct shell *sh) {
    if (!sh) return;

    if (sh->prompt) {
        free(sh->prompt);
        sh->prompt = NULL;
    }
}

// Trim leading/trailing whitespace from a string
char *trim_white(char *line) {
    if (!line) return NULL;

    while (*line == ' ') line++;

    char *end = line + strlen(line) - 1;
    while (end > line && *end == ' ') end--;
    *(end + 1) = '\0';

    return line;
}

// Parses command input into arguments
char **cmd_parse(const char *line) {
    if (!line) return NULL;

    int max_args = 128;
    char **cmd = calloc(max_args, sizeof(char *));
    if (!cmd) {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    char *input_copy = strdup(line);
    if (!input_copy) {
        perror("strdup failed");
        free(cmd);
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input_copy, " ");
    int i = 0;
    while (token && i < max_args - 1) {
        cmd[i] = strdup(token);
        if (!cmd[i]) {
            perror("strdup failed");
            free(input_copy);
            cmd_free(cmd);
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, " ");
        i++;
    }
    cmd[i] = NULL;

    free(input_copy);

    return cmd;
}


// Frees memory allocated for command arguments
void cmd_free(char **cmd) {
    if (!cmd) return;

    for (int i = 0; cmd[i] != NULL; i++) {
        free(cmd[i]);
        cmd[i] = NULL;
    }
    free(cmd);
}


// Handles built-in commands like exit, cd, and fg
bool do_builtin(struct shell *sh, char **argv) {
    if (!argv || !argv[0]) {
        return false;
    }

    // Exit command
    if (strcmp(argv[0], "exit") == 0) {
        sh_destroy(sh);
        cmd_free(argv);
        exit(0);
    }

    // cd command
    if (strcmp(argv[0], "cd") == 0) {
        if (change_dir(argv) == -1) {
            fprintf(stderr, "cd: failed to change directory\n");
        }
        return true;
    }

    // fg command
    if (strcmp(argv[0], "fg") == 0) {
        pid_t pid = tcgetpgrp(STDIN_FILENO);
        if (pid > 0) {
            tcsetpgrp(STDIN_FILENO, pid);
            kill(-pid, SIGCONT);
            waitpid(pid, NULL, WUNTRACED);
            tcsetpgrp(STDIN_FILENO, getpgrp());
        } else {
            fprintf(stderr, "No stopped process to resume\n");
        }
        return true;
    }

    return false;
}

// Retrieve shell prompt
char *get_prompt(const char *env) {
    const char *env_value = getenv(env);
    if (env_value != NULL && strlen(env_value) > 0) {
        // Use the environment variables value
        return strdup(env_value);
    } else {
        // Fallback to default prompt if MY_PROMPT is not set or is empty.
        return strdup("shell>");
    }
}




// Change working directory
int change_dir(char **dir) {
    if (!dir || !dir[1] || !dir[1][0]) {
        const char *home = getenv("HOME");
        if (!home) {
            struct passwd *pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            return -1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return -1;
        }
        return 0;
    }
    if (chdir(dir[1]) != 0) {
        perror("cd");
        return -1;
    }
    return 0;
}

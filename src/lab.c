#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "lab.h"

void parse_args(int argc, char **argv)
{
    int opt;
    // Only recognize the -v flag
    while ((opt = getopt(argc, argv, "v")) != -1)
    {
        switch(opt)
        {
            case 'v':
                printf("lab version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Usage: %s [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}


/* Initializes the shell (placeholder for now for testing) */
void sh_init(struct shell *sh) {
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);
    sh->prompt = "shell>";
}

/* Cleanup shell resources */
void sh_destroy(struct shell *sh) {
    free(sh->prompt);
}

/* Trim leading/trailing whitespace (placeholder for now for testing)*/
char *trim_white(char *line) {
    return line;
}

/* Parse command (placeholder for now for testing) */
char **cmd_parse(const char *line) {
    UNUSED(line);
    return NULL;
}

/* Free command memory */
void cmd_free(char **cmd) {
    if (cmd) free(cmd);
}

/* Handle built-in commands (placeholder for now for testing)*/
bool do_builtin(struct shell *sh, char **argv) {
    UNUSED(sh);
    UNUSED(argv);
    return false;
}

char *get_prompt(const char *env) {
    const char *prompt = getenv(env);
    if (!prompt) {
        prompt = "shell>";
    }
    return strdup(prompt);
}

int change_dir(char **dir) {
    if (!dir || !dir[1]) {  // No argument, go to HOME
        const char *home = getenv("HOME");
        if (!home) {
            struct passwd *pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
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

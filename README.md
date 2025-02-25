# Makefile Project Template

This project implements a simple shell with several features including custom prompt support, built-in commands (exit, cd, and fg), job control, and signal handling. It also prints the version when started with a command line argument.

## Features

-Print Version:
  Run the shell with the `-v` flag to print the lab version (ex: `./myprogram -v`).

- GNU Readline:  
  Input is handled using the GNU Readline library, allowing for command history and line editing.

- Custom Prompt:  
  The shell checks for the environment variable `MY_PROMPT`. If set, the shell uses its value as the prompt. If `MY_PROMPT` is unset or empty, the shell defaults to using `shell>`.

- Built-in Commands:  
  Supports several built-in commands that are executed by the shell:
  - `exit`: Terminates the shell.
  - `cd`: Changes the current working directory.
  - `fg`: Resumes a stopped process in the foreground.

- Creating a Process and Signal Handling:
  The shell uses `fork` and `execvp` to create new processes and properly handles signals.


## Building

```bash
make
```

## Testing

```bash
make check
```

## Clean

```bash
make clean
```

## Install Dependencies

In order to use git send-mail you need to run the following command:

```bash
make install-deps
```

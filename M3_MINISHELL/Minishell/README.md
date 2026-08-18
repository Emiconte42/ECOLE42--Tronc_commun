*This project has been created as part of the 42 curriculum by emiconte, wihumeau.*
.
# Minishell

## Description

Minishell is a minimalist shell inspired by Bash, developed as part of the 42 curriculum.
It reproduces the core behavior of a shell: reading commands, expanding variables,
handling redirections, pipes, signals, and essential builtins.

The main goal is to gain a deep understanding of how processes and file descriptors work.
Also it is a opportunity for us to create a project as big as this one. 

## Features

- Interactive prompt with command history
- Executable search via `PATH`, relative or absolute path
- Single and double quote handling (`'` and `"`)
- Environment variable expansion (`$VAR`, `$?`)
- Redirections: `<`, `>`, `>>`, `<<` (heredoc)
- Pipes `|` between commands
- Signal handling: `Ctrl-C`, `Ctrl-D`, `Ctrl-\`
- Single global variable for signals (`g_signal`)
- Implemented builtins:
  - `echo` (with `-n` option)
  - `cd` (relative or absolute path)
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`

## Instructions

### Compilation

```bash
make
```

### Launch

```bash
./minishell
```
or check with valgrind
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=all --suppressions=readline.supp ./minishell
```

### Cleanup

```bash
make clean    # removes object files
make fclean   # removes object files and binary
make re       # recompiles everything
```

### Usage Examples

```bash
# Redirections
echo hello > file.txt
cat < file.txt
echo world >> file.txt

# Pipes
ls | grep .c | wc -l

# Heredoc
cat << EOF
$HOME
EOF

# Variables
export MY_VAR=42
echo $MY_VAR
unset MY_VAR

# Exit status
ls /nonexistent2
echo $?
```

## Resources

### Technical References

- [Bash](http://manpagesfr.free.fr/man/man1/bash.1.html)

- `man 2 fork`, `man 2 execve`, `man 2 pipe`, `man 2 dup2`, `man 2 sigaction`

### AI Usage

AI (Claude, Anthropic) was used to help us understand certain technical points
and to suggest debug printf placements during development.

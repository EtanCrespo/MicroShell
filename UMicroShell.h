#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios old_stdin;
static struct termios old_stdout;

static void _restore(void) {
    tcsetattr(STDIN_FILENO,  TCSAFLUSH, &old_stdin);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &old_stdout);
}

__attribute__((constructor))
static void _init_terminal(void) {
    struct termios raw_in, raw_out;

    tcgetattr(STDIN_FILENO,  &old_stdin);
    tcgetattr(STDOUT_FILENO, &old_stdout);
    atexit(_restore);

    raw_in = old_stdin;
    raw_in.c_lflag &= ~(ICANON | ECHO);
    raw_in.c_iflag |= ICRNL;
    raw_in.c_cc[VMIN]  = 1;
    raw_in.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_in);

    raw_out = old_stdout;
    raw_out.c_oflag |= OPOST | ONLCR;
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &raw_out);
}

#include "MicroShell.h"

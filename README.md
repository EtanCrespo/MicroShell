# MicroShell

This is a simple Shell written in C using standard libraries.

To use it just include ```MicroShell.h``` in your main.

## Functions

it contains 5 functions:
 - ```MicroShell(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size)```
    - Function to call in your main loop to run the command interpreter

 - ```MicroShell_acquire(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size)```
    - Function to call to acquire your commands

 - ```void MicroShell_bind(MicroShell_t *MS, char(*getc)(void), void(*sendc)(char), void(*sends)(char*))```
    - Function to call before MicroShell(...) to bind I/O functions and intialise the default shortcuts

 - ```void MicroShell_shortcuts(MicroShell_t *MS, char *F[12])```
    - Function to affect all your shortcuts at the same time

 - ```void MicroShell_shortcut(MicroShell_t *MS, char Fn, char F[])```
    - Function to change only one shortcut (Fn between 1 and 12)

and 1 macro:
 - ```sizeof_CMDS(CMDS)```
    - Macro to automatically calculate your CMD table size

## Create your commands

The header also contains cmd_t type which is used to create your own commands.

This type contains 3 fields:
 - ```const char *cmd``` string name of your function
 - ```int(*func)(int arc,char** argv)``` function pointer to your command
 - ```const char *desc``` description of your function

To create your own list of commands, just do:

~~~
cmd_t your_CMDS[] = {
    {"command1", cmd_command1, "command1 does something"},
    {"command2, cmd_command2, "command2 does something else"}
};
~~~

Then call MicroShell function with arguments ```&MS```, ```your_CMDS``` and ```sizeof_CMDS(your_CMDS)```

Here is a prototype for a function to call with your command:

```int cmd_command1(int argc, char **argv);```


## Default values and misc behavior

### Dafault commands
 - ```help```: Lists all the available commands
 - ```history```: Prints your command history accepts "clear" or a number as a second argument. with "clear", it clears your history and with a number, it replays the nth command of your history
 - ```clear```: Clears your terminal view
 - ```shortcuts```: Prints all your shortcuts

### Default values

MicroShell contains some default values. They can be overriden with a ```-D``` when compiling.

Thoses values are:

 - ```MICROSHELL_MAX_CMD``` which defines maximum command string size, defaults to 64 (keep in mind that a +2 is hardcoded for the cmdline and a +1 is hardcoded for the shortcuts)
 - ```MICROSHELL_MAX_ARGS``` which defines how many arguments there can be at most, defaults to 16
 - ```MICROSHELL_MAX_HISTORY_SIZE```  which defines your history size, defaults to 16

### Misc behavior
 - If you provide a ```reboot``` command, there's a fallback to run it right after acquisition
 - Pressing tab with nothing typed lists your commands, otherwise it tries to autocompletes your command (just the command, not the argument)
 - You can use up/down keys to move in your history
 - Supports backspace
 - You can override default command by using the same string to call your command
 - No spaces allowed in a command name or arguments
 - Pressing any key of the function row fills your cmd interpreter with a defined string (only works with PuTTY so far)
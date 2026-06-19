# MicroShell

This is a simple Shell written in C using standard libraries.

To use it just include ```MicroShell.h``` in your main.

## Functions

it contains 3 functions:
 - ```MicroShell.init()```

    - Initialisation function, does almost nothing for now
 - ```MicroShell.acquire(cmd_t CMDS[], unsigned int CMDS_size)```

    - Function to call to acquire your commands
 - ```MicroShell.run(cmd_t CMDS[], unsigned int CMDS_size)```

    - Function to call in your main loop to run your last acquired command

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

Then call MicroShell function with arguments ```your_CMDS``` and ```sizeof(your_CMDS)/sizeof(cmd_t)```

Here is a prototype for a function to call with your command:

```int cmd_command1(int argc, char **argv);```


## Default values and misc behavior

### Dafault commands
 - ```list```: Lists all the available commands
 - ```history```: Prints your command history accepts "clear" or a number as a second argument. with "clear", it clears your history and with a number, it replays the nth command of your history
 - ```clear```: Clears your terminal view

### Default values

MicroShell contains some default values. They can be overriden with a ```-D``` when compiling.

Thoses values are:

 - ```MICROSHELL_MAX_CMD``` which defines maximum command string size, defaluts to 64
  - ```MICROSHELL_MAX_ARGS``` which defines how many arguments there can be at most, defaults to 16
   - ```MICROSHELL_MAX_HISTORY_SIZE```  which defines your history size, defaults to 16

### Misc behavior
 - If you provide a ```reboot``` command, there's a fallback to run it right after acquisition
 - Pressing tab with nothing typed lists your commands, otherwise it tries to autocompletes your command
 - You can use up/down keys to move in your history
 - Supports backspace
 - You can override default command by using the same string to call your command
 - No spaces allowed in a command name or arguments
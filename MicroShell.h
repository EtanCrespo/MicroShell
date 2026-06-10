#ifndef MICROSHELL_H
#define MICROSHELL_H

#ifndef MICROSHELL_MAX_CMD
#define MICROSHELL_MAX_CMD (1024)
#endif

#ifndef MICROSHELL_MAX_ARGS
#define MICROSHELL_MAX_ARGS (256)
#endif

#ifndef MICROSHELL_MAX_HISTORY_SIZE
#define MICROSHELL_MAX_HISTORY_SIZE (16)
#endif

typedef struct{
	const char *cmd;
	int(*func)(int arc,char** argv);
	const char *desc;
} cmd_t;

typedef struct{
	void(*init)(void);
	int(*acquire)(cmd_t CMDS[], unsigned int CMDS_size);
	int(*run)(cmd_t CMDS[], unsigned int CMDS_size);
} MicroShell_t;

extern MicroShell_t MicroShell;

#endif

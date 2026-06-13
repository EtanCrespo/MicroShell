#pragma once

#ifndef MICROSHELL_MAX_CMD
#define MICROSHELL_MAX_CMD (64)
#endif

#ifndef MICROSHELL_MAX_ARGS
#define MICROSHELL_MAX_ARGS (16)
#endif

#ifndef MICROSHELL_MAX_HISTORY_SIZE
#define MICROSHELL_MAX_HISTORY_SIZE (16)
#endif

typedef struct{
	const char *cmd;
	int(*func)(int arc,char** argv);
	const char *desc;
} cmd_t;

class MicroShell_c{
	public:
		void init(void);
		int acquire(cmd_t CMDS[], unsigned int CMDS_size);
		int run(cmd_t CMDS[], unsigned int CMDS_size);
};

static MicroShell_c MicroShell;
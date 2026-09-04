#ifndef MICROSHELL_H
#define MICROSHELL_H

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
	int(*func)(int argc,char** argv);
	const char *desc;
} cmd_t;

typedef enum{
	MS_INIT = 0,
	MS_IDLE,
	MS_RUN
} MicroShell_s;

typedef struct{
	char(*getc)(void);
	void(*sendc)(char);
	void(*sends)(char*);
	char cmd[MICROSHELL_MAX_CMD];
	int cmd_pos;
	char history[MICROSHELL_MAX_HISTORY_SIZE][MICROSHELL_MAX_CMD];
	int history_size;
	int history_pos;
	MicroShell_s state;
	MicroShell_s state_n;
	short unsigned int from_history;
	short unsigned int ESC;
	short unsigned int Fn_f;
	short unsigned int F;
} MicroShell_t;

void MicroShell(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size);
void MicroShell_bind(MicroShell_t *MS, char(*getc)(void), void(*sendc)(char), void(*sends)(char*));
int MicroShell_acquire(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size);

#define sizeof_CMDS(CMDS) (unsigned int)sizeof(CMDS)/sizeof(cmd_t)

extern char __attribute__((weak)) F1[];

#endif

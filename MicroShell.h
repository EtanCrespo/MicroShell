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
	MS_RUN,
	MS_TAB,
	MS_UP,
	MS_DOWN,
	// MS_LEFT,
	// MS_RIGHT,
	MS_FUNCTION,
	MS_BACKSPACE
} MicroShell_s;

typedef struct{
	char(*getc)(void);
	void(*sendc)(char);
	void(*sends)(char*);
	char cmd[MICROSHELL_MAX_CMD+2];
	int cmd_pos;
	char history[MICROSHELL_MAX_HISTORY_SIZE][MICROSHELL_MAX_CMD+2];
	int history_size;
	int history_pos;
	MicroShell_s state;
	MicroShell_s state_n;
	short unsigned int Ctrl_C;
	short unsigned int from_history;
	short unsigned int Fn_key;
	short unsigned int ESC_f;
	char F[12][MICROSHELL_MAX_CMD+1];
} MicroShell_t;

void MicroShell(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size);
int MicroShell_acquire(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size);

void MicroShell_bind(MicroShell_t *MS, char(*getc)(void), void(*sendc)(char), void(*sends)(char*));
void MicroShell_shortcuts(MicroShell_t *MS, char *F[12]);
void MicroShell_shortcut(MicroShell_t *MS, char Fn, char F[]);

short unsigned int MicroShell_Ctrl_C(MicroShell_t *MS);
void MicroShell_reset_Ctrl_C(MicroShell_t *MS);

#define sizeof_CMDS(CMDS) (unsigned int)sizeof(CMDS)/sizeof(cmd_t)


#endif

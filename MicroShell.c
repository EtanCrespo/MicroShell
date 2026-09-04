#include "MicroShell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char F1[] = "";

int cmd_help(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv);
int cmd_clear(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv);
int cmd_history(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv);

typedef struct{
	const char *cmd;
	int(*func)(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc,char** argv);
	const char *desc;
} def_cmd_t;


const def_cmd_t def_cmds[] ={
	{"help", cmd_help, "Lists the commands"},
	{"clear", cmd_clear, "Clears the screen"},
	{"history", cmd_history, "Prints the history"}
};

int cmd_help(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv){
	if(CMDS != NULL || CMDS_size != 0){
		MS->sends("Added commands:\r\n");
		for(unsigned int i = 0; i < CMDS_size; i++){
			MS->sends("  ");
			MS->sends((char *)CMDS[i].cmd);
			MS->sends(" : ");
			MS->sends((char *)CMDS[i].desc);
			MS->sends("\r\n");
		}
	}
	MS->sends("Default commands:\r\n");
	for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
		MS->sends("  ");
		MS->sends((char *)def_cmds[i].cmd);
		MS->sends(" : ");
		MS->sends((char *)def_cmds[i].desc);
		MS->sends("\r\n");
	}
	return 0;
}

int cmd_clear(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv){
	MS->sends("\033[2J\033[0;0H");
	return 0;
}

int cmd_history(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size, int argc, char **argv){
	if(argc == 1){
		MS->sends("History:\r\n");
		for(int i = 0; i < MS->history_size; i++){
			MS->sends("  ");
			char h_index[9];
			sprintf(h_index, "%d", i+1);
			MS->sends(h_index);
			MS->sends(": ");
			MS->sends(MS->history[i]);
			MS->sends("\r\n");
		}
	}
	else if(argc == 2){
		if(strcmp(argv[1],"clear") == 0){
			MS->history_size = 0;
		}
		else if(atoi(argv[1]) > 0 && atoi(argv[1]) < MS->history_size){
			strcpy((char*)MS->cmd,MS->history[atoi(argv[1])-1]);
			MS->from_history = 1;
			MS->state_n = MS_RUN;
		}
	}
	return 0;
}

int MS_autocomplete(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size){
	char matches[CMDS_size+sizeof(def_cmds)/sizeof(cmd_t)][MICROSHELL_MAX_CMD];
	int matches_size = 0;
	int match_cnt = 0;
	// Used only when there's one match
	int match = 0;
	// We prevent any arror when we only have default commands
	if(CMDS != NULL || CMDS_size != 0){
		for(unsigned int i = 0; i < CMDS_size; i++){
			if(MS->cmd[0] == CMDS[i].cmd[0]){
				strcpy((char*)matches[matches_size++],CMDS[i].cmd);
				match_cnt++;
			}
		}
	}

	for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
		if(MS->cmd[0] == def_cmds[i].cmd[0]){
			strcpy((char*)matches[matches_size++],def_cmds[i].cmd);
			match_cnt++;
		}
	}

	if(match_cnt == 0){
		return 0;
	}

	for(int i = 0; i < MS->cmd_pos; i++){
		if(match_cnt == 1){
			break;
		}
		for(int j = 0; j < matches_size; j++){
			if(match_cnt == 1){
				match = j;
				break;
			}
			if(MS->cmd[i] != matches[j][i]){
				matches[j][0] = '\0';
				match_cnt--;
			}
		}
	}

	if(match_cnt == 1){
		strcpy((char*)MS->cmd,matches[match]);
		MS->cmd_pos = strlen(MS->cmd);
		MS->sends("\r> ");
		MS->sends(MS->cmd);
		MS->sends("\033[j");
		return 0;
	}
	MS->sends("\033[s\r\n\033[K");
	for(int i = 0; i < matches_size; i++){
		MS->sends(matches[i]);
		MS->sends("\t");
	}
	MS->sends("\033[u");
	return 0;
}

void MS_init(MicroShell_t *MS){
	MS->cmd_pos = 0;
	MS->history_size = 0;
	MS->history_pos = 0;
	MS->from_history = 0;
	MS->ESC = 2;
	MS->Fn_f = 0;
	MS->F = 0;
	MS->sends("MicroShell\r\n> ");
}

int MS_run(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size){
	char *argv[MICROSHELL_MAX_ARGS];
	int cmd_len = strlen(MS->cmd);
	argv[0] = strtok(MS->cmd," ");
	if((argv[0] == NULL) || (cmd_len <= 1)){
		MS->sends("> ");
		return 0;
	}
	int argc = 1;
	char *arg;
	arg = strtok(NULL," ");
	while(arg!=NULL){
		argv[argc++] = arg;
		arg = strtok(NULL," ");
		if(argc >= MICROSHELL_MAX_ARGS){
			break;
		}
	}
	if(CMDS != NULL || CMDS_size != 0){
		for(unsigned int i = 0; i < CMDS_size; i++){
			if(strcmp(CMDS[i].cmd,argv[0]) == 0){
				CMDS[i].func(argc,argv);
				MS->sends("> ");
				return 0;
			}
		}
	}

	for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
		if(strcmp(def_cmds[i].cmd, argv[0]) == 0){
			def_cmds[i].func(MS, CMDS, CMDS_size, argc , argv);
			if(MS->from_history == 0){
				MS->sends("> ");
			}
			MS->from_history = 0;
			return 0;
		}
	}
	MS->sends(argv[0]);
	MS->sends(" not found\r\n");
	MS->sends("> ");
	MS->history_size = ((MS->history_size-1)<0)?0:MS->history_size-1;
	return 0;
}

void MicroShell(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size){
	MS->state = MS->state_n;
	switch (MS->state)
	{
	case MS_INIT:
		MS->state_n = MS_IDLE;
		MS_init(MS);
		break;
	case MS_IDLE:
		break;
	case MS_RUN:
		MS->state_n = MS_IDLE;
		MS_run(MS, CMDS, CMDS_size);
		break;
	default:
		MS->state_n = MS_INIT;
		break;
	}
}

void MicroShell_bind(MicroShell_t *MS, char(*getc)(void), void(*sendc)(char), void(*sends)(char*)){
	MS->getc = getc;
	MS->sendc = sendc;
	MS->sends = sends;
	MS->state_n = MS_INIT;
}

int MicroShell_acquire(MicroShell_t *MS, cmd_t CMDS[], unsigned int CMDS_size){
	char c = MS->getc();
	if(MS->ESC == 0){
		MS->ESC = 2;
		switch(c){
			// Beginning of arrow detection
			case 'A':
				strcpy((char*)MS->cmd,(char*)MS->history[MS->history_pos]);
				// We decrement when we go up since we use a simple list where index 0 is the oldest command and ...size-1 is the latest
				MS->history_pos = (((MS->history_pos-1)%((MS->history_size-1))) == -1)?MS->history_size-1:((MS->history_pos-1)%((MS->history_size-1)));
				if(MS->history_pos == MS->history_size-1){
					MS->history_pos = 0;
				}
				MS->cmd_pos = strlen((char*)MS->cmd)-1;
				MS->sends("\r\033[K> ");
				MS->sends(MS->cmd);
				MS->sends("\b");
				break;
			case 'B':
				if(MS->history_pos == MS->history_size-1){
					MS->cmd_pos = 0;
					strcpy(MS->cmd,"");
					MS->sends("\r\033[K> ");
					return 0;
				}
				// Same logic here
				MS->history_pos = (MS->history_pos+1)%MS->history_size;
				strcpy((char *)MS->cmd,(char *)MS->history[MS->history_pos]);
				MS->cmd_pos = strlen((char *)MS->cmd)-1;
				MS->sends("\r\033[K> ");
				MS->sends(MS->cmd);
				MS->sends("\b");
				break;
			// Ending of arrow detection
			// Beginning of function keys detection
	
			case '1':
				if(MS->F == 0){
					MS->F = 1;
					MS->ESC = 0;
					return 0;
				}
				else if(MS->F == 1){
					MS->F = 0;
					MS->Fn_f = 1;
					strcpy(MS->cmd, F1);
					MS->sends("\r> ");
					MS->sends(MS->cmd);
					MS->cmd_pos = strlen(MS->cmd);
					goto ENTER;
				}
				else if(MS->F == 2){
					MS->F = 0;
					MS->sends("F10 pressed\r\n");
				}
				break;
			case '2':
				if(MS->F == 0){
					MS->F = 2;
					MS->ESC = 0;
					return 0;
				}
				else if(MS->F == 1){
					MS->F = 0;
					MS->sends("F2 pressed\r\n");
				}
				else if(MS->F == 2){
					MS->F = 0;
					MS->sends("F11 pressed\r\n");
				}
				break;
	
			// Ending of function keys detection
			default:
				// MS->sends("\r\nunused escape sequence detected (e[");
				// MS->sends(c);
				// MS->sends(")\r\n");
				break;
		}
		return 0;
	}
	switch(c){
		case '\r':
			ENTER:  // No need to rewrite this for \n since we can use a goto
			MS->cmd[MS->cmd_pos++] = ' ';
			MS->cmd[MS->cmd_pos] = '\0';
			MS->sends("\r\n");
			MS->state_n = MS_RUN;
			if(MS->cmd_pos != 1){               
				strcpy((char *)MS->history[MS->history_size], (char *)MS->cmd);
				MS->history_size = (MS->history_size+1)%MICROSHELL_MAX_HISTORY_SIZE;
				MS->history_pos = MS->history_size-1;
			}
			MS->cmd_pos = 0;
			if(CMDS != NULL || CMDS_size != 0){
				char *cmd = "0";
				char *cmd0;
				strcpy(cmd, MS->cmd);
				cmd0 = strtok(cmd," ");
				if(strcmp(cmd0,"reboot") == 0){
					for(unsigned int i = 0; i < CMDS_size; i++){
						if(strcmp(CMDS[i].cmd,cmd0) == 0){
							CMDS[i].func(1,&cmd0);
							return 0;
						}
					}
				}
			}
			break;
		case '\n':
			goto ENTER;
			break;
		// Tab case, used to handle autocomplete
		case '\t':
			if(MS->cmd_pos == 0){
				strcpy(MS->cmd,"help");
				MS->sends("\r");
				MS->state_n = MS_RUN;
				return 0;
			}
			MS->cmd[MS->cmd_pos] = '\0';
			MS_autocomplete(MS, CMDS, CMDS_size);
			break;
		// Backspace case
		case '\177':
			if(MS->cmd_pos > 0){
				MS->sends("\b\033[K");
			}
			MS->cmd[MS->cmd_pos--] = '\0';
			if(MS->cmd_pos < 0){
				MS->cmd_pos = 0;
			}
			break;
		// Escape sequence detection begin
		case '\033':
			MS->ESC--;
			break;
		// Escape sequence introducer, we still want it to be taken into account for a command when not in an escape sequence
		case '[':
			if(MS->ESC == 1){
				MS->ESC--;
			}
			else{
				goto DEFAULT;
			}
			break;
		case '~':
			if(MS->Fn_f){
				MS->Fn_f = 0;
				break;
			}
			else{
				goto DEFAULT;
			}
			break;
		// Default case to handle all characters
		default:
			DEFAULT:
			MS->cmd[MS->cmd_pos] = c;
			MS->cmd_pos = (MS->cmd_pos+1)%MICROSHELL_MAX_CMD;
			if(MS->cmd_pos == 0){
				MS->sends("\r\n/!\\ Max size of a command overflow, you erased it, might consider overriding MICROSHELL_MAX_CMD\r\n> ");
			}
			MS->sendc(c);
			// Printf for debug purposes
			// MS->sends("c value:");
			// MS->sends(c);
			// MS->sends("\r\n");
			break;
	}
	return 0;
}
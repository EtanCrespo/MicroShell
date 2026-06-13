#include "MicroShell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void init(void);
int acquire(cmd_t CMDS[], unsigned int CMDS_size);
int run_cmd(cmd_t CMDS[], unsigned int CMDS_size);

MicroShell_t MicroShell = {init, acquire, run_cmd};

char G_cmd[MICROSHELL_MAX_CMD];
int G_cmd_pos = 0;
char G_history[MICROSHELL_MAX_HISTORY_SIZE][MICROSHELL_MAX_CMD];
int G_history_size = 0;
int G_history_pos = 0;
short unsigned int G_cmd_run = 0;
short unsigned int G_from_history = 0;

int cmd_clear(int argc, char **argv);
int cmd_history(int argc, char **argv);

const cmd_t def_cmds[] ={
	{"list", NULL, "Lists the commands"},
	{"clear", cmd_clear, "Clears the screen"},
	{"history", cmd_history, "Prints the history"},
};

int cmd_clear(int argc, char **argv){
	printf("\033[2J\033[0;0H\r\n");
	return 0;
}

int cmd_history(int argc, char **argv){
	if(argc == 1){
		printf("History:\r\n");
		for(int i = 0; i < G_history_size; i++){
			printf("  %d: %s\r\n",i+1,G_history[i]);
		}
	}
	else if(argc == 2){
		if(strcmp(argv[1],"clear") == 0){
			G_history_size = 0;
		}
		else if(atoi(argv[1]) > 0 && atoi(argv[1]) <G_history_size){
			strcpy((char*)G_cmd,G_history[atoi(argv[1])-1]);
			G_from_history = 1;
			G_cmd_run = 1;
		}
	}
	return 0;
}

int run_cmd(cmd_t CMDS[], unsigned int CMDS_size){
	if(G_cmd_run == 1){
		G_cmd_run = 0;
		char *argv[MICROSHELL_MAX_ARGS];
		argv[0] = strtok(G_cmd," ");
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
		if(CMDS != NULL){
			for(unsigned int i = 0; i < CMDS_size; i++){
				if(strcmp(CMDS[i].cmd,argv[0]) == 0){
					CMDS[i].func(argc,argv);
					printf("> ");
					return 0;
				}
			}
		}
		if(strcmp(argv[0],"list") == 0){
			if(CMDS != NULL){
				printf("Added commands:\r\n");
				for(unsigned int i = 0; i < CMDS_size; i++){
					printf("  %s : %s\r\n",CMDS[i].cmd,CMDS[i].desc);
				}
			}
			printf("Default commands:\r\n");
			for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
				printf("  %s : %s\r\n",def_cmds[i].cmd,def_cmds[i].desc);
			}
			printf("> ");
			return 0;
		}

		for(unsigned int i = 1; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
			if(strcmp(def_cmds[i].cmd,argv[0]) == 0){
				def_cmds[i].func(argc,argv);
				if(G_from_history == 0){
					printf("> ");
				}
				G_from_history = 0;
				return 0;
			}
		}
		if(argv[0] == NULL){
			printf("> ");
			return 0;
		}
		printf("%s not found\r\n",argv[0]);
		printf("> ");
		return -1;
	}
	return 0;
}

void init(void){
	printf("MicroShell\r\n> ");
}

void autocomplete(cmd_t CMDS[], unsigned int CMDS_size){
	char matches[CMDS_size+sizeof(def_cmds)/sizeof(cmd_t)][MICROSHELL_MAX_CMD];
	int matches_size = 0;
	int match_cnt = 0;
	// Used only when there's one match
	int match;
	for(unsigned int i = 0; i < CMDS_size; i++){
		if(G_cmd[0] == CMDS[i].cmd[0]){
			strcpy((char*)matches[matches_size++],CMDS[i].cmd);
			match_cnt++;
		}
	}

	for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
		if(G_cmd[0] == def_cmds[i].cmd[0]){
			strcpy((char*)matches[matches_size++],def_cmds[i].cmd);
			match_cnt++;
		}
	}

	if(match_cnt == 0){
		return;
	}

	for(int i = 0; i < G_cmd_pos; i++){
		if(match_cnt == 1){
			break;
		}
		for(int j = 0; j < matches_size; j++){
			if(match_cnt == 1){
				match = j;
				break;
			}
			if(G_cmd[i] != matches[j][i]){
				matches[j][0] = '\0';
				match_cnt--;
			}
		}
	}

	if(match_cnt == 1){
		strcpy((char*)G_cmd,matches[match]);
		G_cmd_pos = strlen(G_cmd);
		printf("\r> %s\033[J",G_cmd);
		return;
	}
	printf("\033[s\r\n\033[K");
	for(int i = 0; i < matches_size; i++){
		printf("%s\t",matches[i]);
	}
	printf("\033[u");
}

int acquire(cmd_t CMDS[], unsigned int CMDS_size){
	static short unsigned int ESC = 2;
	char c;
	scanf("%c",&c);
	if(ESC == 0){
		ESC = 2;
		static short unsigned int F = 0;
		switch(c){
			case 'A':
				strcpy((char*)G_cmd,(char*)G_history[G_history_pos]);
				G_history_pos = (((G_history_pos-1)%((G_history_size-1))) == -1)?G_history_size-1:((G_history_pos-1)%((G_history_size-1)));
				G_cmd_pos = strlen((char*)G_cmd);
				printf("\r\033[K> %s",G_cmd);
				break;
			case 'B':
				G_history_pos = (G_history_pos+1)%G_history_size;
				strcpy((char *)G_cmd,(char *)G_history[G_history_pos]);
				G_cmd_pos = strlen((char *)G_cmd);
				printf("\r\033[K> %s",G_cmd);
				break;
			case 'C':

	/*
			case '1':
				if(F == 0){
					F = 1;
					ESC = 0;
					return 0;
				}
				else if(F == 1){
					F = 0;
					printf("F1 pressed\r\n");
				}
				else if(F == 2){
					F = 0;
					printf("F10 pressed\r\n");
				}
				break;
			case '2':
				if(F == 0){
					F = 2;
					ESC = 0;
					return 0;
				}
				else if(F == 1){
					F = 0;
					printf("F2 pressed\r\n");
				}
				else if(F == 2){
					F = 0;
					printf("F11 pressed\r\n");
				}
				break;
	*/
			default:
				printf("\r\nunused escape sequence detected (e[%d)\r\n",c);
				break;
		}
		return 0;
	}
	switch(c){
		case '\r':
			ENTER:  // No need to rewrite this for \n since we can use a goto
			G_cmd[G_cmd_pos++] = ' ';
			G_cmd[G_cmd_pos] = '\0';
			G_cmd_pos = 0;
			printf("\r\n");
			G_cmd_run = 1;
			if(strlen((char *)G_cmd) != 0){               
				strcpy((char *)G_history[G_history_size], (char *)G_cmd);
				G_history_size = (G_history_size+1)%MICROSHELL_MAX_HISTORY_SIZE;
				G_history_pos = G_history_size-1;
			}
			if(CMDS != NULL){
				char *cmd = "0";
				char *cmd0;
				strcpy(cmd, G_cmd);
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
		case '\t':
			if(G_cmd_pos == 0){
				strcpy(G_cmd,"list");
				printf("\r");
				G_cmd_run = 1;
				return 0;
			}
			G_cmd[G_cmd_pos] = '\0';
			autocomplete(CMDS,CMDS_size);
			break;
		case '\177':
			G_cmd[G_cmd_pos--] = '\0';
			if(G_cmd_pos < 0){
				G_cmd_pos = 0;
			}
			printf("\b\033[K");
			break;
		case '\033':
			ESC--;
			break;
		case '[':
			if(ESC == 1){
				ESC--;
			}
			else{
				printf("[");
			}
			break;
		default:
			G_cmd[G_cmd_pos] = c;
			G_cmd_pos = (G_cmd_pos+1)%MICROSHELL_MAX_CMD;
			if(G_cmd_pos == 0){
				printf("\r\n/!\\ Max sie of a command overflow, you erased it, might consider overriding MICROSHELL_MAX_CMD\r\n> ");
			}
			printf("%c",c);
			//printf("c value:%d\r\n",c);
			break;
	}
	return 0;
}

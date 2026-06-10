#include "MicroShell.h"
#include <stdio.h>
#include <string.h>

void init(void);
int acquire(cmd_t CMDS[], unsigned int CMDS_size);
int run_cmd(cmd_t CMDS[], unsigned int CMDS_size);

MicroShell_t MicroShell = {init, acquire, run_cmd};

char G_cmd[MICROSHELL_MAX_CMD];
int G_cmd_pos = 0;
short unsigned int G_cmd_run = 0;

int cmd_clear(int argc, char **argv);

const cmd_t def_cmds[] ={
	{"clear", cmd_clear, "Clears the screen"},
};

int cmd_clear(int argc, char **argv){
	printf("\033[2J\033[0;0H");
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
			printf("  list : Lists the commands\r\n");
			for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
				printf("  %s : %s\r\n",def_cmds[i].cmd,def_cmds[i].desc);
			}
			printf("> ");
			return 0;
		}

		for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
			if(strcmp(def_cmds[i].cmd,argv[0]) == 0){
				def_cmds[i].func(argc,argv);
				printf("> ");
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

int acquire(cmd_t CMDS[], unsigned int CMDS_size){
	char c;
	printf("\033[s");
	c = getchar();
	switch(c){
		case '\r':
			G_cmd[G_cmd_pos++] = ' ';
			G_cmd[G_cmd_pos] = '\0';
			G_cmd_pos = 0;
			printf("\r\n");
			G_cmd_run = 1;
			break;
		case '\t':
			printf("\033[u");
			break;
		default:
			G_cmd[(G_cmd_pos++)%MICROSHELL_MAX_CMD] = c;
			//printf("c value:%d\r\n",c);
			break;
	}
	return 0;
}

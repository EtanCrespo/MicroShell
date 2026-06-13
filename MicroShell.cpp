#include "MicroShell.hpp"
#include <Arduino.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

void init(void);
int acquire(cmd_t CMDS[], unsigned int CMDS_size);
int run_cmd(cmd_t CMDS[], unsigned int CMDS_size);


char G_cmd[MICROSHELL_MAX_CMD];
int G_cmd_pos = 0;
char G_history[MICROSHELL_MAX_HISTORY_SIZE][MICROSHELL_MAX_CMD];
int G_history_size = 0;
int G_history_pos = 0;
short unsigned int G_cmd_run = 0;

int cmd_clear(int argc, char **argv);
int cmd_history(int argc, char **argv);

const cmd_t def_cmds[] ={
	{"clear", cmd_clear, "Clears the screen"},
	{"history", cmd_history, "Prints the history"},
};

int cmd_clear(int argc, char **argv){
	Serial.printf("\033[2J\033[0;0H\r\n");
	return 0;
}

int cmd_history(int argc, char **argv){
	if(argc == 1){
		Serial.printf("History:\r\n");
		for(int i = 0; i < G_history_size; i++){
			Serial.printf("  %d: %s\r\n",i+1,G_history[i]);
		}
	}
	else if(argc == 2){
		if(strcmp(argv[1],"clear") == 0){
			G_history_size = 0;
		}
		else if(atoi(argv[1]) > 0 && atoi(argv[1]) <G_history_size){
			strcpy((char*)G_cmd,G_history[atoi(argv[1])-1]);
			G_cmd_run = 1;
		}
	}
	return 0;
}

int MicroShell_c::run(cmd_t CMDS[], unsigned int CMDS_size){
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
					Serial.printf("> ");
					return 0;
				}
			}
		}
		if(strcmp(argv[0],"list") == 0){
			if(CMDS != NULL){
				Serial.printf("Added commands:\r\n");
				for(unsigned int i = 0; i < CMDS_size; i++){
					Serial.printf("  %s : %s\r\n",CMDS[i].cmd,CMDS[i].desc);
				}
			}
			Serial.printf("Default commands:\r\n");
			Serial.printf("  list : Lists the commands\r\n");
			for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
				Serial.printf("  %s : %s\r\n",def_cmds[i].cmd,def_cmds[i].desc);
			}
			Serial.printf("> ");
			return 0;
		}

		for(unsigned int i = 0; i < sizeof(def_cmds)/sizeof(cmd_t); i++){
			if(strcmp(def_cmds[i].cmd,argv[0]) == 0){
				def_cmds[i].func(argc,argv);
				Serial.printf("> ");
				return 0;
			}
		}
		if(argv[0] == NULL){
			Serial.printf("> ");
			return 0;
		}
		Serial.printf("%s not found\r\n",argv[0]);
		Serial.printf("> ");
		return -1;
	}
	return 0;
}

void MicroShell_c::init(void){
	Serial.println("MicroShell");
	Serial.print("> ");
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
		Serial.printf("\r> %s\033[J",G_cmd);
		return;
	}
	Serial.printf("\033[s\r\n\033[K");
	for(int i = 0; i < matches_size; i++){
		Serial.printf("%s\t",matches[i]);
	}
	Serial.printf("\033[u");
}

int MicroShell_c::acquire(cmd_t CMDS[], unsigned int CMDS_size){
	#ifndef WITH_IRQ
	if(Serial.available()){
	#endif
	static short unsigned int arrow = 2;
	char c;
	c = Serial.read();
	Serial.print(c);
	if(arrow == 0){
		arrow = 2;
		switch(c){
			case 'A':
				Serial.printf("\033[B");
				strcpy((char*)G_cmd,(char*)G_history[G_history_pos]);
				G_history_pos = (((G_history_pos-1)%((G_history_size-1))) == -1)?G_history_size-1:((G_history_pos-1)%((G_history_size-1)));
				G_cmd_pos = strlen((char*)G_cmd);
				Serial.printf("\r\033[K> %s",G_cmd);
				break;
			case 'B':
				Serial.printf("\033[A");
				G_history_pos = (G_history_pos+1)%G_history_size;
				strcpy((char *)G_cmd,(char *)G_history[G_history_pos]);
				G_cmd_pos = strlen((char *)G_cmd);
				Serial.printf("\r\033[K> %s",G_cmd);
				break;
			default:
				Serial.printf("\r\nunused escape sequence detected\r\n");
				break;
		}
		return 0;
	}
	switch(c){
		case '\r':
			ENTER:  // No need to rewrite this for \n since we can use a goto
			if(G_cmd_pos == 0){
				Serial.println();
				Serial.print("> ");
				return 0;
			}
			G_cmd[G_cmd_pos++] = ' ';
			G_cmd[G_cmd_pos] = '\0';
			G_cmd_pos = 0;
			Serial.println();
			G_cmd_run = 1;
			if(strlen((char *)G_cmd) != 0){               
				strcpy((char *)G_history[G_history_size], (char *)G_cmd);
				G_history_size = (G_history_size+1)%MICROSHELL_MAX_HISTORY_SIZE;
				G_history_pos = G_history_size-1;
			}
			if(CMDS != NULL){
				char *cmd;
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
			Serial.printf("\r\033[%dC",G_cmd_pos+2);
			if(G_cmd_pos == 0){
				strcpy(G_cmd,"list");
				Serial.printf("\r");
				G_cmd_run = 1;
				return 0;
			}
			G_cmd[G_cmd_pos] = '\0';
			autocomplete(CMDS,CMDS_size);
			break;
		case '\177':
			Serial.printf("\b\e[K");
			G_cmd[G_cmd_pos--] = '\0';
			if(G_cmd_pos < 0){
				G_cmd_pos = 0;
				Serial.printf("\r\033[2C");
			}
			break;
		case '\033':
			arrow--;
			break;
		case '[':
			if(arrow == 1){
				arrow--;
			}
			break;
		default:
			G_cmd[G_cmd_pos] = c;
			G_cmd_pos = (G_cmd_pos+1)%MICROSHELL_MAX_CMD;
			if(G_cmd_pos == 0){
				Serial.printf("\r\n/!\\ Max sie of a command overflow, you erased it, might consider overriding MICROSHELL_MAX_CMD\r\n> ");
			}
			//Serial.printf("c value:%d\r\n",c);
			break;
	}
	#ifndef WITH_IRQ
	}
	#endif
	return 0;
}

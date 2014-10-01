#include "stdio.h"
#include "defs.h"
#include "string.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
	return;
}
void ParsePosition(char* lineIn, S_BOARD *pos){
	return;
}

void Uci_Loop() {
	//remove buffering so we don't miss commands from/to GUI
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char line[INPUTBUFFER];
	printf("id name %s\n", NAME);
	printf("id author Emil Ostensen");
	printf("uciok");

	S_BOARD pos[1];
	S_SEARCHINFO info[1];
	InitPvTable(pos->PvTable);

	while (TRUE) {
		//clear the line
		memset(&line[0], 0, sizeof(line));
		//flush what we might have sent to the GUI earlier
		fflush(stdout);
		//run until we get a command from the GUI
		if ((!fgets(line, INPUTBUFFER, stdin)) || line[0] == '\n') {
			continue;
		}
		
		if (!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
		} else if (!strncmp(line, "position", 8)) {
			ParsePosition(line, pos);
		} else if (!strncmp(line, "ucinewgame", 10)) {
			ParsePosition("position startpos\n", pos);
		} else if (!strncmp(line, "go", 2)) {
			ParseGo(line, info, pos);
		} else if (!strncmp(line, "quit", 4)) {
			info->quit = TRUE;
		} else if (!strncmp(line, "uci", 3)) {
			printf("id name %s\n", NAME);
			printf("id author Emil Ostensen");
			printf("uciok");
		}

		if(info->quit) {
			break;
		}
	}
}

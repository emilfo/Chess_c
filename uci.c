#include "stdio.h"
#include "defs.h"
#include "string.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
	return;
}

//Parses position from the UCI pro
void ParsePosition(char* lineIn, S_BOARD *pos){
	//skip 'position '
	char* strPtr = lineIn + 9;
	
	if (strncmp(strPtr, "startpos", 8) == 0) {
		parseFEN(START_FEN, pos);
		strPtr += 9;
	} else if (strncmp(strPtr, "fen", 3) == 0) {
		strPtr += 4;
		parseFEN(strPtr, pos);
	} else { //failsafe, should never happen
		parseFEN(START_FEN, pos);
	}
	
	strPtr = strstr(lineIn, "moves");
	int move;

	//the GUI wants us to parse moves aswell
	if (strPtr != NULL) {
		//skip 'moves '
		strPtr += 6;

		while (*strPtr) {
			move = ParseMove(strPtr, pos);

			if (move == NOMOVE) {
				break;
			}

			MakeMove(pos, move);

			pos->ply = 0;

			//skip parsed move move
			while (*strPtr && *strPtr != ' ') {
				strPtr++;
			}
			strPtr++;
		}
	}
	printBoard(pos);
}

void Uci_Loop() {
	//remove buffering so we don't miss commands from/to GUI
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char line[INPUTBUFFER];
	printf("id name %s\n", NAME);
	printf("id author Emil Ostensen\n");
	printf("uciok\n");

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
		
		if (strncmp(line, "isready", 7) == 0) {
			printf("readyok\n");
			continue;
		} else if (strncmp(line, "position", 8) == 0) {
			ParsePosition(line, pos);
		} else if (strncmp(line, "ucinewgame", 10) == 0) {
			ParsePosition("position startpos\n", pos);
		} else if (strncmp(line, "go", 2) == 0) {
			ParseGo(line, info, pos);
		} else if (strncmp(line, "quit", 4) == 0) {
			info->quit = TRUE;
		} else if (strncmp(line, "uci", 3) == 0) {
			printf("id name %s\n", NAME);
			printf("id author Emil Ostensen\n");
			printf("uciok\n");
		}

		if(info->quit) {
			break;
		}
	}
	free(pos->PvTable->pTable);
}

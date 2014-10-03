#include "stdio.h"
#include "defs.h"
#include "string.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
	int depth = -1, movetime = -1, time = -1, inc = 0, movestogo = 30;
	char *strPtr = NULL;
	info->timeset = FALSE;
	info->depth = MAXDEPTH;

	if ((strPtr = strstr(line, "infinite"))) {
		info->timeset = FALSE;
	}
	if ((strPtr = strstr(line, "binc")) && pos->side == BLACK) {
		inc = atoi(strPtr + 5);
	}
	if ((strPtr = strstr(line, "winc")) && pos->side == WHITE) {
		inc = atoi(strPtr + 5);
	}
	if ((strPtr = strstr(line, "btime")) && pos->side == BLACK) {
		time = atoi(strPtr + 6);
	}
	if ((strPtr = strstr(line, "wtime")) && pos->side == WHITE) {
		time = atoi(strPtr + 6);
	}
	if ((strPtr = strstr(line, "movestogo"))) {
		movestogo = atoi(strPtr + 10);
	}
	if ((strPtr = strstr(line, "movetime"))) {
		movetime = atoi(strPtr + 9);
	}
	if ((strPtr = strstr(line, "depth"))) {
		depth = atoi(strPtr + 6);
	}

	info->starttime = GetTimeMs();

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}
	if(time != -1) {
		info->timeset = TRUE;
		time /= movestogo;
		time -= 50;
		info->stoptime = info->starttime + time + inc;
	}
	if(depth != -1) {
		info->depth = depth;
	}

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time,info->starttime,info->stoptime,info->depth,info->timeset);

	SearchPosition(pos, info);
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

	//if the GUI wants us to parse moves aswell
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

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
	//remove buffering so we don't miss commands from/to GUI
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char line[INPUTBUFFER];
	printf("id name %s\n", NAME);
	printf("id author Emil Ostensen\n");
	printf("uciok\n");

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
}

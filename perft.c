#include "defs.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"


U64 Perft(int depth, S_BOARD *pos) {
	ASSERT(checkBoard(pos));
 
 	S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    if(depth == 1) {
    	return list->count;
    }

    U64 nodes = ZERO64;
    int i;
    for (i = 0; i < list->count; i++) {
    	if (MakeMove(pos, list->moves[i].move)) {
      	  nodes += Perft(depth - 1, pos);
      	  TakeMove(pos);
    	}
    }
    
    return nodes;
}

void perftSuiteTest() {
	char const* const filename = "perftsuite.epd";
	FILE* file = fopen(filename, "r");

	if(file == NULL) {
		printf("didnt find file: %s", filename);
		return;
	}

	char line[256];
	char *fen;
	char *depthVal;

	U64 nodes;
	U64 perftResult;
	int depth;
	const char delim[2] = ";";

	S_BOARD board[1];

	while (fgets(line, sizeof(line), file)) {
	 	fen = strtok(line, delim);
	 	parseFEN(fen, board);


	 	depthVal = strtok(NULL, delim);

	 	depth = 1;
	 	nodes = ZERO64;

	 	while(depthVal != NULL) {
	 		nodes = atoi(depthVal+3);
	 		perftResult = Perft(depth, board);

	 		if(nodes != perftResult) {
	 			printBoard(board);
	 			printf("expected %I64d\nresult   %I64d \ndepth %d", nodes, perftResult, depth);
	 		}

	 		depthVal = strtok(NULL, delim);
	 		depth++;
	 	}

	 	//printBoard(board);
        printf("%s\n", fen); 
    }
}
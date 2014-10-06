#include "defs.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include <inttypes.h>


U64 Perft(int depth, S_BOARD *pos) {
	ASSERT(checkBoard(pos));
 
 	S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    if(depth == 0) {
    	return 1;
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

	 	printf("\n\nTesting position: \n");
	 	printBoard(board);


	 	depthVal = strtok(NULL, delim);

	 	depth = 1;
	 	nodes = ZERO64;

 		while(depthVal != NULL) {
 			printf("depth %d", depth);
	 		nodes = atoi(depthVal+3);
	 		perftResult = Perft(depth, board);

	 		if(nodes != perftResult) {
	 			printf("\nERROR: expected %"PRIu64"\nresult %"PRIu64"\ndepth %d\n", nodes, perftResult, depth);
	 		}
	 		else {
	 			printf(" ok\n");
	 		}


	 		depthVal = strtok(NULL, delim);
	 		depth++;
	 	}
    }
}

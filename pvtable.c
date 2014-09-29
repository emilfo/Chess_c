#include "stdio.h"
#include "defs.h"

//2 megabytes of size, should be set by gui
const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos) {
	ASSERT(depth < MAXDEPTH);

	int move = ProbePvTable(pos);
	int count = 0;

	while(move != NOMOVE && count < depth) {
		ASSERT(count < MAXDEPTH);

		if(MoveExists(pos, move)) {
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}
		move = ProbePvTable(pos);
	}

	//Taking back all moves that GetPvLine did
	while(pos->ply > 0) {
		TakeMove(pos);
	}

	return count;
}

void ClearPvTable(S_PVTABLE *table) {

  S_PVENTRY *pvEntry;

  for (pvEntry = table->pTable;
	  				pvEntry < table->pTable + table->numEntries; pvEntry++) {
    pvEntry->posKey = 0ULL;
    pvEntry->move = NOMOVE;
  }
}

void InitPvTable(S_PVTABLE *table) {  
    table->numEntries = PvSize / sizeof(S_PVENTRY);
    table->numEntries -= 2;
    if (table->pTable)
		free(table->pTable);
    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    ClearPvTable(table);
    printf("PvTable init complete with %d entries\n",table->numEntries);
}

void StorePvMove(const S_BOARD *pos, const int move) {
	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index < pos->PvTable->numEntries);

	pos->PvTable->pTable[index].move = move;
	pos->PvTable->pTable[index].posKey = pos->posKey;
}

int ProbePvTable(const S_BOARD *pos) {
	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);

	if (pos->PvTable->pTable[index].posKey == pos->posKey) {
		return pos->PvTable->pTable[index].move;
	}

	return NOMOVE;
}


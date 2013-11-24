#include "stdio.h"
#include "defs.h"

//2 megabytes of size, should be set by gui
const int PvSize = 0x100000 * 2;

void ClearPvTable(S_PVTABLE *table) {

  S_PVENTRY *pvEntry;

  for (pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
    pvEntry->posKey = 0ULL;
    pvEntry->move = NOMOVE;
  }
}

void InitPvTable(S_PVTABLE *table) {  
  
    table->numEntries = PvSize / sizeof(S_PVENTRY);
    table->numEntries -= 2;
    free(table->pTable);
    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    ClearPvTable(table);
    printf("PvTable init complete with %d entries\n",table->numEntries);
	
}
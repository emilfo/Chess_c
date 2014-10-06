#include "stdio.h"
#include "defs.h"

//16 megabytes of size, should be set by gui
const int HashSize = 0x100000 * 10;

int GetPvLine(const int depth, S_BOARD *pos) {
	ASSERT(depth < MAXDEPTH);

	int move = ProbePvMove(pos);
	int count = 0;

	while(move != NOMOVE && count < depth) {
		ASSERT(count < MAXDEPTH);

		if(MoveExists(pos, move)) {
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}
		move = ProbePvMove(pos);
	}

	//Taking back all moves that GetPvLine did
	while(pos->ply > 0) {
		TakeMove(pos);
	}

	return count;
}

void ClearHashTable(S_HASHTABLE *table) {
	int i;
	S_HASHENTRY *tableEntry = table->pTable;

	for (i = 0; i < table->numEntries; i++) {
		tableEntry->posKey = ZERO64;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flag = 0;
		tableEntry++;
	}
	table->newWrite = 0;
}

void InitHashTable(S_HASHTABLE *table) {
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;
	
    if (table->pTable) free(table->pTable);

    table->pTable = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
    ClearHashTable(table);
    printf("HashTable init complete with %d entries\n",table->numEntries);
}

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {
	int index = pos->posKey % pos->HashTable->numEntries;

	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
    ASSERT(depth>=1&&depth<MAXDEPTH);
    ASSERT(alpha<beta);
    ASSERT(alpha>=-INFINITE&&alpha<=INFINITE);
    ASSERT(beta>=-INFINITE&&beta<=INFINITE);
    ASSERT(pos->ply>=0&&pos->ply<MAXDEPTH);

	if (pos->HashTable->pTable[index].posKey == pos->posKey) {
		*move = pos->HashTable->pTable[index].move;

		if (pos->HashTable->pTable[index].depth >= depth) {
			pos->HashTable->hit++;

			ASSERT(pos->HashTable->pTable[index].depth >= 1
					&& pos->HashTable->pTable[index].depth<MAXDEPTH);
			ASSERT(pos->HashTable->pTable[index].flag >= HFALPHA
					&& pos->HashTable->pTable[index].flag <= HFEXACT);
		
			*score = pos->HashTable->pTable[index].score;
			//reset the mate score
			if (*score > ISMATE) {
				score -= pos->ply;
			} else if (*score < -ISMATE) {
				score += pos->ply;
			}

			switch (pos->HashTable->pTable[index].flag) {
				ASSERT(*score>=-INFINITE && *score<=INFINITE);
				ASSERT(alpha>=-INFINITE && alpha<=INFINITE);
				ASSERT(beta>=-INFINITE && beta<=INFINITE);
				case HFALPHA: 
					if(*score <= alpha) {
						*score = alpha;
						return TRUE;
					}
					break;
				case HFBETA:
					if(*score >= beta) {
						*score = beta;
						return TRUE;
					}
					break;
				case HFEXACT:
					return TRUE;
					break;
				default:
					ASSERT(FALSE);
					break;
			}
		}
	}
	return FALSE;
}





void StoreHashEntry(S_BOARD *pos, const int move, int score,
		const int flags, const int depth) {

	int index = pos->posKey % pos->HashTable->numEntries;

	ASSERT(index >= 0 && index < pos->HashTable->numEntries);
	ASSERT(depth>=1 && depth<MAXDEPTH);
    ASSERT(flags>=HFALPHA && flags<=HFEXACT);
    ASSERT(score>=-INFINITE && score<=INFINITE);
    ASSERT(pos->ply >= 0 && pos->ply<MAXDEPTH);

	if( pos->HashTable->pTable[index].posKey == 0 ) {
		pos->HashTable->newWrite++;
	} else {
		pos->HashTable->overWrite++;
	}

	if (score > ISMATE) {
		score -= pos->ply;
	} else if (score < -ISMATE) {
		score += pos->ply;
	}

	pos->HashTable->pTable[index].move = move;
	pos->HashTable->pTable[index].posKey = pos->posKey;
	pos->HashTable->pTable[index].flag = flags;
	pos->HashTable->pTable[index].score = score;
	pos->HashTable->pTable[index].depth = depth;
}

int ProbePvMove(const S_BOARD *pos) {
	int index = pos->posKey % pos->HashTable->numEntries;
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

	if (pos->HashTable->pTable[index].posKey == pos->posKey) {
		return pos->HashTable->pTable[index].move;
	}

	return NOMOVE;
}


#include "defs.h"
int SQ120toSQ64[BRD_SQ_NUM];
int SQ64toSQ120[64];

U64 SetMask[64];
U64 ClearMask[64];

void initBitMask() {
	int i = 0;

	//init value of 0ULL
	for(i = 0; i < 64; i++) {
		SetMask[i] = 0ULL;
		ClearMask[i] = 0ULL;
	}

	for(i = 0; i < 64; i++) {
		SetMask[i] |= (1ULL << i);
		ClearMask[i] = ~SetMask[i];
	}


}

void InitSQ120toSQ64 () {
	int i = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;
	int sq64 = 0;

	/*Loop that runs through and sets up the arrays which translates between the
	board of size 64, and the board of size 120, initialize 65 to invalid 120 sq*/
	for(i = 0; i < BRD_SQ_NUM; i++) {
		SQ120toSQ64[i] = 65;
	}

	for (rank = RANK_1; rank <= RANK_8; rank++) {
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = FRtoSQ(file, rank);
			SQ64toSQ120[sq64] = sq;
			SQ120toSQ64[sq] = sq64;
			sq64++;
		}
	}


}

void AllInit () {
	InitSQ120toSQ64();

}


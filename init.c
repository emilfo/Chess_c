#include "defs.h"
#include "stdio.h"

int SQ120toSQ64[BRD_SQ_NUM];
int SQ64toSQ120[64];
int GetFile[BRD_SQ_NUM];
int GetRank[BRD_SQ_NUM];

U64 SetMask[64];
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

//used to make a random number used for the hashkey'ing
#define RAND_64 (	(U64)rand() | \
					(U64)rand() << 15 | \
					(U64)rand() << 30 | \
					(U64)rand() << 45 | \
					((U64)rand() & 0xf) << 60)
					



void InitGetFileRank() {
	int rank;
	int file;
	int sq;

	for (sq = 0; sq<BRD_SQ_NUM; sq++) {
		GetFile[sq] = OFFBOARD;
		GetRank[sq] = OFFBOARD;
	}

	for (rank = RANK_1; rank <=RANK_8; rank++) {
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = FRtoSQ(file,rank);
			GetFile[sq] = file;
			GetRank[sq] = rank;
		}
	}
}
void InitHashKeys() {
	
	int index = 0;
	int index2 = 0;
	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < 120; ++index2) {
			PieceKeys[index][index2] = RAND_64;
		}
	}
	SideKey = RAND_64;
	for(index = 0; index < 16; ++index) {
		CastleKeys[index] = RAND_64;
	}

}

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
	initBitMask();
	InitHashKeys();
	InitGetFileRank();

}


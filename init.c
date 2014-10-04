#include "defs.h"
#include "stdio.h"

//used to make a random number used for the hashkey'ing
#define RAND_64 (	(U64)rand() | \
					(U64)rand() << 15 | \
					(U64)rand() << 30 | \
					(U64)rand() << 45 | \
					((U64)rand() & 0xf) << 60)

int SQ120toSQ64[BRD_SQ_NUM];
int SQ64toSQ120[64];
int GetFile[BRD_SQ_NUM];
int GetRank[BRD_SQ_NUM];

U64 SetMask[64];
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

U64 FileBBMask[8];
U64 RankBBMask[8];

U64 BlackPassedMask[64];
U64 WhitePassedMask[64];
U64 IsolatedMask[64];
					

void InitEvalMasks() {
	int sq, tsq, r, f;

	for (sq = 0; sq < 8; sq++) {
		FileBBMask[sq] = ZERO64;
		RankBBMask[sq] = ZERO64;
	}

	for (r = RANK_8; r >= RANK_1; r--) {
		for (f = FILE_A; f <= FILE_H; f++) {
			sq = (r * 8) + f;
			FileBBMask[f] |= (ONE64 << sq);
			RankBBMask[r] |= (ONE64 << sq);
		}
	}

	for (sq = 0; sq < 64; sq++) {
		IsolatedMask[sq] = ZERO64;
		WhitePassedMask[sq] = ZERO64;
		BlackPassedMask[sq] = ZERO64;
	}


	for (sq = 0; sq < 64; sq++) {

		tsq = sq + 8;
		while (tsq < 64) {
			WhitePassedMask[sq] |= (ONE64 << tsq);
			tsq += 8;
		}


		tsq = sq - 8;
		while (tsq >= 0) {
			BlackPassedMask[sq] |= (ONE64 << tsq);
			tsq -= 8;
		}

		if(GetFile[SQ120(sq)] > FILE_A) {
			IsolatedMask[sq] |= FileBBMask[GetFile[SQ120(sq)] - 1];

			tsq = sq + 7;
			while (tsq < 64) {
				WhitePassedMask[sq] |= (ONE64 << tsq);
				tsq += 8;
			}

			tsq = sq - 9;
			while (tsq >= 0) {
				BlackPassedMask[sq] |= (ONE64 << tsq);
				tsq -= 8;
			}
		}

		if(GetFile[SQ120(sq)] < FILE_H) {
			IsolatedMask[sq] |= FileBBMask[GetFile[SQ120(sq)] + 1];

			tsq = sq + 9;
			while (tsq < 64) {
				WhitePassedMask[sq] |= (ONE64 << tsq);
				tsq += 8;
			}

			tsq = sq - 7;
			while (tsq >= 0) {
				BlackPassedMask[sq] |= (ONE64 << tsq);
				tsq -= 8;
			}
		}

	}
}

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

void InitBitMask() {
	int i = 0;

	//init value of ZERO64
	for(i = 0; i < 64; i++) {
		SetMask[i] = ZERO64;
		ClearMask[i] = ZERO64;
	}

	for(i = 0; i < 64; i++) {
		SetMask[i] |= (ONE64 << i);
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
	InitBitMask();
	InitHashKeys();
	InitGetFileRank();
	InitEvalMasks();
	InitMvvLva();
}


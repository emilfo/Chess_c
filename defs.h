#ifndef DEFS_H
#define DEFS_H

#include "stdint.h"
#include "stdlib.h"

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
# define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed ", #n); \
printf("In File %s ", __FILE__); \
printf("At Line %d \n", __LINE__); \
exit(1); }
#endif

typedef uint64_t U64;

#define NAME  "ELOmif 0.1"
#define BRD_SQ_NUM 120
#define MAXGAMEMOVES 2048
#define ZERO64 UINT64_C(0)
#define ONE64 UINT64_C(1)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE, };

enum { WHITE, BLACK, BOTH };
enum { 
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum { FALSE, TRUE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {
	int move;
	int score;
}S_MOVE;

typedef struct 
{
	int move;
	int castlePerm;
	int enPasssantSQ;
	int fiftyMove;
	U64 posKey;

} S_UNDO;

typedef struct 
{
	int pieces [BRD_SQ_NUM];
	U64 pawns[3];
	/*U64 rooks[3];
	U64 knights[3];
	U64 bishops[3];
	U64 kings[3];
	U64 Queens[3];*/

	int kingSQ[2];

	int side;
	int enPassantSQ;
	int fiftyMove;

	int castlePerm;

	int ply;
	int hisPly;

	U64 posKey;

	int pceNum[13];
	int bigPce[2];
	int majPce[2];
	int minPce[2];
	int material[2];

	S_UNDO history [MAXGAMEMOVES];

	//piece list
	int pList[13][10];

} S_BOARD;

/* INT MOVE */
/*   
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/
#define FROM(m) ((m) & 0x7F)
#define TO(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFALGPROM 0xF00000


/* MACROS */

#define FRtoSQ(file,rank) ( ( 21+(file) )+( 10*(rank) ) )
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define SQ64(sq120) (SQ120toSQ64[(sq120)])
#define SQ120(sq64) (SQ64toSQ120[(sq64)])
#define CLRBIT(bb,sq) ((bb) &= ClearMsk[(sq)])
#define SETBIT(bb,sq) ((bb) |= SetMask[(sq)])

#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])

/* GLOBALS */

extern int SQ120toSQ64[BRD_SQ_NUM];
extern int SQ64toSQ120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][BRD_SQ_NUM];
extern U64 SideKey;
extern U64 CastleKeys[16];
extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceColor[13];
extern int PieceVal[13];

extern int GetRank[BRD_SQ_NUM];
extern int GetFile[BRD_SQ_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];



/* FUNCTIONS */

//init.c
extern void AllInit();

// bitboards.c
extern void printBitBoard (U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);

//board.c
extern void resetBoard(S_BOARD *pos);
extern int parseFEN(char *fen, S_BOARD *pos);
extern void printBoard (const S_BOARD *pos);
extern void UpdateListsMaterial (S_BOARD *pos);
extern int checkBoard(const S_BOARD *pos);

//hashkey.c
U64 GeneratePosKey(const S_BOARD *pos);

//attack.c
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);

//io.c
extern char *printSquare(const int sq);
extern char *printMove(const int move);

#endif
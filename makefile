all:
	gcc chess.c init.c bitboards.c board.c hashkeys.c data.c validate.c attack.c io.c movegen.c makemove.c perft.c search.c misc.c pvtable.c evaluate.c -o ELOmif

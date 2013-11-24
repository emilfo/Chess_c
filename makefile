all:
	gcc chess.c init.c bitboards.c board.c hashkeys.c data.c attack.c io.c movegen.c makemove.c validate.c perft.c -o ELOmif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "maps.h"

typedef enum {
	FALSE, TRUE
} boolean;

int MAX_LEVEL = 4;

#define MAKE_NEW_BOARD_ITER_

// Authors
// Andreas Øverland
// Eva Solfrid Øverland
// Fredrik Christian Øverland.
// Sara Franses Gläser
// Ines Christiane Øverland
// Andreas Ehfihgithiuhurhfrhdjfurhtghrghtughrjkhg Øverland
// Oma Ingri Gisnås Øverland
// Opa Bjørnar Øverland

// TODO:
// nbin-text

void printDiagram(unsigned long *board);

void printNumBoard(unsigned long *board);

unsigned long printStats();

void count(const unsigned long *board);

// bitboard functions, highly speed sensitive
void makeNewBoard(const unsigned long *oldBoard, unsigned long *newBoard);
void makeNewBoardIter();

int calculateWhiteKingCheckStatus(const unsigned long *board);
int calculateBlackKingCheckStatus(const unsigned long *board);

unsigned long calculateWhiteKingCheckStatus2(unsigned long *board, unsigned long lastMoveMap);
unsigned long calculateBlackKingCheckStatus2(unsigned long *board, unsigned long lastMoveMap);

int findAllPossibleMoves2();
int moveWhitePawns();
int moveBlackPawns();
int moveWhiteRooksOrQueens(int pieceMapIndex);
int moveWhiteKnights();
int moveWhiteBishopsOrQueens(int pieceMapIndex);
int moveWhiteKing();
int moveBlackRooksOrQueens(int pieceMapIndex);
int moveBlackKnights();
int moveBlackBishopsOrQueens(int pieceMapIndex);
int moveBlackKing();

void makeWhiteMove(unsigned long *move, int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long blackPieces);

void makeBlackMove(unsigned long *move, int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long whitePieces);

void clearBlackPiecesWithClearMap(unsigned long *board, unsigned long clear);
void clearWhitePiecesWithClearMap(unsigned long *board, unsigned long clear);
void makeWhiteBitPromos(unsigned long *board, unsigned long map);
void makeBlackBitPromos(unsigned long *board, unsigned long map);

// util for board printing and conversion.
void setBitsToChar(char *str, unsigned long bits, char c);
void sprintDiagram(char *target, unsigned long *board);
void printBitBoard(unsigned long *board);
void diagramToBitBoard(unsigned long *board, char diagram[]);
void printLongAsBitBoard(unsigned long bitstream);
void compressBitBoard(unsigned long *board);

void bitBoardToNibbleBinary(const unsigned long *b, unsigned char nibbleBinary[]);
void nibbleBinaryToBitBoard(unsigned char nibbleBinary[], unsigned long *board);

/*** LEVEL ***/
/*** LEVEL ***/
/*** LEVEL ***/

int LOG_TYPE = LOG_TYPE_NONE;
unsigned long MULTIPLIER = 1;
unsigned long numMoves[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numCaptures[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numEP[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numCastles[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numPromos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numCheckmates[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numStalemates[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDiscoveryChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDiscoveryPromoChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDiscoveryCaptureChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDiscoveryEPChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDoubleChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDoublePromoChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDoubleCaptureChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long numDoubleEPChecks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned long makeNewBoardInvocations = 0;
unsigned long isSquaresThreatenedByColorInvocations = 0;
unsigned long influenceMapForSquareInvocations = 0;
unsigned long moveLinearInvocations = 0;

int maxNumMoves = 0;

char *workUnitId = NULL;
FILE *outFile = NULL;
FILE *matesOutFile = NULL;
FILE *inFile = NULL;

int outFileBuffOffset = 0;
const int outFileBuffSize = 1024 * 1024;
char outFileBuff[outFileBuffSize];

int matesOutFileBuffOffset = 0;
char matesOutFileBuff[outFileBuffSize];

int buffWrites = 0;
int fileWrites = 0;

// Non recursive variant
unsigned long boards[28 * 30 * 100]; // 28 longs per board, 30 boards per level, 10 levels deep max
int currentBoardIdx = 0;
int nextBoardIdx = 1;

int main(int argc, char **argv) {

	for (int i = 0; i < 28 * 30 * 100; i++) {
		boards[i] = 0l;
	}


	// "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR"

	char *initialBoard = "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR w KQkq -";
	//char *initialBoard = "k....... ........ ........ ........ ........ ........ ...Q...K w -";

	//*

	// ADA Chess FEN test
	// 7k/8/8/8/6p1/5p1r/7n/7K
	// ".......k ........ ........ ........ ......p. .....p.r .......n .......K b - - 0"

	// Position 4
	// r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1
	//char *initialBoard = "r..q.rk. pP.p..pp Q....n.. bbp.p... Np...... .B...NBn pPPP.PPP R...K..R b kq - 1";



/*    char *initialBoard = "\
                      r . . . k . . r\
                      p . p p q p b .\
                      b n . . p n p .\
                      . . . P N . . .\
                      . p . . P . . .\
                      . . N . . Q . p\
                      P P P B B P P P\
                      R . . . K . . R\
                      w KQkq -";
*/
/*
	char *initialBoard = "\
					  . . . . k . . .\
					  . . . . . . . .\
					  . . . . . . . .\
					  . . . . . . . .\
					  . . P p . . . .\
					  . . . . . . . .\
					  . . . . . . . .\
					  . . . . . . K .\
					  b - c3 11";
*/


	unsigned long board[NUM_BYTES];

	diagramToBitBoard(board, initialBoard);

	if (argc > 1) {
		if (strcmp(argv[1], "SLEEP") == 0) {
			printf("SLEEPING");
			fflush(stdout);
			sleep(60);
			return 0;
		}
		else if (strcmp(argv[1], "-h") == 0) {
			printf("** Borgsum Chess Engine (fast, but stupid version) **\r\n");
			printf("Usage:\r\n");
			printf("chessengine [arguments]\r\n");
			printf("Arguments:\r\n");
			printf("No arguments       The chessengine will run once with the standard starting position, to a level of 5 deep, then print statistics.\r\n");
			printf("-diagram \"string\"  A human readable form of the board enclosed in \"'s\r\n");
			printf("                   Examples:\r\n");
			printf("                   \"rnbqkbnr .ppppppp ........ p....... ........ .......P PPPPPPP. RNBQKBNR w KQkq a6 2\"\r\n");
			printf("                   \"r....rk. p.ppqpb. bn..pnN. ...P.... .p..P... ..N..Q.p PPPBBPPP R....RK. b - - 3\"\r\n");
			printf("                   \"r....r.. p..pqpbk bn..pnN. ..pP.... .p..P... ..N..Q.p PPPBBPPP R....R.K w - b5 6\"\r\n");
			printf("                   Each row, beginning with black pieces on a8, then followed by turn w or b, followed by\r\n");
			printf("                   castling rights in any combo of KQkq, or - for none, followed by en passant square if the last move\r\n");
			printf("                   was a pawn being moved to squares. The en passant square is the attackable square behind the moved pawn.\r\n");
			printf("                   The last number is the move number, or ply really. \r\n");
			printf("                   Spaces are ignored, and dots are treated as empty squares. \r\n");
			printf("-maxlevel N        Max recursion level, defaults to 5.\r\n");
			printf("-workunitid ID     Handy for distributing workunits. Only used when printing the statistics.\r\n");
			printf("-logtype TYPE      Either 'nbin' or 'diagram'. Will print out each new board found as 'nibble binary' or 'diagram' format.\r\n");
			printf("                   If omitted, only the statistics will be printed at the end of the run, without logging each found board.\r\n");
			printf("                   The format nbin has a fixed length of 33 bytes, nice for sorting and finding duplicated for later runs with the -infile argument.\r\n");
			printf("SLEEP              The program will sleep for 60 seconds. Useful when running as a distributed client.\r\n");

			printf("\r\n\r\nandreasoverland@gmail.com\r\n\r\n");
			fflush(stdout);
			exit(0);
		}

	}

	for (int a = 1; a < argc; a++) {

		if (strcmp(argv[a], "-infile") == 0) {
			a++;

			inFile = fopen(argv[a], "rb");
			if (inFile == NULL) {
				printf("** ERORR : File not found '%s' **\r\n", argv[a]);
				exit(EXIT_FAILURE);
			}

		}

		if (strcmp(argv[a], "-outfile") == 0) {
			a++;
			outFile = fopen(argv[a], "wb");
			matesOutFile = fopen("mates.out", "wb");

		}

		if (strcmp(argv[a], "-diagram") == 0) {
			a++;
			diagramToBitBoard(board, argv[a]);
		}

		if (strcmp(argv[a], "-maxlevel") == 0) {
			a++;
			MAX_LEVEL = atoi(argv[a]);
		}

		if (strcmp(argv[a], "-workunitid") == 0) {
			a++;
			workUnitId = argv[a];
		}

		if (strcmp(argv[a], "-logtype") == 0) {
			a++;
			if (strcmp(argv[a], "diagram") == 0) {
				LOG_TYPE = LOG_TYPE_DIAGRAM;
			}
			else if (strcmp(argv[a], "nbin") == 0) {
				LOG_TYPE = LOG_TYPE_NIBBLE_BINARY;
			}

		}
		if (strcmp(argv[a], "-mul") == 0) {
			a++;
			MULTIPLIER = atoi(argv[a]);
		}

	}

	if (argc == 1) {
		printf("run this with arguments :\r\n");
		char line[120];
		sprintDiagram(line, board);
		printf("./chessengine -diagram %s -maxlevel %d\n", line, MAX_LEVEL);
	}


	char *line = NULL;
	size_t len = 0;
	int read;

	// nonrec : move initial board into stack
	memcpy(boards, board, sizeof(unsigned long) * NUM_BYTES);
	currentBoardIdx = 0;

	struct timespec ts1, ts2;
	clock_gettime(CLOCK_REALTIME, &ts1);
	int boardNum = 0;

	// ################# LOOP ###########################################################################
	// ################# LOOP ###########################################################################
	// ################# LOOP ###########################################################################
	// ################# LOOP ###########################################################################

	printf("Starting loop\r\n");
	unsigned long numMovesFound = 0;
	int maxBoardsInBuffer = 0;
	printBitBoard(boards);
	count(boards);
	do {
		if (boards[currentBoardIdx * NUM_BYTES + IDX_MOVE_NUM] < MAX_LEVEL &&
			boards[currentBoardIdx * NUM_BYTES + IDX_FINISHED] == 0) {
			numMovesFound += findAllPossibleMoves2(); // <- changes currentBoardIdx
		}
		else {
			currentBoardIdx--;
		}
		if (nextBoardIdx > maxBoardsInBuffer) {
			maxBoardsInBuffer = nextBoardIdx;
		}
		nextBoardIdx = currentBoardIdx + 1;
	} while (currentBoardIdx != 0);

	printf("Num Moves Found %lu\r\n", numMovesFound);
	printf("Max Boards In Buffer %d\r\n", maxBoardsInBuffer);
	printf("Num new board invocations: %lu\r\n", makeNewBoardInvocations);

	clock_gettime(CLOCK_REALTIME, &ts2);
	if (ts2.tv_nsec < ts1.tv_nsec) {
		ts2.tv_nsec += 1000000000;
		ts2.tv_sec--;
	}

	if (line) {
		free(line);
	}
	if (inFile != NULL) {
		fclose(inFile);
	}


	int l = 14;

	if (workUnitId != NULL && LOG_TYPE != LOG_TYPE_CFEN) {
		printf("# WORKUNITID: %s\r\n", workUnitId);
	}

	if (LOG_TYPE != LOG_TYPE_CFEN) {

		long total = printStats();
		printf("\n# Total valid moves found : %lu \n", total);
		printf("# Time spent: %ld.%09ld \n", (long) (ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);
		if (MULTIPLIER != 0) {
			printf("# multiplier %lu\n", MULTIPLIER);
		}
	}

	if (outFile != NULL) {

		if (LOG_TYPE == LOG_TYPE_BINARY || LOG_TYPE == LOG_TYPE_COMP_BINARY || LOG_TYPE == LOG_TYPE_NIBBLE_BINARY ||
			LOG_TYPE == LOG_TYPE_CFEN) {
			if (outFileBuffOffset != 0) {
				fwrite(outFileBuff, 1, outFileBuffOffset, outFile);
				outFileBuffOffset = 0;
				fileWrites++;
			}
			fclose(outFile);
			if (matesOutFileBuffOffset != 0) {
				fwrite(matesOutFileBuff, 1, matesOutFileBuffOffset, matesOutFile);
				matesOutFileBuffOffset = 0;
			}
			fclose(matesOutFile);
		}
	}


	return 0;
} // end main

unsigned long printStats() {


	static unsigned long total = 0;

	for (int t = 0; t < MAX_LEVEL + 4; t++) {
		printf("# %d", t);
		printf(", N: %lu", numMoves[t]);
		printf(", Caps: %lu", numCaptures[t]);
		printf(", EP: %lu", numEP[t]);
		printf(", Cast: %lu", numCastles[t]);
		printf(", Prom: %lu", numPromos[t]);
		printf(", Checks: %lu", numChecks[t]);
		printf(", Disc: %lu", numDiscoveryChecks[t]);
		printf(", Disc(promo): %lu", numDiscoveryPromoChecks[t]);
		printf(", Disc(capture): %lu", numDiscoveryCaptureChecks[t]);
		printf(", Disc(ep): %lu", numDiscoveryEPChecks[t]);
		printf(", Dbl: %lu", numDoubleChecks[t]);
		printf(", Dbl(promo) : %lu", numDoublePromoChecks[t]);
		printf(", Dbl(capture) : %lu", numDoubleCaptureChecks[t]);
		printf(", Dbl(ep) : %lu", numDoubleEPChecks[t]);
		printf(", Mates: %lu", numCheckmates[t]);
		printf(", S.Mates: %lu\n", numStalemates[t]);

		total += numMoves[t];
	}

	printf("\n");
	fflush(stdout);
	return total;

}

/************************************************************************************************************
 **																										   **
 **												 ENGINE												       **
 **																										   **
 ************************************************************************************************************/

unsigned long lastCountedMoveNum = 0;
unsigned long lastCountedMoveNumCounted = 0;

int findAllPossibleMoves2() {

	unsigned long numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES; // eller << 16 + << 8 + << 4
	unsigned long *board = boards + offset;

	if (board[IDX_TURN] == WHITE_MASK) { // turn == white

		numMovesFound += moveWhitePawns();
		numMovesFound += moveWhiteKnights();
		numMovesFound += moveWhiteRooksOrQueens(IDX_WHITE_QUEENS);
		numMovesFound += moveWhiteRooksOrQueens(IDX_WHITE_ROOKS);
		numMovesFound += moveWhiteBishopsOrQueens(IDX_WHITE_QUEENS);
		numMovesFound += moveWhiteBishopsOrQueens(IDX_WHITE_BISHOPS);
		numMovesFound += moveWhiteKing();

	}
	else {

		numMovesFound += moveBlackPawns();
		numMovesFound += moveBlackKnights();
		numMovesFound += moveBlackRooksOrQueens(IDX_BLACK_QUEENS);
		numMovesFound += moveBlackRooksOrQueens(IDX_BLACK_ROOKS);
		numMovesFound += moveBlackBishopsOrQueens(IDX_BLACK_QUEENS);
		numMovesFound += moveBlackBishopsOrQueens(IDX_BLACK_BISHOPS);
		numMovesFound += moveBlackKing();

	}

	board[IDX_FINISHED] = 1;
	currentBoardIdx += numMovesFound;

	return numMovesFound;
}


// ----------------------------------- WHITE KING ---------------------------------

int moveWhiteKing() {

	int numMovesFound = 0;

	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieceMap = b[IDX_WHITE_KING];

	int idx = __builtin_ctzll(pieceMap);

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	unsigned long clearMap = ~pieceMap;
	unsigned long moveToMaps = KING_ATTACK_MAPS[idx];

	moveToMaps &= ~whitePieces;
	int moveToIdx = 0;

	while (moveToMaps) {

		unsigned long moveToShift = __builtin_ctzll(moveToMaps);
		moveToIdx += moveToShift;

		unsigned long moveToMap = 1ul << moveToIdx;

		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif
		makeWhiteMove(move, IDX_WHITE_KING, moveToMap, clearMap, blackPieces);
		move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_KING_SIDE | MASK_CASTLING_WHITE_QUEEN_SIDE);

		if (calculateWhiteKingCheckStatus(move) == 0) {
			calculateBlackKingCheckStatus2(move, moveToMap);
			count(move);
			numMovesFound++;
			nextBoardIdx++;
		}

		moveToMaps >>= moveToShift;
		moveToMaps >>= 1ul;
		moveToIdx++;

	}

	if ((b[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE) {
		if (!(B1_C1_D1_MASK & allPieces)) {
			b[IDX_WHITE_KING] = C1_MASK; // King will end up on this square
			if (!calculateWhiteKingCheckStatus(b)) {
				b[IDX_WHITE_KING] = D1_MASK; // King will move past this suare
				if (!calculateWhiteKingCheckStatus(b)) {
					b[IDX_WHITE_KING] = pieceMap; // king moves from this square
					if (!calculateWhiteKingCheckStatus(b)) {

						unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
						makeNewBoardIter();
#else
						makeNewBoard(b, move);
#endif
						makeWhiteMove(move, IDX_WHITE_KING, C1_MASK, clearMap, blackPieces);
						makeWhiteMove(move, IDX_WHITE_ROOKS, D1_MASK, ~A1_MASK, blackPieces);
						calculateBlackKingCheckStatus2(move, C1_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
						count(move);
						numMovesFound++;
						nextBoardIdx++;
					}
				}
			}
			b[IDX_WHITE_KING] = pieceMap;
		}
	}

	if ((b[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE) {
		if (!(F1_G1_MASK & allPieces)) {
			b[IDX_WHITE_KING] = F1_MASK; // king is moving past this spot
			if (!calculateWhiteKingCheckStatus(b)) {
				b[IDX_WHITE_KING] = G1_MASK; // king is landing on this spot
				if (!calculateWhiteKingCheckStatus(b)) {
					b[IDX_WHITE_KING] = pieceMap; // king moves from
					if (!calculateWhiteKingCheckStatus(b)) {
						unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
						makeNewBoardIter();
#else
						makeNewBoard(b, move);
#endif
						makeWhiteMove(move, IDX_WHITE_KING, G1_MASK, clearMap, blackPieces);
						makeWhiteMove(move, IDX_WHITE_ROOKS, F1_MASK, ~H1_MASK, blackPieces);
						calculateBlackKingCheckStatus2(move, G1_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
						count(move);
						numMovesFound++;
						nextBoardIdx++;
					}
				}
			}
			b[IDX_WHITE_KING] = pieceMap;
		}
	}

	return numMovesFound;

}


// ----------------------------------- BLACK KING ---------------------------------

int moveBlackKing() {

	int numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieceMap = b[IDX_BLACK_KING];

	int idx = __builtin_ctzll(pieceMap);

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	unsigned long clearMap = ~pieceMap;
	unsigned long moveToMaps = KING_ATTACK_MAPS[idx];

	moveToMaps &= ~blackPieces;
	int moveToIdx = 0;

	while (moveToMaps) {

		int moveToShift = __builtin_ctzll(moveToMaps);
		moveToIdx += moveToShift;

		unsigned long moveToMap = 1l << moveToIdx;

		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif
		makeBlackMove(move, IDX_BLACK_KING, moveToMap, clearMap, whitePieces);
		move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_KING_SIDE | MASK_CASTLING_BLACK_QUEEN_SIDE);

		if (calculateBlackKingCheckStatus(move) == 0) {
			calculateWhiteKingCheckStatus2(move, moveToMap);
			count(move);
			nextBoardIdx++;
			numMovesFound++;
		}

		moveToMaps >>= moveToShift;
		moveToMaps >>= 1;
		moveToIdx++;

	}

	if ((b[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE) {
		if (!(B8_C8_D8_MASK & allPieces)) {
			b[IDX_BLACK_KING] = C8_MASK; // King will end up on this square
			if (!calculateBlackKingCheckStatus(b)) {
				b[IDX_BLACK_KING] = D8_MASK; // King will move past this suare
				if (!calculateBlackKingCheckStatus(b)) {
					b[IDX_BLACK_KING] = pieceMap; // king moves from this square
					if (!calculateBlackKingCheckStatus(b)) {
						unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
						makeNewBoardIter();
#else
						makeNewBoard(b, move);
#endif

						makeBlackMove(move, IDX_BLACK_KING, C8_MASK, clearMap, whitePieces);
						makeBlackMove(move, IDX_BLACK_ROOKS, D8_MASK, ~A8_MASK, whitePieces);
						calculateWhiteKingCheckStatus2(move, C8_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
						count(move);
						numMovesFound++;
						nextBoardIdx++;
					}
				}
			}
			b[IDX_BLACK_KING] = pieceMap;
		}
	}

	if ((b[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE) {
		if (!(F8_G8_MASK & allPieces)) {
			b[IDX_BLACK_KING] = F8_MASK; // king is moving past this spot
			if (!calculateBlackKingCheckStatus(b)) {
				b[IDX_BLACK_KING] = G8_MASK; // king is landing on this spot
				if (!calculateBlackKingCheckStatus(b)) {
					b[IDX_BLACK_KING] = pieceMap; // king is moving from this spot
					if (!calculateBlackKingCheckStatus(b)) {
						unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
						makeNewBoardIter();
#else
						makeNewBoard(b, move);
#endif
						makeBlackMove(move, IDX_BLACK_KING, G8_MASK, clearMap, whitePieces);
						makeBlackMove(move, IDX_BLACK_ROOKS, F8_MASK, ~H8_MASK, whitePieces);
						calculateWhiteKingCheckStatus2(move, G8_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
						count(move);
						numMovesFound++;
						nextBoardIdx++;
					}
				}
			}
			b[IDX_BLACK_KING] = pieceMap;
		}

	}

	return numMovesFound;

}


//////////////////////////////////// MOVE WHITE PAWN //////////////
int moveWhitePawns() {

	int numPawnMoves = 0;
	const unsigned long offset = currentBoardIdx * NUM_BYTES;
	const unsigned long *b = boards + offset;
	unsigned long pawns = b[IDX_WHITE_PAWNS];
	const unsigned long originalPawns = pawns;
	unsigned long blackPieces = b[IDX_BLACK_PIECES];

	// 1. Make moves for pawns that can move ONE forward
	// 2. Make moves for pawns that can move TWO forward and set EP index/map
	// 3. Make capturing moves for pawns with WHITE_PAWN_ATTACK_MAPS
	// 4. Make en passant captures for pawns with WHITE_PAWN_ATTACK_MAPS that matches EP Index/map

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long pawnsThatCanMoveOneForward = pawns & ~(allPieces >> 8ul);
	unsigned long pawnsThatCanMoveTwoForward = pawnsThatCanMoveOneForward & R2;
	pawnsThatCanMoveTwoForward &= ~(allPieces >> 16ul);

	unsigned long pieceIdx = 0;

	while (pawnsThatCanMoveOneForward) {
		int shift = __builtin_ctzll(pawnsThatCanMoveOneForward);
		pieceIdx += shift;
		unsigned long pieceMap = 1ul << pieceIdx;
		unsigned long moveToMap = pieceMap << 8ul;
		unsigned long clearMap = ~pieceMap;
		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif

		makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);
		// printBitBoard(move);
		if (calculateWhiteKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {

			if (moveToMap & R8) {
				move[IDX_CASTLING] &= ~moveToMap;
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO;
				numPawnMoves += 4;
				makeWhiteBitPromos(move, moveToMap);
			}
			else {
				calculateBlackKingCheckStatus2(move, moveToMap);
				numPawnMoves++;
                count(move);
                nextBoardIdx++;
			}


		}

		pawnsThatCanMoveOneForward >>= shift;
		pawnsThatCanMoveOneForward >>= 1;
		pieceIdx++;
	}

	pieceIdx = 0;
	while (pawnsThatCanMoveTwoForward) {
		int shift = __builtin_ctzll(pawnsThatCanMoveTwoForward);
		pieceIdx += shift;
		unsigned long pieceMap = 1l << pieceIdx;
		unsigned long moveToMap = pieceMap << 16;
		unsigned long clearMap = ~pieceMap;
		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif

		makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);
		move[IDX_EP_IDX] = pieceMap << 8;
		// printBitBoard(move);

		if (calculateWhiteKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {
			calculateBlackKingCheckStatus2(move, moveToMap);
			// The board currently on the top of the stack, is the next legal move to be expolored, unless maxleve is hit
			count(move);
			nextBoardIdx++;
			numPawnMoves++;
		}

		pawnsThatCanMoveTwoForward >>= shift;
		pawnsThatCanMoveTwoForward >>= 1;
		pieceIdx++;

	}

	pieceIdx = 0;
	while (pawns) {
		int shift = __builtin_ctzll(pawns);
		pieceIdx += shift;

		unsigned long attackMap = WHITE_PAWN_ATTACK_MAPS[pieceIdx];

		if (blackPieces & attackMap) {

			unsigned long pieceMap = 1l << pieceIdx;
			unsigned long validAttacks = blackPieces & attackMap;
			int attackIndex = 0;

			while (validAttacks) {
				int attackShift = __builtin_ctzll(validAttacks);
				attackIndex += attackShift;

				unsigned long moveToMap = 1L << attackIndex;
				unsigned long clearMap = ~pieceMap;
				unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
				makeNewBoardIter();
#else
				makeNewBoard(b, move);
#endif

				makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);
				//printBitBoard(move);

				if (calculateWhiteKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {

					if (moveToMap & R8) {
						move[IDX_CASTLING] &= ~moveToMap;
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO | MASK_LAST_MOVE_WAS_CAPTURE;
						numPawnMoves += 4;
						makeWhiteBitPromos(move, moveToMap);
					}
					else {
						calculateBlackKingCheckStatus2(move, moveToMap);
						numPawnMoves++;
					}

					count(move);
					nextBoardIdx++;

				}
				validAttacks >>= attackShift;
				validAttacks >>= 1;
				attackIndex++;
			}
		}
		pawns >>= shift;
		pawns >>= 1;
		pieceIdx++;
	}

	if (b[IDX_EP_IDX] != 0) {
		// ep index is between A6 and H6, ie: R6
		// check if any pawn on R5 has that mask in their attack-map.
		unsigned long pawnsOnR5 = originalPawns & R5;
		if (pawnsOnR5) {
			unsigned long epMap = b[IDX_EP_IDX];
			// gi through attack maps for each pawn. check overlap of EP_index
			int attackIndex = 0;
			while (pawnsOnR5) {
				int attackShift = __builtin_ctzll(pawnsOnR5);
				attackIndex += attackShift;
				unsigned long attackingPawn = 1l << attackIndex;

				if ((WHITE_PAWN_ATTACK_MAPS[attackIndex] & epMap) == epMap) {

					unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
					makeNewBoardIter();
#else
					makeNewBoard(b, move);
#endif


					clearBlackPiecesWithClearMap(move, ~(epMap >> 8));
					move[IDX_WHITE_PAWNS] &= ~attackingPawn;
					move[IDX_WHITE_PIECES] &= ~attackingPawn;
					move[IDX_WHITE_PAWNS] |= epMap;
					move[IDX_WHITE_PIECES] |= epMap;
					move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | move[IDX_BLACK_PIECES];
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

					if (calculateWhiteKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {
						//printBitBoard(move);
						calculateBlackKingCheckStatus2(move, epMap);
						count(move);
						nextBoardIdx++;
						numPawnMoves++;
					}
				}

				pawnsOnR5 >>= attackShift;
				pawnsOnR5 >>= 1;
				attackIndex++;
			}
		}
	}

	return numPawnMoves;

}

//////////////////////////////////// MOVE BLACK PAWNs //////////////

int moveBlackPawns() {

	int numPawnMoves = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pawns = b[IDX_BLACK_PAWNS];
	unsigned long originalPawns = pawns;
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	// 1. Make moves for pawns that can move ONE forward
	// 2. Make moves for pawns that can move TWO forward and set EP index/map
	// 3. Make capturing moves for pawns with BLACK_PAWN_ATTACK_MAPS
	// 4. Make en passant captures for pawns with BLACK_PAWN_ATTACK_MAPS that matches EP Index/map

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long pawnsThatCanMoveOneForward = pawns & ~(allPieces << 8);
	unsigned long pawnsThatCanMoveTwoForward = pawnsThatCanMoveOneForward & R7;
	pawnsThatCanMoveTwoForward &= ~(allPieces << 16);

	int pieceIdx = 0;

	while (pawnsThatCanMoveOneForward) {
		int shift = __builtin_ctzll(pawnsThatCanMoveOneForward);
		pieceIdx += shift;
		unsigned long pieceMap = 1l << pieceIdx;
		unsigned long moveToMap = pieceMap >> 8;
		unsigned long clearMap = ~pieceMap;
		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif

		makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);
		// printBitBoard(move);

		if (calculateBlackKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {

			if (moveToMap & R1) {
				move[IDX_CASTLING] &= ~moveToMap;
				numPawnMoves += 4;
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO;
				makeBlackBitPromos(move, moveToMap);
			}
			else {
				calculateWhiteKingCheckStatus2(move, moveToMap);
				numPawnMoves++;
			}
			count(move);
			nextBoardIdx++;
		}

		pawnsThatCanMoveOneForward >>= shift;
		pawnsThatCanMoveOneForward >>= 1;
		pieceIdx++;

	}

	pieceIdx = 0;

	while (pawnsThatCanMoveTwoForward) {

		int shift = __builtin_ctzll(pawnsThatCanMoveTwoForward);
		pieceIdx += shift;
		unsigned long pieceMap = 1l << pieceIdx;
		unsigned long moveToMap = pieceMap >> 16;
		unsigned long clearMap = ~pieceMap;
		unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
		makeNewBoardIter();
#else
		makeNewBoard(b, move);
#endif

		makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);
		move[IDX_EP_IDX] = pieceMap >> 8;
		// printBitBoard(move);

		if (calculateBlackKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {
			calculateWhiteKingCheckStatus2(move, moveToMap);
			count(move);
			nextBoardIdx++;
			numPawnMoves++;
		}

		pawnsThatCanMoveTwoForward >>= shift;
		pawnsThatCanMoveTwoForward >>= 1;
		pieceIdx++;

	}


	pieceIdx = 0;
	while (pawns) {
		int shift = __builtin_ctzll(pawns);
		pieceIdx += shift;

		unsigned long attackMap = BLACK_PAWN_ATTACK_MAPS[pieceIdx];

		if (whitePieces & attackMap) {

			unsigned long pieceMap = 1l << pieceIdx;
			unsigned long validAttacks = b[IDX_WHITE_PIECES] & attackMap;
			unsigned long clearMap = ~pieceMap;

			int attackIndex = 0;

			while (validAttacks) {
				int attackShift = __builtin_ctzll(validAttacks);
				attackIndex += attackShift;

				unsigned long moveToMap = 1L << attackIndex;
				unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
				makeNewBoardIter();
#else
				makeNewBoard(b, move);
#endif

				makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);
				// printBitBoard(move);

				if (calculateBlackKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {


					if (moveToMap & R1) {
						move[IDX_CASTLING] &= ~moveToMap;
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO | MASK_LAST_MOVE_WAS_CAPTURE;
						numPawnMoves += 4;
						makeBlackBitPromos(move, moveToMap);
					}
					else {
						calculateWhiteKingCheckStatus2(move, moveToMap);
						numPawnMoves++;
					}
					count(move);
					nextBoardIdx++;

				}
				validAttacks >>= attackShift;
				validAttacks >>= 1;
				attackIndex++;
			}
		}

		pawns >>= shift;
		pawns >>= 1;
		pieceIdx++;
	}

	if (b[IDX_EP_IDX] != 0) {
		// ep index is between A6 and H6, ie: R6
		// check if any pawn on R5 has that mask in their attack-map.
		unsigned long pawnsOnR4 = originalPawns & R4;
		if (pawnsOnR4) {
			unsigned long epMap = b[IDX_EP_IDX];
			// gi through attack maps for each pawn. check overlap of EP_index
			int attackIndex = 0;
			while (pawnsOnR4) {
				int attackShift = __builtin_ctzll(pawnsOnR4);
				attackIndex += attackShift;
				unsigned long attackingPawn = 1l << attackIndex;

				if ((BLACK_PAWN_ATTACK_MAPS[attackIndex] & epMap) == epMap) {
					unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
					makeNewBoardIter();
#else
					makeNewBoard(b, move);
#endif

					clearWhitePiecesWithClearMap(move, ~(epMap << 8));
					move[IDX_BLACK_PAWNS] &= ~attackingPawn;
					move[IDX_BLACK_PIECES] &= ~attackingPawn;
					move[IDX_BLACK_PAWNS] |= epMap;
					move[IDX_BLACK_PIECES] |= epMap;
					move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | move[IDX_BLACK_PIECES];
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

					if (calculateBlackKingCheckStatus(move) == 0 && b[IDX_MOVE_NUM] < MAX_LEVEL) {
						//printBitBoard(move);
						calculateWhiteKingCheckStatus2(move, epMap);
						count(move);
						nextBoardIdx++;
						numPawnMoves++;
					}
				}

				pawnsOnR4 >>= attackShift;
				pawnsOnR4 >>= 1;
				attackIndex++;

			}
		}
	}

	return numPawnMoves;
}

//////////////////////////////////// MOVE WHITE BISHOPS /////////////////////////////////////
int moveWhiteBishopsOrQueens(int pieceMapIndex) {

	int numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;
	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;
		unsigned long pieceMap = 1l << idx;

		unsigned long moveToMap = pieceMap;
		unsigned long clearMap = ~pieceMap;

		//--

		int rank = idx >> 3;
		int file = idx & 7;


		int maxM = 7 - rank;
		maxM = 7 - file < maxM ? 7 - file : maxM;
		unsigned long testMap = 0;
		for (int m = 0; m < maxM; m++) {
			moveToMap <<= 9;
			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;

			}

			if (moveToMap & blackPieces) {
				break;
			}

			testMap |= moveToMap;

		}

		maxM = 7 - rank;
		maxM = file < maxM ? file : maxM;

		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap <<= 7;
			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

			testMap |= moveToMap;
		}


		maxM = rank;
		maxM = maxM > (7 - file) ? (7 - file) : maxM;

		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap >>= 7;
			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

			testMap |= moveToMap;
		}

		maxM = rank;
		maxM = maxM > file ? file : maxM;
		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap >>= 9;
			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

			testMap |= moveToMap;

		}

		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;
}

//////////////////////////////////// MOVE BLACK BISHOPS /////////////////////////////////////
int moveBlackBishopsOrQueens(int pieceMapIndex) {

	int numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;

	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;
	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;
		unsigned long pieceMap = 1l << idx;

		unsigned long moveToMap = pieceMap;
		unsigned long clearMap = ~pieceMap;

		//--

		int rank = idx >> 3;
		int file = idx & 7;

		int maxM = 7 - rank;
		maxM = 7 - file < maxM ? 7 - file : maxM;

		for (int m = 0; m < maxM; m++) {
			moveToMap <<= 9;

			if (moveToMap & blackPieces) {
				break;
			}
			// todo: make new move burde kunne bruke currentIdx og nextIdx
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		maxM = 7 - rank;
		maxM = file < maxM ? file : maxM;

		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap <<= 7;
			if (moveToMap & blackPieces) {
				break;
			}
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		maxM = rank;
		maxM = maxM > (7 - file) ? (7 - file) : maxM;

		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap >>= 7;

			if (moveToMap & blackPieces) {
				break;
			}
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		maxM = rank;
		maxM = maxM > file ? file : maxM;
		moveToMap = pieceMap;

		for (int m = 0; m < maxM; m++) {
			moveToMap >>= 9;
			if (moveToMap & blackPieces) {
				break;
			}
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);

#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;
}

///////////////////////////////// MOVE WHITE ROOKS //////////////////////////////
int moveWhiteRooksOrQueens(int pieceMapIndex) {

	int numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;
	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;

		unsigned long pieceMap = 1l << idx;
		// for hver piece. flytt h&v til vi treffer noe
		int file = idx & 7;
		int rank = idx >> 3;

		unsigned long moveToMap = pieceMap;
		unsigned long clearMap = ~pieceMap;

		// up
		for (int m = 0; m < 7 - rank; m++) {
			moveToMap <<= 8;

			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

		}

		// left
		moveToMap = pieceMap;
		for (int m = 0; m < 7 - file; m++) {
			moveToMap <<= 1;

			if (moveToMap & whitePieces) {
				break;
			}
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

		}

		// right
		moveToMap = pieceMap;
		for (int m = 0; m < file; m++) {
			moveToMap >>= 1;

			if (moveToMap & whitePieces) {
				break;
			}
			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

		}

		// down
		moveToMap = pieceMap;
		for (int m = 0; m < rank; m++) {

			moveToMap >>= 8;

			if (moveToMap & whitePieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & blackPieces) {
				break;
			}

		}

		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;

}

///////////////////////////////////// MOVE BLACK ROOKS //////////////////////////////////////
int moveBlackRooksOrQueens(int pieceMapIndex) {

	int numMovesFound = 0;

	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;

	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;

		const unsigned long pieceMap = 1l << idx;
		// for hver piece. flytt h&v til vi treffe noe

		int file = idx & 7;
		int rank = idx >> 3;

		unsigned long moveToMap = pieceMap;
		unsigned long clearMap = ~pieceMap;

		// up
		for (int m = 0; m < 7 - rank; m++) {
			moveToMap <<= 8;

			if (moveToMap & blackPieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		// left
		moveToMap = pieceMap;

		for (int m = 0; m < 7 - file; m++) {
			moveToMap <<= 1;

			if (moveToMap & blackPieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		// right
		moveToMap = pieceMap;

		for (int m = 0; m < file; m++) {
			moveToMap >>= 1;

			if (moveToMap & blackPieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}

		}

		// down
		moveToMap = pieceMap;

		for (int m = 0; m < rank; m++) {
			moveToMap >>= 8;

			if (moveToMap & blackPieces) {
				break;
			}

			unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
			makeNewBoardIter();
#else
			makeNewBoard(b, move);
#endif

			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move, moveToMap);
				count(move);
				numMovesFound++;
				nextBoardIdx++;
			}

			if (moveToMap & whitePieces) {
				break;
			}
		}

		//printLongAsBitBoard( testMap );
		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;

}

///////////////////////////////////// MOVE WHITE KNIGHTS //////////////////////////////////////
int moveWhiteKnights() {

	int numMovesFound = 0;
	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;
	unsigned long pieces = b[IDX_WHITE_KNIGHTS];

	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;
		unsigned long pieceMap = 1l << idx;
		unsigned long clearMap = ~pieceMap;

		unsigned long moveToMaps = KNIGHT_ATTACK_MAPS[idx];
		moveToMaps &= ~whitePieces;

		int moveToIdx = 0;

		while (moveToMaps) {

			int moveToShift = __builtin_ctzll(moveToMaps);
			moveToIdx += moveToShift;

			unsigned long moveToMap = 1l << moveToIdx;
			if (!(moveToMap & whitePieces)) {
				unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
				makeNewBoardIter();
#else
				makeNewBoard(b, move);
#endif

				if (moveToMap & blackPieces) {
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
					clearBlackPiecesWithClearMap(move, ~moveToMap);
				}

				move[IDX_WHITE_KNIGHTS] &= clearMap;
				move[IDX_WHITE_KNIGHTS] |= moveToMap;
				move[IDX_WHITE_PIECES] &= clearMap;
				move[IDX_WHITE_PIECES] |= moveToMap;
				move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | move[IDX_BLACK_PIECES];

				if (calculateWhiteKingCheckStatus(move) == 0) {

					move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
					calculateBlackKingCheckStatus2(move, moveToMap);
					count(move);
					nextBoardIdx++;
					numMovesFound++;
				}
			}

			moveToMaps >>= moveToShift;
			moveToMaps >>= 1;
			moveToIdx++;

		}

		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;

}

///////////////////////////////////// MOVE BLACK KNIGHTS //////////////////////////////////////

int moveBlackKnights() {

	int numMovesFound = 0;

	unsigned long offset = currentBoardIdx * NUM_BYTES;
	unsigned long *b = boards + offset;

	unsigned long pieces = b[IDX_BLACK_KNIGHTS];
	unsigned long originalPieces = pieces;

	int idx = 0;

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;
		unsigned long pieceMap = 1ul << idx;
		unsigned long clearMap = ~pieceMap;

		unsigned long moveToMaps = KNIGHT_ATTACK_MAPS[idx];
		moveToMaps &= ~blackPieces;

		int moveToIdx = 0;

		while (moveToMaps) {

			int moveToShift = __builtin_ctzll(moveToMaps);
			moveToIdx += moveToShift;

			unsigned long moveToMap = 1l << moveToIdx;
			if (!(moveToMap & blackPieces)) {
				unsigned long *move = boards + (nextBoardIdx * NUM_BYTES);
#ifdef MAKE_NEW_BOARD_ITER
				makeNewBoardIter();
#else
				makeNewBoard(b, move);
#endif

				makeBlackMove(move, IDX_BLACK_KNIGHTS, moveToMap, clearMap, whitePieces);

				if (calculateBlackKingCheckStatus(move) == 0) {
					move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
					calculateWhiteKingCheckStatus2(move, moveToMap);
					count(move);
					nextBoardIdx++;
					numMovesFound++;
				}
			}

			moveToMaps >>= moveToShift;
			moveToMaps >>= 1;
			moveToIdx++;

		}

		pieces >>= shift;
		pieces >>= 1;
		idx++;

	}

	return numMovesFound;

}

//////////////////////////////// ----------- white bit promos ---------
void makeWhiteBitPromos(unsigned long *board, unsigned long newPieceMap) {

	unsigned long clearPiece = ~newPieceMap;
	board[IDX_WHITE_PAWNS] &= clearPiece;
	board[IDX_WHITE_ROOKS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board, newPieceMap);
    count(board);
    nextBoardIdx++;

    board[IDX_WHITE_ROOKS] &= clearPiece;
	board[IDX_WHITE_QUEENS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

	board[IDX_WHITE_QUEENS] &= clearPiece;
	board[IDX_WHITE_BISHOPS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

	board[IDX_WHITE_BISHOPS] &= clearPiece;
	board[IDX_WHITE_KNIGHTS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

}

void makeBlackBitPromos(unsigned long *board, unsigned long newPieceMap) {

	// TODO: NR add boards to stack

	unsigned long clearPiece = ~newPieceMap;

	board[IDX_BLACK_PAWNS] &= clearPiece;
	board[IDX_BLACK_ROOKS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

	board[IDX_BLACK_ROOKS] &= clearPiece;
	board[IDX_BLACK_QUEENS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

	board[IDX_BLACK_QUEENS] &= clearPiece;
	board[IDX_BLACK_BISHOPS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

	board[IDX_BLACK_BISHOPS] &= clearPiece;
	board[IDX_BLACK_KNIGHTS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board, newPieceMap);
	// TODO: NR dig(board);

}

void makeNewBoardIter() {
	makeNewBoardInvocations++;
	unsigned long *newBoard = boards + (nextBoardIdx * NUM_BYTES);
	unsigned long *oldBoard = boards + (currentBoardIdx * NUM_BYTES);

	memcpy(newBoard, oldBoard, sizeof(unsigned long) * NUM_BYTES_TO_COPY);
	newBoard[IDX_LAST_MOVE_WAS] = 0;
	newBoard[IDX_EP_IDX] = 0;
	newBoard[IDX_MULTIPLIER] = oldBoard[IDX_MULTIPLIER];

	newBoard[IDX_MOVE_ID] = makeNewBoardInvocations;
	newBoard[IDX_PARENT_MOVE_ID] = oldBoard[IDX_MOVE_ID];
	newBoard[IDX_MOVE_NUM]++;
	newBoard[IDX_TURN] ^= TURN_INVERTER;
	newBoard[IDX_CHECK_STATUS] = 0;
	newBoard[IDX_PARENT_IDX] = oldBoard[IDX_CURRENT_IDX];
	newBoard[IDX_FINISHED] = 0;
}

void makeNewBoard(const unsigned long *oldBoard, unsigned long *newBoard) {

	memcpy(newBoard, oldBoard, sizeof(unsigned long) * NUM_BYTES_TO_COPY);
	newBoard[IDX_MOVE_ID] = makeNewBoardInvocations++;
	newBoard[IDX_LAST_MOVE_WAS] = 0;
	newBoard[IDX_EP_IDX] = 0;
	newBoard[IDX_FINISHED] = 0;
	newBoard[IDX_CHECK_STATUS] = 0;

	newBoard[IDX_MULTIPLIER] = oldBoard[IDX_MULTIPLIER];
	newBoard[IDX_PARENT_MOVE_ID] = oldBoard[IDX_MOVE_ID];
	newBoard[IDX_TURN] ^= TURN_INVERTER;
	newBoard[IDX_PARENT_IDX] = oldBoard[IDX_CURRENT_IDX];
	newBoard[IDX_MOVE_NUM]++;

}


void makeWhiteMove(unsigned long move[], int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long blackPieces) {
	unsigned long xorMap = ~clearMap | moveToMap;
	move[pieceMapIndex] ^= xorMap;
	move[IDX_WHITE_PIECES] ^= xorMap;

	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | blackPieces;

	if (moveToMap & blackPieces) {
		move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
		clearBlackPiecesWithClearMap(move, ~moveToMap);
	}

}

void makeBlackMove(unsigned long move[], int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long whitePieces) {
	move[pieceMapIndex] &= clearMap;
	move[pieceMapIndex] |= moveToMap;
	move[IDX_BLACK_PIECES] &= clearMap;
	move[IDX_BLACK_PIECES] |= moveToMap;
	move[IDX_ALL_PIECES] = move[IDX_BLACK_PIECES] | whitePieces;

	if (moveToMap & whitePieces) {
		move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
		clearWhitePiecesWithClearMap(move, ~moveToMap);
	}

}

// TODO: kanskje denne ikke trenger board, men bare nextBoardIdx
int calculateWhiteKingCheckStatus(const unsigned long *board) {

	// TODO: i følge stats, er det den brykke som ble flytta sist som utgjorde trusselen


	unsigned long king = board[IDX_WHITE_KING];
	unsigned long idx = __builtin_ctzll(king);


	if (KNIGHT_ATTACK_MAPS[idx] & board[IDX_BLACK_KNIGHTS]) {
		return MASK_WHITE_KING_CHECKED;
	}

	if (BLACK_PAWN_REVERSE_ATTACK_MAPS[idx] & board[IDX_BLACK_PAWNS]) {
		return MASK_WHITE_KING_CHECKED;
	}

	if (KING_ATTACK_MAPS[idx] & board[IDX_BLACK_KING]) {
		return MASK_WHITE_KING_CHECKED;
	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QB_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS])) {

		// need to check each direction for a block
		unsigned long qb = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QB_ATTACK_MAPS[idx];

		if (__builtin_popcountll(test) == 1) {
			return MASK_WHITE_KING_CHECKED;
		}

		// QB_ATTACK_MAPS_1 down right
		// QB_ATTACK_MAPS_2 up left
		// QB_ATTACK_MAPS_3 up right
		// QB_ATTACK_MAPS_4 down left

		unsigned long allPiecesExceptQB = allPieces & ~qb;

		if (king > (qb & (QB_ATTACK_MAPS_1[idx] | QB_ATTACK_MAPS_4[idx]))) {

			if ((qb & QB_ATTACK_MAPS_4[idx]) > ((QB_ATTACK_MAPS_4[idx] & allPiecesExceptQB))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((qb & QB_ATTACK_MAPS_1[idx]) > ((QB_ATTACK_MAPS_1[idx] & allPiecesExceptQB))) {
				return MASK_WHITE_KING_CHECKED;
			}
		}


		if (qb & QB_ATTACK_MAPS_2[idx]) {
			if (king < (qb & QB_ATTACK_MAPS_2[idx])) {
				if (__builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]) <= __builtin_ctzll(QB_ATTACK_MAPS_2[idx] & allPieces)) {
					return MASK_WHITE_KING_CHECKED;
				}
				if ((QB_ATTACK_MAPS_2[idx] & allPiecesExceptQB) == 0) {
					return MASK_WHITE_KING_CHECKED;
				}
			}
		}


		if (king < (qb & QB_ATTACK_MAPS_3[idx])) {
			if (__builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]) <= __builtin_ctzll((QB_ATTACK_MAPS_3[idx] & allPieces))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((QB_ATTACK_MAPS_3[idx] & allPiecesExceptQB) == 0) {
				return MASK_WHITE_KING_CHECKED;
			}
		}

	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QR_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS])) {

		// need to check each direction for a block
		unsigned long qr = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
		if (__builtin_popcountll(test) == 1) {
			return MASK_WHITE_KING_CHECKED;
		}


		// quadrant testing
		// QR_ATTACK_MAPS_1[] = right
		// QR_ATTACK_MAPS_2[] = down
		// QR_ATTACK_MAPS_3[] = up
		// QR_ATTACK_MAPS_4[] = left

		unsigned long allPiecesExceptQR = allPieces & ~qr;

		if (king > (qr & (QR_ATTACK_MAPS_1[idx] | QR_ATTACK_MAPS_2[idx]))) {

			if ((qr & QR_ATTACK_MAPS_2[idx]) > ((QR_ATTACK_MAPS_2[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((qr & QR_ATTACK_MAPS_1[idx]) > ((QR_ATTACK_MAPS_1[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
		}


		if (king < (qr & QR_ATTACK_MAPS_3[idx])) {
			if (__builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) <
				__builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
				return MASK_WHITE_KING_CHECKED;
			}
		}

		if (king < (qr & QR_ATTACK_MAPS_4[idx])) {
			if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
				__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR) == 0) {
				return MASK_WHITE_KING_CHECKED;
			}
		}

	}

	return 0;

}

int calculateBlackKingCheckStatus(const unsigned long *board) {

	unsigned long king = board[IDX_BLACK_KING];
	unsigned long idx = __builtin_ctzll(king);

	if (KNIGHT_ATTACK_MAPS[idx] & board[IDX_WHITE_KNIGHTS]) {
		return MASK_BLACK_KING_CHECKED;
	}

// Yes, it is ok to use black pawn attack maps here.
	if (BLACK_PAWN_ATTACK_MAPS[idx] & board[IDX_WHITE_PAWNS]) {
		return MASK_BLACK_KING_CHECKED;
	}

	if (KING_ATTACK_MAPS[idx] & board[IDX_WHITE_KING]) {
		return MASK_BLACK_KING_CHECKED;
	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QB_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS])) {

		// need to check each direction for a block
		unsigned long qb = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QB_ATTACK_MAPS[idx];

		if (__builtin_popcountll(test) == 1) {
			return MASK_BLACK_KING_CHECKED;
		}


		unsigned long allPiecesExceptQB = allPieces & ~qb;

		if (king > (qb & (QB_ATTACK_MAPS_1[idx] | QB_ATTACK_MAPS_4[idx]))) {

			if ((qb & QB_ATTACK_MAPS_4[idx]) > ((QB_ATTACK_MAPS_4[idx] & allPiecesExceptQB))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((qb & QB_ATTACK_MAPS_1[idx]) > ((QB_ATTACK_MAPS_1[idx] & allPiecesExceptQB))) {
				return MASK_BLACK_KING_CHECKED;
			}
		}


		if (qb & QB_ATTACK_MAPS_2[idx]) {
			if (king < (qb & QB_ATTACK_MAPS_2[idx])) {

				if (__builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]) <= __builtin_ctzll(QB_ATTACK_MAPS_2[idx] & allPieces)) {
					return MASK_BLACK_KING_CHECKED;
				}

				if ((QB_ATTACK_MAPS_2[idx] & allPiecesExceptQB) == 0) {
					return MASK_BLACK_KING_CHECKED;
				}
			}
		}


		if (king < (qb & QB_ATTACK_MAPS_3[idx])) {
			if (__builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]) <= __builtin_ctzll((QB_ATTACK_MAPS_3[idx] & allPieces))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((QB_ATTACK_MAPS_3[idx] & allPiecesExceptQB) == 0) {
				return MASK_BLACK_KING_CHECKED;
			}
		}

	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QR_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS])) {

		// need to check each direction for a block
		unsigned long qr = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
		if (__builtin_popcountll(test) == 1) {
			return MASK_BLACK_KING_CHECKED;
		}


		// quadrant testing
		// QR_ATTACK_MAPS_1[] = right
		// QR_ATTACK_MAPS_2[] = down
		// QR_ATTACK_MAPS_3[] = up
		// QR_ATTACK_MAPS_4[] = left
		unsigned long allPiecesExceptQR = allPieces & ~qr;

		if (king > (qr & (QR_ATTACK_MAPS_1[idx] | QR_ATTACK_MAPS_2[idx]))) {

			if ((qr & QR_ATTACK_MAPS_2[idx]) > ((QR_ATTACK_MAPS_2[idx] & allPiecesExceptQR))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((qr & QR_ATTACK_MAPS_1[idx]) > ((QR_ATTACK_MAPS_1[idx] & allPiecesExceptQR))) {
				return MASK_BLACK_KING_CHECKED;
			}
		}


		if (king < (qr & QR_ATTACK_MAPS_3[idx])) {
			if (__builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) <
				__builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
				return MASK_BLACK_KING_CHECKED;
			}
		}

		if (king < (qr & QR_ATTACK_MAPS_4[idx])) {
			if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
				__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR) == 0) {
				return MASK_BLACK_KING_CHECKED;
			}
		}
	}

	return 0;
}

// this is used to check for discovered and double checks
unsigned long calculateBlackKingCheckStatus2(unsigned long *board, unsigned long lastMoveMap) {

	//board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
	//return 0;


	unsigned long king = board[IDX_BLACK_KING];
	unsigned long idx = __builtin_ctzll(king);

	unsigned long threat = 0;

	threat = KNIGHT_ATTACK_MAPS[idx] & board[IDX_WHITE_KNIGHTS];
	threat |= BLACK_PAWN_ATTACK_MAPS[idx] & board[IDX_WHITE_PAWNS];

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QB_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS])) {

		// need to check each direction for a block
		unsigned long qb = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QB_ATTACK_MAPS[idx];

		if (__builtin_popcountll(test) == 1) {
			threat |= allPieces & QB_ATTACK_MAPS[idx];
		}
		else {

			unsigned long allPiecesExceptQB = allPieces & ~qb;

			// QB_ATTACK_MAPS_1 down right
			// QB_ATTACK_MAPS_2 up left
			// QB_ATTACK_MAPS_3 up right
			// QB_ATTACK_MAPS_4 down left

			// up left
			if (qb & QB_ATTACK_MAPS_2[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_2[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_2[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]);
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if ((allPieces & QB_ATTACK_MAPS_2[idx]) == (qb & QB_ATTACK_MAPS_2[idx])) {
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll(allPiecesExceptQB & QB_ATTACK_MAPS_2[idx]);
						if (test < apnq) {
							threat |= 1L << test;
						}
					}

				}
			}

			// up right
			if (qb & QB_ATTACK_MAPS_3[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_3[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_3[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]);
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if ((allPieces & QB_ATTACK_MAPS_3[idx]) == (qb & QB_ATTACK_MAPS_3[idx])) {
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll(allPiecesExceptQB & QB_ATTACK_MAPS_3[idx]);
						if (test < apnq) {
							threat |= 1L << test;
						}
					}

				}
			}


			// down right
			if (qb & QB_ATTACK_MAPS_1[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_1[idx]) == 1) {
					threat |= qb & QB_ATTACK_MAPS_1[idx];
				}
				else {
					int test = __builtin_clzll(qb & QB_ATTACK_MAPS_1[idx]);

					if ((allPieces & QB_ATTACK_MAPS_1[idx]) == (qb & QB_ATTACK_MAPS_1[idx])) {
						threat |= 1L << (63 - test);
					}
					else {
						// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
						int apnq = __builtin_clzll(allPiecesExceptQB & QB_ATTACK_MAPS_1[idx]);
						if (test < apnq) {
							threat |= 1L << (63 - test);
						}
					}
				}
			}
			// down left
			if (qb & QB_ATTACK_MAPS_4[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_4[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_4[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int apnq = __builtin_clzll(allPiecesExceptQB & QB_ATTACK_MAPS_4[idx]);
					int test = __builtin_clzll(qb & QB_ATTACK_MAPS_4[idx]);
					if (test < apnq) {
						threat |= 1L << (63 - test);
					}
				}
			}

		}
	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QR_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS])) {

		// need to check each direction for a block
		unsigned long qr = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
		if (__builtin_popcountll(test) == 1) {
			threat |= test;
		}
		else {
			// quadrant testing
			// QR_ATTACK_MAPS_1[] = right
			// QR_ATTACK_MAPS_2[] = down
			// QR_ATTACK_MAPS_3[] = up
			// QR_ATTACK_MAPS_4[] = left
			unsigned long allPiecesExceptQR = allPieces & ~qr;

			if (king > (qr & (QR_ATTACK_MAPS_1[idx] | QR_ATTACK_MAPS_2[idx]))) {

				if (__builtin_popcountll(qr & QR_ATTACK_MAPS_1[idx]) == 1 &&
					__builtin_popcountll(allPieces & QR_ATTACK_MAPS_1[idx]) == 1) {
					threat |= qr & QR_ATTACK_MAPS_1[idx];
				}
				else if (__builtin_popcountll(qr & QR_ATTACK_MAPS_2[idx]) == 1 &&
						 __builtin_popcountll(allPieces & QR_ATTACK_MAPS_2[idx]) == 1) {
					threat |= qr & QR_ATTACK_MAPS_2[idx];
				}
				else {

					if ((qr & QR_ATTACK_MAPS_2[idx]) > ((QR_ATTACK_MAPS_2[idx] & allPiecesExceptQR))) {
						long testThreat = 1L << (63 - __builtin_clzll((qr & QR_ATTACK_MAPS_2[idx])));
						threat |= testThreat;

					}
					if ((qr & QR_ATTACK_MAPS_1[idx]) > ((QR_ATTACK_MAPS_1[idx] & allPiecesExceptQR))) {
						long testThreat = 1L << (63 - __builtin_clzll((qr & QR_ATTACK_MAPS_1[idx])));
						threat |= testThreat;
					}
				}
			}


			if (king < (qr & QR_ATTACK_MAPS_3[idx])) {
				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]);
				}
			}

			if (king < (qr & QR_ATTACK_MAPS_4[idx])) {

				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]);
				}
			}
		}
	}


	board[IDX_KING_THREATS] = threat;

	if (threat) {
		board[IDX_CHECK_STATUS] = MASK_BLACK_KING_CHECKED;

		if (!(threat & lastMoveMap)) {
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
		}

		if (__builtin_popcountll(threat) > 1) {
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DOUBLE;
		}
	}
	else {
		board[IDX_CHECK_STATUS] = 0;

	}

	return threat;
}


// this is used to check for discovered and double checks on white king
unsigned long calculateWhiteKingCheckStatus2(unsigned long *board, unsigned long lastMoveMap) {

	//board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
	//return 0;

	unsigned long king = board[IDX_WHITE_KING];
	unsigned long idx = __builtin_ctzll(king);

	unsigned long threat = 0;

	threat = KNIGHT_ATTACK_MAPS[idx] & board[IDX_BLACK_KNIGHTS];
	threat |= WHITE_PAWN_ATTACK_MAPS[idx] & board[IDX_BLACK_PAWNS];

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QB_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS])) {

		// need to check each direction for a block
		unsigned long qb = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QB_ATTACK_MAPS[idx];

		if (__builtin_popcountll(test) == 1) {
			threat |= allPieces & QB_ATTACK_MAPS[idx];
		}
		else {

			unsigned long allPiecesExceptQB = allPieces & ~qb;

			// QB_ATTACK_MAPS_1 down right
			// QB_ATTACK_MAPS_2 up left
			// QB_ATTACK_MAPS_3 up right
			// QB_ATTACK_MAPS_4 down left

			// up left
			if (qb & QB_ATTACK_MAPS_2[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_2[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_2[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]);
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if ((allPieces & QB_ATTACK_MAPS_2[idx]) == (qb & QB_ATTACK_MAPS_2[idx])) {
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll(allPiecesExceptQB & QB_ATTACK_MAPS_2[idx]);
						if (test < apnq) {
							threat |= 1L << test;
						}
					}

				}
			}

			// up right
			if (qb & QB_ATTACK_MAPS_3[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_3[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_3[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]);
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if ((allPieces & QB_ATTACK_MAPS_3[idx]) == (qb & QB_ATTACK_MAPS_3[idx])) {
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll(allPiecesExceptQB & QB_ATTACK_MAPS_3[idx]);
						if (test < apnq) {
							threat |= 1L << test;
						}
					}

				}
			}


			// down right
			if (qb & QB_ATTACK_MAPS_1[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_1[idx]) == 1) {
					threat |= qb & QB_ATTACK_MAPS_1[idx];
				}
				else {
					int test = __builtin_clzll(qb & QB_ATTACK_MAPS_1[idx]);

					if ((allPieces & QB_ATTACK_MAPS_1[idx]) == (qb & QB_ATTACK_MAPS_1[idx])) {
						threat |= 1L << (63 - test);
					}
					else {
						// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
						int apnq = __builtin_clzll(allPiecesExceptQB & QB_ATTACK_MAPS_1[idx]);
						if (test < apnq) {
							threat |= 1L << (63 - test);
						}
					}
				}
			}
			// down left
			if (qb & QB_ATTACK_MAPS_4[idx]) {
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if (__builtin_popcountll(allPieces & QB_ATTACK_MAPS_4[idx]) == 1) {
					threat |= allPieces & QB_ATTACK_MAPS_4[idx];

				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int apnq = __builtin_clzll(allPiecesExceptQB & QB_ATTACK_MAPS_4[idx]);
					int test = __builtin_clzll(qb & QB_ATTACK_MAPS_4[idx]);
					if (test < apnq) {
						threat |= 1L << (63 - test);
					}
				}
			}

		}
	}

	// now for the vector threats. a piece may be on the map, but can be blocked
	if (QR_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS])) {

		// need to check each direction for a block
		unsigned long qr = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS]);
		unsigned long allPieces = board[IDX_ALL_PIECES];

		// if the only pieces intersecting the attackmap is a Q or B, then white
		// is in check
		unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
		if (__builtin_popcountll(test) == 1) {
			threat |= test;
		}
		else {
			// quadrant testing
			// QR_ATTACK_MAPS_1[] = right
			// QR_ATTACK_MAPS_2[] = down
			// QR_ATTACK_MAPS_3[] = up
			// QR_ATTACK_MAPS_4[] = left
			unsigned long allPiecesExceptQR = allPieces & ~qr;

			if (__builtin_popcountll(qr & QR_ATTACK_MAPS_1[idx]) == 1 &&
				__builtin_popcountll(allPieces & QR_ATTACK_MAPS_1[idx]) == 1) {
				threat |= qr & QR_ATTACK_MAPS_1[idx];
			}
			else if (__builtin_popcountll(qr & QR_ATTACK_MAPS_2[idx]) == 1 &&
					 __builtin_popcountll(allPieces & QR_ATTACK_MAPS_2[idx]) == 1) {
				threat |= qr & QR_ATTACK_MAPS_2[idx];
			}
			else {

				if ((qr & QR_ATTACK_MAPS_2[idx]) > ((QR_ATTACK_MAPS_2[idx] & allPiecesExceptQR))) {
					long testThreat = 1L << (63 - __builtin_clzll((qr & QR_ATTACK_MAPS_2[idx])));
					threat |= testThreat;

				}
				if ((qr & QR_ATTACK_MAPS_1[idx]) > ((QR_ATTACK_MAPS_1[idx] & allPiecesExceptQR))) {
					long testThreat = 1L << (63 - __builtin_clzll((qr & QR_ATTACK_MAPS_1[idx])));
					threat |= testThreat;
				}
			}

			if (king < (qr & QR_ATTACK_MAPS_3[idx])) {
				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]);
				}
			}

			if (king < (qr & QR_ATTACK_MAPS_4[idx])) {

				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]);
				}
			}
		}
	}


	board[IDX_KING_THREATS] = threat;

	if (threat) {
		board[IDX_CHECK_STATUS] = MASK_WHITE_KING_CHECKED;

		if (!(threat & lastMoveMap)) {
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
		}

		if (__builtin_popcountll(threat) > 1) {
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DOUBLE;
		}
	}
	else {
		board[IDX_CHECK_STATUS] = 0;
	}


	return threat;
}

void clearBlackPiecesWithClearMap(unsigned long *board, unsigned long clear) {
	for (int t = IDX_BLACK_PAWNS; t <= IDX_BLACK_PIECES; t++) {
		board[t] &= clear;
	}
}

void clearWhitePiecesWithClearMap(unsigned long *board, unsigned long clear) {
	for (int t = IDX_WHITE_PAWNS; t <= IDX_WHITE_PIECES; t++) {
		board[t] &= clear;
	}
}

// todo : make a count function that just counts the board at board[nextBoardIdx]
void count(const unsigned long *b) {

	//if( b[IDX_MOVE_NUM] == 3) {
	// printf("current/next %d %d\n", currentBoardIdx, nextBoardIdx );
	//}

	const unsigned long level = b[IDX_MOVE_NUM];
	const unsigned long add = b[IDX_MULTIPLIER];
	numMoves[level] += add;
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CAPTURE)) {
		numCaptures[level] += add;
		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE) {
			numDoubleCaptureChecks[level] += add;
		}
		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryCaptureChecks[level] += add;
		}
	}
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_EP_STRIKE)) {
		numEP[level] += add;
		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE) {
			numDoubleEPChecks[level] += add;
		}
		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryEPChecks[level] += add;
		}
	}
	if ((b[IDX_LAST_MOVE_WAS] & (MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE | MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE))) {
		numCastles[level] += add;
	}
	if (b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_PROMO) {
		numPromos[level] += add;

		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE) {
			numDoublePromoChecks[level] += add;
		}
		if (b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryPromoChecks[level] += add;
		}

	}

	if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_MATED)) {
		numCheckmates[level] += add;
	}

	if ((b[IDX_CHECK_STATUS] & (MASK_WHITE_KING_CHECKED | MASK_BLACK_KING_CHECKED))) {
		numChecks[level] += add;
	}

	if ((b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE)) {
		numDoubleChecks[level] += add;
	}

	if ((b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED)) {
		numDiscoveryChecks[level] += add;
	}

	if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_STALEMATED)) {
		numStalemates[level] += add;
	}

	if (outFile != NULL && b[IDX_MOVE_NUM] == MAX_LEVEL) {
		if (LOG_TYPE == LOG_TYPE_NIBBLE_BINARY) {
			unsigned char binary[NIBBLE_BINARY_BYTE_SIZE];
			bitBoardToNibbleBinary(b, binary);

			memcpy(outFileBuff + outFileBuffOffset, binary, NIBBLE_BINARY_BYTE_SIZE);
			outFileBuffOffset += NIBBLE_BINARY_BYTE_SIZE;
			buffWrites++;

			if (outFileBuffOffset > 1023 * 1024) {
				fileWrites++;
				fwrite(outFileBuff, 1, outFileBuffOffset, outFile);
				outFileBuffOffset = 0;
			}
		}
	}

}


/************************************************************************************************************
 **																										**
 **													UTILS												 **
 **																										**
 ************************************************************************************************************/

void printNumBoard(unsigned long *board) {
	for (int s = 0; (s & 64) == 0; s++) {
		if (s % 8 == 0 && s != 0) {
			printf("\n");
		}
		printf(" %ld\t", board[s]);
	}
}

void sprintDiagram(char *target, unsigned long *board) {

	char str[] = "................................................................\0";

	setBitsToChar(str, board[IDX_WHITE_PAWNS], 'P');
	setBitsToChar(str, board[IDX_WHITE_ROOKS], 'R');
	setBitsToChar(str, board[IDX_WHITE_BISHOPS], 'B');
	setBitsToChar(str, board[IDX_WHITE_KNIGHTS], 'N');
	setBitsToChar(str, board[IDX_WHITE_QUEENS], 'Q');
	setBitsToChar(str, board[IDX_WHITE_KING], 'K');

	setBitsToChar(str, board[IDX_BLACK_PAWNS], 'p');
	setBitsToChar(str, board[IDX_BLACK_ROOKS], 'r');
	setBitsToChar(str, board[IDX_BLACK_BISHOPS], 'b');
	setBitsToChar(str, board[IDX_BLACK_KNIGHTS], 'n');
	setBitsToChar(str, board[IDX_BLACK_QUEENS], 'q');
	setBitsToChar(str, board[IDX_BLACK_KING], 'k');

	char res[] = "                                                                       \0";

	for (int t = 0; t < 64; t++) {
		res[t + (t >> 3)] = str[t];
	}

	char castling[] = "\0\0\0\0\0";

	int moveCursor = 0;
	if (board[IDX_CASTLING] == 0) {
		castling[moveCursor++] = '-';
	}
	else {
		if ((board[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE) {
			castling[moveCursor++] = 'K';
		}
		if ((board[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE) {
			castling[moveCursor++] = 'Q';
		}
		if ((board[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE) {
			castling[moveCursor++] = 'k';
		}
		if ((board[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE) {
			castling[moveCursor++] = 'q';
		}
	}

	char epSquare[] = "-\0\0";
	int rank = 0;
	int file = 0;
	if (board[IDX_EP_IDX] != 0) {
		int idx = __builtin_ctzll(board[IDX_EP_IDX]);
		rank = (idx >> 3);
		file = (7 - (idx & 7));

		epSquare[0] = 97 + file;
		epSquare[1] = 49 + rank;
	}

	sprintf(target, "\"%s %s %s %s %lu\"", res, board[IDX_TURN] == WHITE_MASK ? "w" : "b", castling, epSquare,
			board[IDX_MOVE_NUM]);


}

void printDiagram(unsigned long *board) {

	char str[200];
	sprintDiagram(str, board);
	/*if( strcmp( str, "\"rnbqkbnr pppppppp ........ ........ ........ ......P. PPPPPP.P RNBQKBNR b KQkq - 5\"") == 0 ){
		printf("# possile diff:\n");
		printf("# %s\r\n# ",str);
		compressBitBoard(board);
	}*/

	printf("%% %s\n", str);

}

void printLongAsBitBoard(unsigned long bitstream) {
	printf("  A B C D E F G H");
	unsigned long backup = bitstream;
	char str[65] = "................................................................\0";
	for (int t = 0; t < 64; t++) {
		if (1 & bitstream) {
			str[63 - t] = '1';
		}
		bitstream >>= 1;
	}

	for (int s = 0; (s & 64) == 0; s++) {
		if (s % 8 == 0) {
			printf("\n%d ", 8 - (s >> 3));
		}
		printf("%c ", str[s]);
	}
	printf("\n");
	printf("\n%#lx", backup);
	printf("\n%lu\n", backup);
	printf("\n\n");
	fflush(stdout);
}

void setBitsToChar(char *str, unsigned long bits, char c) {

	unsigned long originalBits = bits;
	int idx = 0;
	while (bits && idx < 64) {
		int shift = __builtin_clzll(bits);
		idx += shift;
		bits <<= (shift + 1);
		str[idx] = c;
		idx++;
	}

}

void printBitBoard(unsigned long *board) {
	printf("\n  A B C D E F G H");
	char str[] = ".................................................................\0";

	setBitsToChar(str, board[IDX_WHITE_PAWNS], 'P');
	setBitsToChar(str, board[IDX_WHITE_ROOKS], 'R');
	setBitsToChar(str, board[IDX_WHITE_BISHOPS], 'B');
	setBitsToChar(str, board[IDX_WHITE_KNIGHTS], 'N');
	setBitsToChar(str, board[IDX_WHITE_QUEENS], 'Q');
	setBitsToChar(str, board[IDX_WHITE_KING], 'K');

	setBitsToChar(str, board[IDX_BLACK_PAWNS], 'p');
	setBitsToChar(str, board[IDX_BLACK_ROOKS], 'r');
	setBitsToChar(str, board[IDX_BLACK_BISHOPS], 'b');
	setBitsToChar(str, board[IDX_BLACK_KNIGHTS], 'n');
	setBitsToChar(str, board[IDX_BLACK_QUEENS], 'q');
	setBitsToChar(str, board[IDX_BLACK_KING], 'k');

	if (board[IDX_EP_IDX]) {
		setBitsToChar(str, board[IDX_EP_IDX], '*');
	}

	for (int s = 0; (s & 64) == 0; s++) {
		if (s % 8 == 0) {
			printf("\n%d ", 8 - (s >> 3));
		}
		printf("%c ", str[s]);
	}
	printf("\n");
	printf("Move num: %ld\n", board[IDX_MOVE_NUM]);

	printf("Turn : %s\n", board[IDX_TURN] == WHITE_MASK ? "White" : "Black");

	if ((board[IDX_CHECK_STATUS] & MASK_KING_IS_MATED)) {
		printf("There is a MATE\n");
	}
	if (board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED) {
		printf("White is in check\n");
	}
	if (board[IDX_CHECK_STATUS] & MASK_BLACK_KING_CHECKED) {
		printf("Black is in check\n");
	}
	if (board[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
		printf("Discovered check\n");
	}
	if (board[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE) {
		printf("Double check\n");
	}


	if (board[IDX_EP_IDX]) {
		printf("EP Index:%d\n", __builtin_ctzll(board[IDX_EP_IDX]));
	}

	printf("Castling: %s%s%s%s\n",
		   (board[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE ? "K" : "",
		   (board[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE ? "Q" : "",
		   (board[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE ? "k" : "",
		   (board[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE ? "q" : ""
	);

	if (board[IDX_LAST_MOVE_WAS]) {
		if (board[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_PROMO) {
			printf("Last move was promotion\n");
		}
		if (board[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CAPTURE) {
			printf("Last move was capture\n");
		}
		if (board[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_EP_STRIKE) {
			printf("Last move was en passant\n");
		}
		if (board[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE) {
			printf("Last move was castling king side\n");
		}
		if (board[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE) {
			printf("Last move was castling queen side\n");
		}

	}

	printf("Level: %lu\n", board[IDX_MOVE_NUM]);
	printf("Current idx %lu\n", board[IDX_CURRENT_IDX]);
	printf("Parent idx %lu\n", board[IDX_PARENT_IDX]);
	//printf("Multiplier: %lu", board[IDX_MULTIPLIER]);
	printf("\n");

	fflush(stdout);

}


void bitBoardToNibbleBinary(const unsigned long *board, unsigned char nibbleBinary[]) {

	memset(nibbleBinary, 0, NIBBLE_BINARY_BYTE_SIZE);
	unsigned long castling = board[IDX_CASTLING];
	unsigned long castlingRooksMap = 0;

	castlingRooksMap = (MASK_CASTLING_BLACK_KING_SIDE & castling) == MASK_CASTLING_BLACK_KING_SIDE ? H8_MASK : 0;
	castlingRooksMap |= (MASK_CASTLING_BLACK_QUEEN_SIDE & castling) == MASK_CASTLING_BLACK_QUEEN_SIDE ? A8_MASK : 0;
	castlingRooksMap |= (MASK_CASTLING_WHITE_KING_SIDE & castling) == MASK_CASTLING_WHITE_KING_SIDE ? H1_MASK : 0;
	castlingRooksMap |= (MASK_CASTLING_WHITE_QUEEN_SIDE & castling) == MASK_CASTLING_WHITE_QUEEN_SIDE ? A1_MASK : 0;

	unsigned long *allPieces = (unsigned long *) &nibbleBinary[0];
	*allPieces = board[IDX_ALL_PIECES];

	unsigned long epMask = board[IDX_EP_IDX];

	if (epMask != 0) {
		// replace pawn +/8 from EP_IDX with
		if ((board[IDX_EP_IDX] & R3)) {
			epMask = board[IDX_EP_IDX] << 8;
		}
		else {
			epMask = board[IDX_EP_IDX] >> 8;
		}

	}


	int numPieces = 16;
	int nibbleShift = 0;

	long mask = 1L;

	for (int i = 0; i < 64; i++) {

		// find which piece

		if ((mask & board[IDX_WHITE_PAWNS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_WHITE_PAWN << nibbleShift;
		}
		else if ((mask & board[IDX_WHITE_ROOKS]) == mask) {
			if ((castlingRooksMap & mask) == mask) {
				nibbleBinary[numPieces >> 1] |= (NIBBLE_WHITE_ROOK_CAN_CASTLE << nibbleShift);
			}
			else {
				nibbleBinary[numPieces >> 1] |= (NIBBLE_WHITE_ROOK_CAN_NOT_CASTLE << nibbleShift);
			}
		}
		else if ((mask & board[IDX_WHITE_KNIGHTS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_WHITE_KNIGHT << nibbleShift;
		}
		else if ((mask & board[IDX_WHITE_BISHOPS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_WHITE_BISHOP << nibbleShift;
		}
		else if ((mask & board[IDX_WHITE_QUEENS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_WHITE_QUEEN << nibbleShift;
		}
		else if ((mask & board[IDX_WHITE_KING]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_WHITE_KING << nibbleShift;
		}
		else if ((mask & board[IDX_BLACK_PAWNS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_PAWN << nibbleShift;
		}
		else if ((mask & board[IDX_BLACK_ROOKS]) == mask) {
			if ((castlingRooksMap & mask) == mask) {
				nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_ROOK_CAN_CASTLE << nibbleShift;
			}
			else {
				nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_ROOK_CAN_NOT_CASTLE << nibbleShift;
			}
		}
		else if ((mask & board[IDX_BLACK_KNIGHTS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_KNIGHT << nibbleShift;
		}
		else if ((mask & board[IDX_BLACK_BISHOPS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_BISHOP << nibbleShift;
		}
		else if ((mask & board[IDX_BLACK_QUEENS]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_QUEEN << nibbleShift;
		}
		else if ((mask & board[IDX_BLACK_KING]) == mask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_BLACK_KING << nibbleShift;
		}

		if (mask == epMask) {
			nibbleBinary[numPieces >> 1] |= NIBBLE_EP_PAWN << nibbleShift;
		}

		if (((*allPieces | epMask) & mask) != 0) {
			numPieces++;
		}

		nibbleShift = (numPieces % 2) << 2;
		mask <<= 1;
	}


	nibbleBinary[24] = board[IDX_MOVE_NUM] & 0xff;

	unsigned long *multi = (unsigned long *) &nibbleBinary[25];
	multi[0] = MULTIPLIER;// board[IDX_MULTIPLIER];

}

void nibbleBinaryToBitBoard(unsigned char nibbleBinary[], unsigned long *board) {
	// TODO: dette.
	// fint å fått kombinert noen resultater og startet derfra.
	// level N here we comes.

	memset(board, 0, NUM_BYTES * 8);

	unsigned long *allPiecesAndEp = (unsigned long *) &nibbleBinary[0]; // 8 bytes allpiece


	unsigned long mask = 1L;
	int pieceNum = 0;
	int nibbleShift = 0;
	for (int i = 0; i < 64; i++) {
		if ((mask & *allPiecesAndEp) == mask) {
			int piece = ((nibbleBinary[8 + (pieceNum >> 1)]) >> nibbleShift) & 0xF;

			switch (piece) {

				case NIBBLE_WHITE_PAWN:
					board[IDX_WHITE_PAWNS] |= mask;
					break;
				case NIBBLE_WHITE_ROOK_CAN_CASTLE:
					board[IDX_WHITE_ROOKS] |= mask;
					if (mask == A1_MASK) {
						board[IDX_CASTLING] |= MASK_CASTLING_WHITE_QUEEN_SIDE;
					}
					else if (mask == H1_MASK) {
						board[IDX_CASTLING] |= MASK_CASTLING_WHITE_KING_SIDE;
					}
					break;
				case NIBBLE_WHITE_ROOK_CAN_NOT_CASTLE:
					board[IDX_WHITE_ROOKS] |= mask;
					break;
				case NIBBLE_WHITE_KNIGHT:
					board[IDX_WHITE_KNIGHTS] |= mask;
					break;
				case NIBBLE_WHITE_BISHOP:
					board[IDX_WHITE_BISHOPS] |= mask;
					break;
				case NIBBLE_WHITE_QUEEN:
					board[IDX_WHITE_QUEENS] |= mask;
					break;
				case NIBBLE_WHITE_KING:
					board[IDX_WHITE_KING] |= mask;
					break;


				case NIBBLE_BLACK_PAWN:
					board[IDX_BLACK_PAWNS] |= mask;
					break;
				case NIBBLE_BLACK_ROOK_CAN_CASTLE:
					board[IDX_BLACK_ROOKS] |= mask;
					if (mask == A8_MASK) {
						board[IDX_CASTLING] |= MASK_CASTLING_BLACK_QUEEN_SIDE;
					}
					else if (mask == H8_MASK) {
						board[IDX_CASTLING] |= MASK_CASTLING_BLACK_KING_SIDE;
					}
					break;
				case NIBBLE_BLACK_ROOK_CAN_NOT_CASTLE:
					board[IDX_BLACK_ROOKS] |= mask;
					break;
				case NIBBLE_BLACK_KNIGHT:
					board[IDX_BLACK_KNIGHTS] |= mask;
					break;
				case NIBBLE_BLACK_BISHOP:
					board[IDX_BLACK_BISHOPS] |= mask;
					break;
				case NIBBLE_BLACK_QUEEN:
					board[IDX_BLACK_QUEENS] |= mask;
					break;
				case NIBBLE_BLACK_KING:
					board[IDX_BLACK_KING] |= mask;
					break;

				case NIBBLE_EP_PAWN:
					if ((mask & R4) == mask) {
						board[IDX_WHITE_PAWNS] |= mask;
						board[IDX_EP_IDX] = mask >> 8;
					}
					else if ((mask & R5) == mask) {
						board[IDX_BLACK_PAWNS] |= mask;
						board[IDX_EP_IDX] = mask << 8;
					}
					break;
			}

			pieceNum++;
			nibbleShift = (pieceNum % 2) << 2;

		}


		mask <<= 1L;
	}

	board[IDX_WHITE_PIECES] = board[IDX_WHITE_PAWNS] |
							  board[IDX_WHITE_ROOKS] |
							  board[IDX_WHITE_KNIGHTS] |
							  board[IDX_WHITE_BISHOPS] |
							  board[IDX_WHITE_QUEENS] |
							  board[IDX_WHITE_KING];


	board[IDX_BLACK_PIECES] = board[IDX_BLACK_PAWNS] |
							  board[IDX_BLACK_ROOKS] |
							  board[IDX_BLACK_KNIGHTS] |
							  board[IDX_BLACK_BISHOPS] |
							  board[IDX_BLACK_QUEENS] |
							  board[IDX_BLACK_KING];

	board[IDX_ALL_PIECES] = board[IDX_WHITE_PIECES] | board[IDX_BLACK_PIECES];

	board[IDX_MOVE_NUM] = nibbleBinary[24];

	unsigned long *multi = (unsigned long *) &nibbleBinary[25];
	board[IDX_MULTIPLIER] = *multi;

	board[IDX_TURN] = (nibbleBinary[24] % 2) == 0 ? WHITE_MASK : BLACK_MASK;


}


void diagramToBitBoard(unsigned long *board, char diagram[]) {

	//
	//		A B C D E F G H
	// 8	0,0,0,0,0,0,0,0,
	// 7	0,0,0,0,0,0,0,0,
	// 6	0,0,0,0,0,0,0,0,
	// 5	0,0,0,0,0,0,0,0,
	// 4	0,0,0,0,0,0,0,0,
	// 3	0,0,0,0,0,0,0,0,
	// 2	0,0,0,0,0,0,0,0,
	// 1	0,0,0,0,0,0,0,0,

	int len = strlen(diagram);

	memset(board, 0, sizeof(unsigned long) * NUM_BYTES);

	board[IDX_CASTLING] = 0;
	board[IDX_EP_IDX] = 0;
	board[IDX_TURN] = WHITE_MASK;
	board[IDX_MULTIPLIER] = 1;
	int pos = 0;
	int lastLenUsed = 0;

	for (int t = 0; t < len; t++) {
		switch (diagram[t]) {
			// BLACK PIECES
			case '.':
				pos++;
				break;
			case 'p':
				board[IDX_BLACK_PAWNS] |= (1ul << (63ul - pos));
				pos++;
				break;
			case 'r':
				board[IDX_BLACK_ROOKS] |= (1ul << (63 - pos));
				pos++;
				break;
			case 'b':
				board[IDX_BLACK_BISHOPS] |= (1ul << (63 - pos));
				pos++;
				break;
			case 'n':
				board[IDX_BLACK_KNIGHTS] |= (1ul << (63 - pos));
				pos++;
				break;
			case 'q':
				board[IDX_BLACK_QUEENS] |= (1ul << (63 - pos));
				pos++;
				break;
			case 'k':
				board[IDX_BLACK_KING] = (1ul << (63 - pos));
				pos++;
				break;
				// WHITE PIECES
			case 'P':
				board[IDX_WHITE_PAWNS] |= (1ul << (63 - pos));
				pos++;
				break;
			case 'R':
				board[IDX_WHITE_ROOKS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'N':
				board[IDX_WHITE_KNIGHTS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'B':
				board[IDX_WHITE_BISHOPS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'Q':
				board[IDX_WHITE_QUEENS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'K':
				board[IDX_WHITE_KING] = (1l << (63 - pos));
				pos++;
				break;

		}// switch



		if (pos > 63) {
			lastLenUsed = t + 1;
			break;
		}

	} // 0..len

	board[IDX_WHITE_PIECES] = board[IDX_WHITE_PAWNS] |
							  board[IDX_WHITE_ROOKS] |
							  board[IDX_WHITE_KNIGHTS] |
							  board[IDX_WHITE_BISHOPS] |
							  board[IDX_WHITE_QUEENS] |
							  board[IDX_WHITE_KING];

	board[IDX_BLACK_PIECES] = board[IDX_BLACK_PAWNS] |
							  board[IDX_BLACK_ROOKS] |
							  board[IDX_BLACK_KNIGHTS] |
							  board[IDX_BLACK_BISHOPS] |
							  board[IDX_BLACK_QUEENS] |
							  board[IDX_BLACK_KING];

	board[IDX_ALL_PIECES] = board[IDX_WHITE_PIECES] |
							board[IDX_BLACK_PIECES];


	board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board) | calculateBlackKingCheckStatus(board);

	int mode = 0; // looking for turn character
	int modeTurnDone = 0;
	int modeCastlingDone = 0;
	int modeEnPassSquareDone = 0;


	char str[strlen(diagram)];
	sprintf(str, "%s", diagram + lastLenUsed);

	char delim[] = " \t\n";
	char *ptr = strtok(str, delim);

	int arg = 0;
	while (ptr != NULL) {

		if (arg == 0) {
			if (strcmp(ptr, "w") == 0) {
				board[IDX_TURN] = WHITE_MASK;
			}
			else if (strcmp(ptr, "b") == 0) {
				board[IDX_TURN] = BLACK_MASK;
			}
		}
		else if (arg == 1) {
			if (strchr(ptr, 'K') != NULL) {
				board[IDX_CASTLING] |= MASK_CASTLING_WHITE_KING_SIDE;
			}

			if (strchr(ptr, 'k') != NULL) {
				board[IDX_CASTLING] |= MASK_CASTLING_BLACK_KING_SIDE;
			}

			if (strchr(ptr, 'Q') != NULL) {
				board[IDX_CASTLING] |= MASK_CASTLING_WHITE_QUEEN_SIDE;
			}

			if (strchr(ptr, 'q') != NULL) {
				board[IDX_CASTLING] |= MASK_CASTLING_BLACK_QUEEN_SIDE;
			}
		}
		else if (arg == 2) {

			if (ptr[0] >= 'a' && ptr[0] <= 'h') {
				int rank = (ptr[1] - 49);
				if (rank == 2 || rank == 5) {
					board[IDX_EP_IDX] = 1L << (7 - (ptr[0] - 97) + (rank << 3));
				}
			}
		}
		else if (arg == 3) {
			board[IDX_MOVE_NUM] = atoi(ptr);
		}

		ptr = strtok(NULL, delim);
		arg++;
	}


} // diagramToBitBoard


void compressBitBoard(unsigned long *board) {

	char compressedBoard[] = "................................................................\0";

	unsigned long idx = 1L << 63;
	int positionCounter = 0;
	int repeatCount = 0;
	char lastPiece = ' ';
	char currentPiece = ' ';

	for (int t = 0; t < 64; t++) {

		if (board[IDX_BLACK_PAWNS] & idx) {
			currentPiece = 'p';
		}
		else if (board[IDX_BLACK_ROOKS] & idx) {
			currentPiece = 'r';
		}
		else if (board[IDX_BLACK_KNIGHTS] & idx) {
			currentPiece = 'n';
		}
		else if (board[IDX_BLACK_BISHOPS] & idx) {
			currentPiece = 'b';
		}
		else if (board[IDX_BLACK_QUEENS] & idx) {
			currentPiece = 'q';
		}
		else if (board[IDX_BLACK_KING] & idx) {
			currentPiece = 'k';
		}
		else if (board[IDX_WHITE_PAWNS] & idx) {
			currentPiece = 'P';
		}
		else if (board[IDX_WHITE_ROOKS] & idx) {
			currentPiece = 'R';
		}
		else if (board[IDX_WHITE_KNIGHTS] & idx) {
			currentPiece = 'N';
		}
		else if (board[IDX_WHITE_BISHOPS] & idx) {
			currentPiece = 'B';
		}
		else if (board[IDX_WHITE_QUEENS] & idx) {
			currentPiece = 'Q';
		}
		else if (board[IDX_WHITE_KING] & idx) {
			currentPiece = 'K';
		}
		else {
			currentPiece = 'e';
		}

		if (currentPiece != lastPiece && lastPiece != ' ') {
			if (repeatCount > 9) {
				compressedBoard[positionCounter] = 48 + (repeatCount / 10);
				positionCounter++;
			}
			if (repeatCount > 1) {
				compressedBoard[positionCounter] = 48 + (repeatCount % 10);
				positionCounter++;
				repeatCount = 1;
			}
			compressedBoard[positionCounter] = lastPiece;
			positionCounter++;
			lastPiece = currentPiece;
		}
		else {
			repeatCount++;
			lastPiece = currentPiece;
		}

		idx >>= 1L;

	}

	if (repeatCount > 1) {
		if (repeatCount > 9) {
			compressedBoard[positionCounter] = 48 + (repeatCount / 10);
			positionCounter++;
		}

		compressedBoard[positionCounter] = 48 + (repeatCount % 10);
		positionCounter++;

		compressedBoard[positionCounter] = lastPiece;
		positionCounter++;
	}
	else if (repeatCount == 1) {
		compressedBoard[positionCounter] = lastPiece;
		positionCounter++;
	}


	int KQkq = 0;
	KQkq = (MASK_CASTLING_BLACK_KING_SIDE & board[IDX_CASTLING]) == MASK_CASTLING_BLACK_KING_SIDE ? 1 : 0;
	KQkq |= (MASK_CASTLING_BLACK_QUEEN_SIDE & board[IDX_CASTLING]) == MASK_CASTLING_BLACK_QUEEN_SIDE ? 2 : 0;
	KQkq |= (MASK_CASTLING_WHITE_KING_SIDE & board[IDX_CASTLING]) == MASK_CASTLING_WHITE_KING_SIDE ? 4 : 0;
	KQkq |= (MASK_CASTLING_WHITE_QUEEN_SIDE & board[IDX_CASTLING]) == MASK_CASTLING_WHITE_QUEEN_SIDE ? 8 : 0;

	if (KQkq > 9) {
		compressedBoard[positionCounter] = 48 + (KQkq / 10);
		positionCounter++;
	}
	compressedBoard[positionCounter] = 48 + (KQkq % 10);
	positionCounter++;


	compressedBoard[positionCounter] = '-';
	if (board[IDX_EP_IDX] != 0) {
		int idx = __builtin_ctzll(board[IDX_EP_IDX]);
		int rank = (idx >> 3);
		int file = (7 - (idx & 7));
		compressedBoard[positionCounter] = 97 + file;
		compressedBoard[positionCounter + 1] = 49 + rank;
		positionCounter++;
	}
	positionCounter++;

	compressedBoard[positionCounter] = board[IDX_TURN] == WHITE_MASK ? 'w' : 'b';
	positionCounter++;

	if (board[IDX_MOVE_NUM] > 9) {
		compressedBoard[positionCounter] = 48 + (board[IDX_MOVE_NUM] / 10);
		positionCounter++;
	}

	compressedBoard[positionCounter] = 48 + (board[IDX_MOVE_NUM] % 10);
	positionCounter++;

	compressedBoard[positionCounter] = 'm';
	positionCounter++;

	unsigned long mul = board[IDX_MULTIPLIER];
	char digits[] = "xxxxxxxxxxxxxx";
	int md = 0;
	while (mul > 0) {
		digits[md] = 48 + (char) (mul % 10);
		mul /= 10;
		md++;
	}
	for (int t = 0; t < md; t++) {
		compressedBoard[positionCounter] = digits[md - t - 1];
		positionCounter++;
	}

	compressedBoard[positionCounter] = '\n';
	positionCounter++;

	if (outFile != NULL) {
		memcpy(outFileBuff + outFileBuffOffset, compressedBoard, positionCounter);
		outFileBuffOffset += positionCounter;
		buffWrites++;
		if (outFileBuffOffset > 1023 * 1024) {
			fileWrites++;
			fwrite(outFileBuff, 1, outFileBuffOffset, outFile);
			outFileBuffOffset = 0;
		}

	}
	else {
		printf("%s", compressedBoard);
	}

}


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


// Logic for testing threats in quadrants
/*


if( board[IDX_WHITE_KING] > (board[IDX_BLACK_QUEENS] & qbAttackMap ) ){
	if( (QB_ATTACK_MAPS[kingIndex] & board[IDX_BLACK_QUEENS]) > (QB_ATTACK_MAPS[kingIndex] & board[IDX_ALL_PIECES] & ~board[IDX_BLACK_QUEENS] )) {
		printf( "White king is in check, bit test 1.\n");
	}
}

*/

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
// 1. Log discovery checks, double checks
//    - make extra function to check for how the move affected the other king, which includes moved piece
// DONE: log stalemate
// 2. Implement ^ for moving, instead of &~ and then |. Do it in makeBlack/WhiteMove
// 3. DONE always exopects a file input
// 4. on check on last level only check until the first valid move is found
// 5. check stalemate counting for with and black maybe bugging for black
// 6. rewrite diagram, create new for level 6, include multiplier

void diagramToByteBoard(unsigned long board[], char diagram[]);

void printDiagram(unsigned long board[]);

void printNumBoard(unsigned long board[]);

unsigned long printStats();

void dig(unsigned long board[]);

void count(unsigned long board[]);

// bitboard functions, highly speed sensitive
void makeNewBoard(unsigned long originalBoard[], unsigned long newBoard[]);

int calculateWhiteKingCheckStatus(unsigned long board[]);
int calculateBlackKingCheckStatus(unsigned long board[]);

unsigned long calculateWhiteKingCheckStatus2( unsigned long board[], unsigned long lastMoveMap );
unsigned long calculateBlackKingCheckStatus2( unsigned long board[], unsigned long lastMoveMap );

int findAllPossibleMoves2(unsigned long originalBoard[]);

int moveWhitePawns(unsigned long b[]);

int moveWhiteRooksOrQueens(unsigned long b[], int pieceMapIndex);

int moveWhiteKnights(unsigned long b[]);

int moveWhiteBishopsOrQueens(unsigned long b[], int pieceMapIndex);

int moveWhiteKing(unsigned long b[]);

int moveBlackPawns(unsigned long b[]);

int moveBlackRooksOrQueens(unsigned long b[], int pieceMapIndex);

int moveBlackKnights(unsigned long b[]);

int moveBlackBishopsOrQueens(unsigned long b[], int pieceMapIndex);

int moveBlackKing(unsigned long b[]);

void makeWhiteMove(unsigned long move[], int pieceIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long blackPieces);

void makeBlackMove(unsigned long move[], int pieceIndex, unsigned long moveToMap, unsigned long clearMap,
				   unsigned long blackPieces);

void clearBlackPiecesWithClearMap(unsigned long board[], unsigned long clear);

void clearWhitePiecesWithClearMap(unsigned long board[], unsigned long clear);

void makeWhiteBitPromos(unsigned long board[], unsigned long map);

void makeBlackBitPromos(unsigned long board[], unsigned long map);

// util for board printing and conversion.
void setBitsToChar(char *str, unsigned long bits, char c);

void sprintDiagram(char *target, unsigned long board[] );

void printBitBoard(unsigned long board[]);

void diagramToBitBoard(unsigned long board[], char diagram[]);

void printLongAsBitBoard(unsigned long bitstream);

void printCompactBoard(unsigned long board[]);


/*** LEVEL ***/
/*** LEVEL ***/
/*** LEVEL ***/

int MAX_LEVEL = 5	;

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

int main( int argc, char **argv){

	// "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR"

	/*char *initialBoard = "\
						 r n b q k b n r\
						 p p p p p p p p\
						 . . . . . . . .\
						 . . . . . . . .\
						 . . . . . . . .\
						 . . . . . . . .\
						 P P P P P P P P\
						 R N B Q K B N R";*/

	//*
	char *initialBoard = "\
                      r . . . k . . r\
                      p . p p q p b .\
                      b n . . p n p .\
                      . . . P N . . .\
                      . p . . P . . .\
                      . . N . . Q . p\
                      P P P B B P P P\
                      R . . . K . . R";


	unsigned long board[NUM_BYTES];



	if( argc > 1 ){
		if( strcmp(argv[1],"SLEEP") == 0 ){
			printf("SLEEPING");
			fflush(stdout);
			sleep(60);
			return 0;
		}
		else if( strcmp(argv[1],"-h") == 0 ){
			printf("** Borgsum Chess Engine (fast, but stupid version) **\r\n");
			printf("Arguments:\r\n");
			printf("1 : board diagram enclosed in \"\r\n");
			printf("2 : turn b or w\r\n");
			printf("3 : castling rights, KQkq or - if no castling rights are left\r\n" );
			printf("4 : max level recursion, integer\r\n");
			printf("5 : workunit id\r\n");
			printf("\r\nandreasoverland@gmail.com\r\n");
			fflush(stdout);
			exit(0);
		}
		initialBoard = argv[1];
	}

	diagramToBitBoard( board, initialBoard);
	

	if( argc > 2 ){
		if( strcmp(argv[2], "b" ) == 0 ){
			board[IDX_TURN] = BLACK_MASK;
		}
		else {
			board[IDX_TURN] = WHITE_MASK;
		}
	}

	if( argc > 3 ){
		board[IDX_CASTLING] = 0;

		if( strchr(argv[3],'K') != NULL ){
			board[IDX_CASTLING] |= MASK_CASTLING_WHITE_KING_SIDE;
		}

		if( strchr(argv[3],'k') != NULL ){
			board[IDX_CASTLING] |= MASK_CASTLING_BLACK_KING_SIDE;
		}

		if( strchr(argv[3],'Q') != NULL ){
			board[IDX_CASTLING] |= MASK_CASTLING_WHITE_QUEEN_SIDE;
		}

		if( strchr(argv[3],'q') != NULL ){
			board[IDX_CASTLING] |= MASK_CASTLING_BLACK_QUEEN_SIDE;
		}
	}

	if( argc > 4 ){
		MAX_LEVEL = atoi(argv[4]);
	}

	if( argc > 5 ){
		workUnitId = argv[5];
		printf("ID:\"%s\" %s %s %s %s\n", argv[1],argv[2],argv[3],argv[4],argv[5] );
	}

	if( argc == 1){
		printf("run this with arguments :\r\n");
		char line[120];
		sprintDiagram(line, board);
		printf("./chessengine %s %d\n", line, MAX_LEVEL  );
	}

	printBitBoard( board );

	struct timespec ts1, ts2;
	clock_gettime(CLOCK_REALTIME, &ts1);

	dig(board);

	clock_gettime(CLOCK_REALTIME, &ts2);
	if (ts2.tv_nsec < ts1.tv_nsec) {
		ts2.tv_nsec += 1000000000;
		ts2.tv_sec--;
	}

	int l = 14;

	long total = printStats();

	printf("\nTotal valid moves found : %lu \n" ,total);

	printf("Time spent: %ld.%09ld \n", (long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);

	return 0;
} // end main

unsigned long printStats() {

	/*printf("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
		   "Depth",
		   "Nodes",
		   "Caps",
		   "E.p.",
		   "Castles",
		   "Promos",
		   "Checks",
		   "Disc.Chk",
		   "Dbl.Chk"
		   "Mates",
		   "S.mates"
	);*/

	static unsigned long total = 0;

	for (int t = 0; t < MAX_LEVEL + 1; t++) {
		printf("%d", t);
		printf(", N: %lu", numMoves[t]);
		printf(", Caps: %lu", numCaptures[t]);
		printf(", EP: %lu", numEP[t]);
		printf(", Cast: %lu", numCastles[t]);
		printf(", Prom: %lu", numPromos[t]);
		printf(", Checks: %lu", numChecks[t]);
		printf(", Disc: %lu", numDiscoveryChecks[t]);
		printf(", Disc(p): %lu", numDiscoveryPromoChecks[t]);
		printf(", Disc(c): %lu", numDiscoveryCaptureChecks[t]);
		printf(", Disc(ep): %lu", numDiscoveryEPChecks[t]);
		printf(", Dbl: %lu", numDoubleChecks[t]);
		printf(", Dbl(p) : %lu", numDoublePromoChecks[t] );
		printf(", Dbl(c) : %lu", numDoubleCaptureChecks[t] );
		printf(", Dbl(ep) : %lu", numDoubleEPChecks[t] );
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

void dig(unsigned long board[]) {

	if (board[IDX_MOVE_NUM] < MAX_LEVEL
		|| (board[IDX_MOVE_NUM] == MAX_LEVEL &&
			board[IDX_CHECK_STATUS] != 0) // Include to test for mates on the last level
			) {
		int numMoves = findAllPossibleMoves2(board);

		if (numMoves == 0 && board[IDX_CHECK_STATUS] != 0) {
			board[IDX_CHECK_STATUS] |= MASK_KING_IS_MATED;
		}
		if (numMoves == 0 && board[IDX_CHECK_STATUS] == 0) {
			board[IDX_CHECK_STATUS] = MASK_KING_IS_STALEMATED;
		}
	}

	count(board);

}

int findAllPossibleMoves2(unsigned long originalBoard[]) {

	int numMovesFound = 0;

	if (originalBoard[IDX_TURN] == WHITE_MASK) { // turn == white

		numMovesFound += moveWhitePawns(originalBoard);
		numMovesFound += moveWhiteRooksOrQueens(originalBoard, IDX_WHITE_QUEENS);
		numMovesFound += moveWhiteRooksOrQueens(originalBoard, IDX_WHITE_ROOKS);
		numMovesFound += moveWhiteBishopsOrQueens(originalBoard, IDX_WHITE_QUEENS);
		numMovesFound += moveWhiteBishopsOrQueens(originalBoard, IDX_WHITE_BISHOPS);
		numMovesFound += moveWhiteKnights(originalBoard);
		numMovesFound += moveWhiteKing(originalBoard);

	} else {

		numMovesFound += moveBlackPawns(originalBoard);
		numMovesFound += moveBlackRooksOrQueens(originalBoard, IDX_BLACK_QUEENS);
		numMovesFound += moveBlackRooksOrQueens(originalBoard, IDX_BLACK_ROOKS);
		numMovesFound += moveBlackBishopsOrQueens(originalBoard, IDX_BLACK_QUEENS);
		numMovesFound += moveBlackBishopsOrQueens(originalBoard, IDX_BLACK_BISHOPS);
		numMovesFound += moveBlackKnights(originalBoard);
		numMovesFound += moveBlackKing(originalBoard);

	}
	return numMovesFound;
}


// ----------------------------------- WHITE KING ---------------------------------

int moveWhiteKing(unsigned long b[]) {

	int numMovesFound = 0;

	unsigned long pieceMap = b[IDX_WHITE_KING];

	int idx = __builtin_ctzll(pieceMap);

	unsigned long move[NUM_BYTES];
	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	unsigned long clearMap = ~pieceMap;
	unsigned long moveToMaps = KING_ATTACK_MAPS[idx];

	moveToMaps &= ~whitePieces;
	int moveToIdx = 0;

	while (moveToMaps) {

		int moveToShift = __builtin_ctzll(moveToMaps);
		moveToIdx += moveToShift;

		unsigned long moveToMap = 1l << moveToIdx;

		makeNewBoard(b, move);
		makeWhiteMove(move, IDX_WHITE_KING, moveToMap, clearMap, blackPieces);
		move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_KING_SIDE | MASK_CASTLING_WHITE_QUEEN_SIDE);

		if (calculateWhiteKingCheckStatus(move) == 0) {
			calculateBlackKingCheckStatus2(move,moveToMap);
			numMovesFound++;
			dig(move);
		}

		moveToMaps >>= moveToShift;
		moveToMaps >>= 1;
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
						makeNewBoard(b, move);
						makeWhiteMove(move, IDX_WHITE_KING, C1_MASK, clearMap, blackPieces);
						makeWhiteMove(move, IDX_WHITE_ROOKS, D1_MASK, ~A1_MASK, blackPieces);
						calculateBlackKingCheckStatus2(move,C1_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
						numMovesFound++;
						dig(move);
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
						makeNewBoard(b, move);
						makeWhiteMove(move, IDX_WHITE_KING, G1_MASK, clearMap, blackPieces);
						makeWhiteMove(move, IDX_WHITE_ROOKS, F1_MASK, ~H1_MASK, blackPieces);
						calculateBlackKingCheckStatus2(move,G1_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
						numMovesFound++;
						dig(move);
					}
				}
			}
			b[IDX_WHITE_KING] = pieceMap;
		}
	}

	return numMovesFound;

}


// ----------------------------------- BLACK KING ---------------------------------

int moveBlackKing(unsigned long b[]) {

	int numMovesFound = 0;

	unsigned long pieceMap = b[IDX_BLACK_KING];

	int idx = __builtin_ctzll(pieceMap);

	unsigned long move[NUM_BYTES];
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

		makeNewBoard(b, move);
		makeBlackMove(move, IDX_BLACK_KING, moveToMap, clearMap, whitePieces);
		move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_KING_SIDE | MASK_CASTLING_BLACK_QUEEN_SIDE);

		if (calculateBlackKingCheckStatus(move) == 0) {
			calculateWhiteKingCheckStatus2(move,moveToMap);
			numMovesFound++;
			dig(move);
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
						makeNewBoard(b, move);
						makeBlackMove(move, IDX_BLACK_KING, C8_MASK, clearMap, whitePieces);
						makeBlackMove(move, IDX_BLACK_ROOKS, D8_MASK, ~A8_MASK, whitePieces);
						calculateWhiteKingCheckStatus2(move,C8_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
						numMovesFound++;
						dig(move);
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
						makeNewBoard(b, move);
						makeBlackMove(move, IDX_BLACK_KING, G8_MASK, clearMap, whitePieces);
						makeBlackMove(move, IDX_BLACK_ROOKS, F8_MASK, ~H8_MASK, whitePieces);
						calculateWhiteKingCheckStatus2(move,G8_MASK);
						move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
						numMovesFound++;
						dig(move);
					}
				}
			}
			b[IDX_BLACK_KING] = pieceMap;
		}

	}

	return numMovesFound;

}


//////////////////////////////////// MOVE WHITE PAWN //////////////
int moveWhitePawns(unsigned long b[]) {

	int numPawnMoves = 0;

	unsigned long pawns = b[IDX_WHITE_PAWNS];
	unsigned long originalPawns = pawns;
	unsigned long blackPieces = b[IDX_BLACK_PIECES];

	// 1. Make moves for pawns that can move ONE forward
	// 2. Make moves for pawns that can move TWO forward and set EP index/map
	// 3. Make capturing moves for pawns with WHITE_PAWN_ATTACK_MAPS
	// 4. Make en passant captures for pawns with WHITE_PAWN_ATTACK_MAPS that matches EP Index/map

	unsigned long move[NUM_BYTES];

	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long pawnsThatCanMoveOneForward = pawns & ~(allPieces >> 8);
	unsigned long pawnsThatCanMoveTwoForward = pawnsThatCanMoveOneForward & R2;
	pawnsThatCanMoveTwoForward &= ~(allPieces >> 16);

	int pieceIdx = 0;

	while (pawnsThatCanMoveOneForward) {
		int shift = __builtin_ctzll(pawnsThatCanMoveOneForward);
		pieceIdx += shift;
		unsigned long pieceMap = 1l << pieceIdx;
		unsigned long moveToMap = pieceMap << 8;
		unsigned long clearMap = ~pieceMap;
		makeNewBoard(b, move);
		makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);

		if (calculateWhiteKingCheckStatus(move) == 0) {
			if (moveToMap & R8) {
				move[IDX_CASTLING] &= ~moveToMap;
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO;
				numPawnMoves += 4;
				makeWhiteBitPromos(move, moveToMap);
			} else {
				calculateBlackKingCheckStatus2(move,moveToMap);
				numPawnMoves++;
				dig(move);
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

		makeNewBoard(b, move);
		makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);
		move[IDX_EP_IDX] = pieceMap << 8;

		if (calculateWhiteKingCheckStatus(move) == 0) {
			calculateBlackKingCheckStatus2(move,moveToMap);
			numPawnMoves++;
			dig(move);
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

				makeNewBoard(b, move);
				makeWhiteMove(move, IDX_WHITE_PAWNS, moveToMap, clearMap, blackPieces);

				if (calculateWhiteKingCheckStatus(move) == 0) {

					if (moveToMap & R8) {
						move[IDX_CASTLING] &= ~moveToMap;
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO | MASK_LAST_MOVE_WAS_CAPTURE;
						numPawnMoves += 4;
						makeWhiteBitPromos(move, moveToMap);
					} else {
						calculateBlackKingCheckStatus2(move,moveToMap);
						numPawnMoves++;
						dig(move);
					}

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
					makeNewBoard(b, move);
					clearBlackPiecesWithClearMap(move, ~(epMap >> 8));
					move[IDX_WHITE_PAWNS] &= ~attackingPawn;
					move[IDX_WHITE_PIECES] &= ~attackingPawn;
					move[IDX_WHITE_PAWNS] |= epMap;
					move[IDX_WHITE_PIECES] |= epMap;
					move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | move[IDX_BLACK_PIECES];
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

					if (calculateWhiteKingCheckStatus(move) == 0) {
						calculateBlackKingCheckStatus2(move,epMap);
						numPawnMoves++;
						dig(move);
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

int moveBlackPawns(unsigned long b[]) {

	int numPawnMoves = 0;

	unsigned long pawns = b[IDX_BLACK_PAWNS];
	unsigned long originalPawns = pawns;
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	// 1. Make moves for pawns that can move ONE forward
	// 2. Make moves for pawns that can move TWO forward and set EP index/map
	// 3. Make capturing moves for pawns with BLACK_PAWN_ATTACK_MAPS
	// 4. Make en passant captures for pawns with BLACK_PAWN_ATTACK_MAPS that matches EP Index/map

	unsigned long move[NUM_BYTES];

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
		makeNewBoard(b, move);
		makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);

		if (calculateBlackKingCheckStatus(move) == 0) {
			if (moveToMap & R1) {
				move[IDX_CASTLING] &= ~moveToMap;
				numPawnMoves += 4;
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO;
				makeBlackBitPromos(move, moveToMap);
			} else {
				calculateWhiteKingCheckStatus2(move,moveToMap);
				numPawnMoves++;
				dig(move);
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
		unsigned long moveToMap = pieceMap >> 16;
		unsigned long clearMap = ~pieceMap;

		makeNewBoard(b, move);
		makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);
		move[IDX_EP_IDX] = pieceMap >> 8;

		if (calculateBlackKingCheckStatus(move) == 0) {
			calculateWhiteKingCheckStatus2(move,moveToMap);
			numPawnMoves++;
			dig(move);
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

				makeNewBoard(b, move);
				makeBlackMove(move, IDX_BLACK_PAWNS, moveToMap, clearMap, whitePieces);

				if (calculateBlackKingCheckStatus(move) == 0) {

					if (moveToMap & R1) {
						move[IDX_CASTLING] &= ~moveToMap;
						move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_PROMO | MASK_LAST_MOVE_WAS_CAPTURE;
						numPawnMoves += 4;
						makeBlackBitPromos(move, moveToMap);
					} else {
						calculateWhiteKingCheckStatus2(move,moveToMap);
						numPawnMoves++;
						dig(move);
					}
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
					makeNewBoard(b, move);
					clearWhitePiecesWithClearMap(move, ~(epMap << 8));
					move[IDX_BLACK_PAWNS] &= ~attackingPawn;
					move[IDX_BLACK_PIECES] &= ~attackingPawn;
					move[IDX_BLACK_PAWNS] |= epMap;
					move[IDX_BLACK_PIECES] |= epMap;
					move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES] | move[IDX_BLACK_PIECES];
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

					if (calculateBlackKingCheckStatus(move) == 0) {
						calculateWhiteKingCheckStatus2(move,epMap);
						numPawnMoves++;
						dig(move);
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
int moveWhiteBishopsOrQueens(unsigned long b[], int pieceMapIndex) {

	int numMovesFound = 0;

	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;
	int idx = 0;

	unsigned long move[NUM_BYTES];
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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
int moveBlackBishopsOrQueens(unsigned long b[], int pieceMapIndex) {

	int numMovesFound = 0;

	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;
	int idx = 0;

	unsigned long move[NUM_BYTES];
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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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

///////////////////////////////////// MOVE WHITE ROOKS //////////////////////////////////////
int moveWhiteRooksOrQueens(unsigned long b[], int pieceMapIndex) {

	int numMovesFound = 0;

	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;

	int idx = 0;

	unsigned long move[NUM_BYTES];
	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;

		unsigned long pieceMap = 1l << idx;
		// for hver piece. flytt h&v til vi treffe noe
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

			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);

				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);

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
			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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

			makeNewBoard(b, move);
			makeWhiteMove(move, pieceMapIndex, moveToMap, clearMap, blackPieces);

			if (calculateWhiteKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateBlackKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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
int moveBlackRooksOrQueens(unsigned long b[], int pieceMapIndex) {

	int numMovesFound = 0;

	unsigned long pieces = b[pieceMapIndex];
	unsigned long originalPieces = pieces;

	int idx = 0;

	unsigned long move[NUM_BYTES];
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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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

			makeNewBoard(b, move);
			makeBlackMove(move, pieceMapIndex, moveToMap, clearMap, whitePieces);

			if (calculateBlackKingCheckStatus(move) == 0) {
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
				calculateWhiteKingCheckStatus2(move,moveToMap);
				numMovesFound++;
				dig(move);
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
int moveWhiteKnights(unsigned long b[]) {

	int numMovesFound = 0;

	unsigned long pieces = b[IDX_WHITE_KNIGHTS];

	int idx = 0;

	unsigned long move[NUM_BYTES];
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

				makeNewBoard(b, move);
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
					calculateBlackKingCheckStatus2(move,moveToMap);
					numMovesFound++;
					dig(move);
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

int moveBlackKnights(unsigned long b[]) {

	int numMovesFound = 0;

	unsigned long pieces = b[IDX_BLACK_KNIGHTS];
	unsigned long originalPieces = pieces;

	int idx = 0;

	unsigned long move[NUM_BYTES];
	unsigned long allPieces = b[IDX_ALL_PIECES];
	unsigned long blackPieces = b[IDX_BLACK_PIECES];
	unsigned long whitePieces = b[IDX_WHITE_PIECES];

	while (pieces) {
		int shift = __builtin_ctzll(pieces);
		idx += shift;
		unsigned long pieceMap = 1l << idx;
		unsigned long clearMap = ~pieceMap;

		unsigned long moveToMaps = KNIGHT_ATTACK_MAPS[idx];
		moveToMaps &= ~blackPieces;

		int moveToIdx = 0;

		while (moveToMaps) {

			int moveToShift = __builtin_ctzll(moveToMaps);
			moveToIdx += moveToShift;

			unsigned long moveToMap = 1l << moveToIdx;
			if (!(moveToMap & blackPieces)) {

				makeNewBoard(b, move);
				makeBlackMove(move, IDX_BLACK_KNIGHTS, moveToMap, clearMap, whitePieces);

				if (calculateBlackKingCheckStatus(move) == 0) {
					move[IDX_CASTLING] &= ~(pieceMap | moveToMap);
					calculateWhiteKingCheckStatus2(move,moveToMap);
					numMovesFound++;
					dig(move);
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
void makeWhiteBitPromos(unsigned long board[], unsigned long newPieceMap) {

	unsigned long clearPiece = ~newPieceMap;
	board[IDX_WHITE_PAWNS] &= clearPiece;
	board[IDX_WHITE_ROOKS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_WHITE_ROOKS] &= clearPiece;
	board[IDX_WHITE_QUEENS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_WHITE_QUEENS] &= clearPiece;
	board[IDX_WHITE_BISHOPS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_WHITE_BISHOPS] &= clearPiece;
	board[IDX_WHITE_KNIGHTS] |= newPieceMap;
	calculateBlackKingCheckStatus2(board,newPieceMap);
	dig(board);

}

void makeBlackBitPromos(unsigned long board[], unsigned long newPieceMap) {

	unsigned long clearPiece = ~newPieceMap;

	board[IDX_BLACK_PAWNS] &= clearPiece;
	board[IDX_BLACK_ROOKS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_BLACK_ROOKS] &= clearPiece;
	board[IDX_BLACK_QUEENS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_BLACK_QUEENS] &= clearPiece;
	board[IDX_BLACK_BISHOPS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board,newPieceMap);
	dig(board);

	board[IDX_BLACK_BISHOPS] &= clearPiece;
	board[IDX_BLACK_KNIGHTS] |= newPieceMap;
	calculateWhiteKingCheckStatus2(board,newPieceMap);
	dig(board);

}

void makeNewBoard(unsigned long oldBoard[], unsigned long newBoard[]) {

	makeNewBoardInvocations++;

	memcpy(newBoard, oldBoard, sizeof(unsigned long) * NUM_BYTES_TO_COPY);
	newBoard[IDX_LAST_MOVE_WAS] = 0;
	newBoard[IDX_EP_IDX] = 0;

	newBoard[IDX_MOVE_ID] = makeNewBoardInvocations;
	newBoard[IDX_PARENT_MOVE_ID] = oldBoard[IDX_MOVE_ID];
	newBoard[IDX_MOVE_NUM]++;
	newBoard[IDX_TURN] ^= TURN_INVERTER;
	newBoard[IDX_CHECK_STATUS] = 0;

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


int calculateWhiteKingCheckStatus(unsigned long board[]) {

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


		if( qb & QB_ATTACK_MAPS_2[idx] ){
			if (king < (qb & QB_ATTACK_MAPS_2[idx])) {
				if ( __builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]) <= __builtin_ctzll(QB_ATTACK_MAPS_2[idx] & allPieces) ) {
					return MASK_WHITE_KING_CHECKED;
				}
				if ((QB_ATTACK_MAPS_2[idx] & allPiecesExceptQB) == 0) {
					return MASK_WHITE_KING_CHECKED;
				}
			}
		}


		if (king < (qb & QB_ATTACK_MAPS_3[idx])) {
			if ( __builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]) <= __builtin_ctzll((QB_ATTACK_MAPS_3[idx]  & allPieces))) {
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
			if (  __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) < __builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
				return MASK_WHITE_KING_CHECKED;
			}
		}

		if (king < (qr & QR_ATTACK_MAPS_4[idx])) {
			if ( __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) < __builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))) {
				return MASK_WHITE_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR) == 0) {
				return MASK_WHITE_KING_CHECKED;
			}
		}

	}

	return 0;

}

int calculateBlackKingCheckStatus(unsigned long board[]) {

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


		if( qb & QB_ATTACK_MAPS_2[idx] ){
			if (king < (qb & QB_ATTACK_MAPS_2[idx])) {

				if ( __builtin_ctzll(qb & QB_ATTACK_MAPS_2[idx]) <= __builtin_ctzll(QB_ATTACK_MAPS_2[idx] & allPieces) ) {
					return MASK_BLACK_KING_CHECKED;
				}

				if ((QB_ATTACK_MAPS_2[idx] & allPiecesExceptQB) == 0) {
					return MASK_BLACK_KING_CHECKED;
				}
			}
		}


		if (king < (qb & QB_ATTACK_MAPS_3[idx])) {
			if ( __builtin_ctzll(qb & QB_ATTACK_MAPS_3[idx]) <= __builtin_ctzll((QB_ATTACK_MAPS_3[idx]  & allPieces))) {
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
			if (  __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]) < __builtin_ctzll((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR))) {
				return MASK_BLACK_KING_CHECKED;
			}
			if ((QR_ATTACK_MAPS_3[idx] & allPiecesExceptQR) == 0) {
				return MASK_BLACK_KING_CHECKED;
			}
		}

		if (king < (qr & QR_ATTACK_MAPS_4[idx])) {
			if ( __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) < __builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))) {
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
unsigned long calculateBlackKingCheckStatus2(unsigned long board[], unsigned long lastMoveMap ) {

	//board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
	//return 0;


	unsigned long king = board[IDX_BLACK_KING];
	unsigned long idx = __builtin_ctzll(king);

	unsigned long threat = 0;

	threat = KNIGHT_ATTACK_MAPS[idx] & board[IDX_WHITE_KNIGHTS];
	threat |=  BLACK_PAWN_ATTACK_MAPS[idx] & board[IDX_WHITE_PAWNS];

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
			if( qb & QB_ATTACK_MAPS_2[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_2[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_2[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll( qb & QB_ATTACK_MAPS_2[idx] );
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if( (allPieces & QB_ATTACK_MAPS_2[idx] ) == (qb & QB_ATTACK_MAPS_2[idx]) ){
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll( allPiecesExceptQB & QB_ATTACK_MAPS_2[idx] );
						if( test < apnq ){
							threat |= 1L << test;
						}
					}

				}
			}

			// up right
			if( qb & QB_ATTACK_MAPS_3[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_3[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_3[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll( qb & QB_ATTACK_MAPS_3[idx] );
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if( (allPieces & QB_ATTACK_MAPS_3[idx] ) == (qb & QB_ATTACK_MAPS_3[idx]) ){
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll( allPiecesExceptQB & QB_ATTACK_MAPS_3[idx] );
						if( test < apnq ){
							threat |= 1L << test;
						}
					}

				}
			}


			// down right
			if( qb & QB_ATTACK_MAPS_1[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_1[idx] ) == 1 ){
					threat |= qb & QB_ATTACK_MAPS_1[idx];
				}
				else {
					int test = __builtin_clzll( qb & QB_ATTACK_MAPS_1[idx] );

					if( (allPieces & QB_ATTACK_MAPS_1[idx] ) == (qb & QB_ATTACK_MAPS_1[idx]) ){
						threat |= 1L << ( 63-test);
					}
					else {
						// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
						int apnq = __builtin_clzll( allPiecesExceptQB & QB_ATTACK_MAPS_1[idx] );
						if( test < apnq ){
							threat |= 1L << ( 63 - test );
						}
					}
				}
			}
			// down left
			if( qb & QB_ATTACK_MAPS_4[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_4[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_4[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int apnq = __builtin_clzll( allPiecesExceptQB & QB_ATTACK_MAPS_4[idx] );
					int test = __builtin_clzll( qb & QB_ATTACK_MAPS_4[idx] );
					if( test < apnq ){
						threat |= 1L << ( 63 - test );
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

				if (__builtin_popcountll( qr & QR_ATTACK_MAPS_1[idx]) == 1 && __builtin_popcountll( allPieces & QR_ATTACK_MAPS_1[idx]) == 1) {
					threat |= qr & QR_ATTACK_MAPS_1[idx];
				} else if (__builtin_popcountll( qr & QR_ATTACK_MAPS_2[idx]) == 1 && __builtin_popcountll( allPieces & QR_ATTACK_MAPS_2[idx]) == 1  ) {
					threat |= qr & QR_ATTACK_MAPS_2[idx];
				} else {

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
					|| (QR_ATTACK_MAPS_3[idx]&allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]);
				}
			}

			if (king < (qr & QR_ATTACK_MAPS_4[idx])) {

				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_4[idx]&allPiecesExceptQR) == 0 ) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]);
				}
			}
		}
	}


	board[IDX_KING_THREATS] = threat;

	if( threat ){
		board[IDX_CHECK_STATUS] = MASK_BLACK_KING_CHECKED;

		if( !(threat & lastMoveMap) ){
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
		}

		if( __builtin_popcountll( threat ) > 1 ){
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
unsigned long calculateWhiteKingCheckStatus2(unsigned long board[], unsigned long lastMoveMap ) {

	//board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
	//return 0;

	unsigned long king = board[IDX_WHITE_KING];
	unsigned long idx = __builtin_ctzll(king);

	unsigned long threat = 0;

	threat = KNIGHT_ATTACK_MAPS[idx] & board[IDX_BLACK_KNIGHTS];
	threat |=  WHITE_PAWN_ATTACK_MAPS[idx] & board[IDX_BLACK_PAWNS];

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
			if( qb & QB_ATTACK_MAPS_2[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_2[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_2[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll( qb & QB_ATTACK_MAPS_2[idx] );
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if( (allPieces & QB_ATTACK_MAPS_2[idx] ) == (qb & QB_ATTACK_MAPS_2[idx]) ){
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll( allPiecesExceptQB & QB_ATTACK_MAPS_2[idx] );
						if( test < apnq ){
							threat |= 1L << test;
						}
					}

				}
			}

			// up right
			if( qb & QB_ATTACK_MAPS_3[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_3[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_3[idx];
				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int test = __builtin_ctzll( qb & QB_ATTACK_MAPS_3[idx] );
					// hvis de eneste brikkene på linka er Q eller B, ta den første som threat.
					if( (allPieces & QB_ATTACK_MAPS_3[idx] ) == (qb & QB_ATTACK_MAPS_3[idx]) ){
						threat |= 1L << test;
					}
					else {
						int apnq = __builtin_ctzll( allPiecesExceptQB & QB_ATTACK_MAPS_3[idx] );
						if( test < apnq ){
							threat |= 1L << test;
						}
					}

				}
			}


			// down right
			if( qb & QB_ATTACK_MAPS_1[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_1[idx] ) == 1 ){
					threat |= qb & QB_ATTACK_MAPS_1[idx];
				}
				else {
					int test = __builtin_clzll( qb & QB_ATTACK_MAPS_1[idx] );

					if( (allPieces & QB_ATTACK_MAPS_1[idx] ) == (qb & QB_ATTACK_MAPS_1[idx]) ){
						threat |= 1L << ( 63-test);
					}
					else {
						// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
						int apnq = __builtin_clzll( allPiecesExceptQB & QB_ATTACK_MAPS_1[idx] );
						if( test < apnq ){
							threat |= 1L << ( 63 - test );
						}
					}
				}
			}
			// down left
			if( qb & QB_ATTACK_MAPS_4[idx] ){
				/// Det ligger minst en Q eller B i den vektoren. Sjekk om den ligger alene.
				if( __builtin_popcountll( allPieces & QB_ATTACK_MAPS_4[idx] ) == 1 ){
					threat |= allPieces & QB_ATTACK_MAPS_4[idx];

				}
				else {
					// det ligger fler enn en brikke på QB_ATTACK_MAPS_1, sjekk om den nærmeste brikken er en Q eller B
					int apnq = __builtin_clzll( allPiecesExceptQB & QB_ATTACK_MAPS_4[idx] );
					int test = __builtin_clzll( qb & QB_ATTACK_MAPS_4[idx] );
					if( test < apnq ){
						threat |= 1L << ( 63 - test );
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

			if (__builtin_popcountll( qr & QR_ATTACK_MAPS_1[idx]) == 1 && __builtin_popcountll( allPieces & QR_ATTACK_MAPS_1[idx]) == 1) {
				threat |= qr & QR_ATTACK_MAPS_1[idx];
			} else if (__builtin_popcountll( qr & QR_ATTACK_MAPS_2[idx]) == 1 && __builtin_popcountll( allPieces & QR_ATTACK_MAPS_2[idx]) == 1  ) {
				threat |= qr & QR_ATTACK_MAPS_2[idx];
			} else {

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
					|| (QR_ATTACK_MAPS_3[idx]&allPiecesExceptQR) == 0) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_3[idx]);
				}
			}

			if (king < (qr & QR_ATTACK_MAPS_4[idx])) {

				if (__builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]) <
					__builtin_ctzll((QR_ATTACK_MAPS_4[idx] & allPiecesExceptQR))
					|| (QR_ATTACK_MAPS_4[idx]&allPiecesExceptQR) == 0 ) {
					threat |= 1L << __builtin_ctzll(qr & QR_ATTACK_MAPS_4[idx]);
				}
			}
		}
	}


	board[IDX_KING_THREATS] = threat;

	if( threat ){
		board[IDX_CHECK_STATUS] = MASK_WHITE_KING_CHECKED;

		if( !(threat & lastMoveMap) ){
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
		}

		if( __builtin_popcountll( threat ) > 1 ){
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DISCOVERED;
			board[IDX_CHECK_STATUS] |= MASK_CHECK_TYPE_DOUBLE;
		}
	}
	else {
		board[IDX_CHECK_STATUS] = 0;
	}



	return threat;
}

void clearBlackPiecesWithClearMap(unsigned long board[], unsigned long clear) {
	for (int t = IDX_BLACK_PAWNS; t <= IDX_BLACK_PIECES; t++) {
		board[t] &= clear;
	}
}

void clearWhitePiecesWithClearMap(unsigned long board[], unsigned long clear) {
	for (int t = IDX_WHITE_PAWNS; t <= IDX_WHITE_PIECES; t++) {
		board[t] &= clear;
	}
}


void count(unsigned long b[]) {

	const int level = b[IDX_MOVE_NUM];
	numMoves[level]++;
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CAPTURE)) {
		numCaptures[level]++;
		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE ) {
			numDoubleCaptureChecks[level]++;
		}
		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryCaptureChecks[level]++;
		}
	}
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_EP_STRIKE)) {
		numEP[level]++;
		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE ) {
			numDoubleEPChecks[level]++;
		}
		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryEPChecks[level]++;
		}
	}
	if ((b[IDX_LAST_MOVE_WAS] & (MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE | MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE))) {
		numCastles[level]++;
	}
	if ( b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_PROMO ) {
		numPromos[level]++;

		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE ) {
			numDoublePromoChecks[level]++;
		}
		if( b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED) {
			numDiscoveryPromoChecks[level]++;
		}

	}

	if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_MATED)) {
		numCheckmates[level]++;
	}

	if ((b[IDX_CHECK_STATUS] & (MASK_WHITE_KING_CHECKED | MASK_BLACK_KING_CHECKED))) {
		numChecks[level]++;
	}

	if ((b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DOUBLE)) {
		numDoubleChecks[level]++;
	}

	if ((b[IDX_CHECK_STATUS] & MASK_CHECK_TYPE_DISCOVERED)) {
		numDiscoveryChecks[level]++;
	}





	if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_STALEMATED)) {
		numStalemates[level]++;
	}


}


/************************************************************************************************************
 **																										**
 **													UTILS												 **
 **																										**
 ************************************************************************************************************/

void printNumBoard(unsigned long board[]) {
	for (int s = 0; (s & 64) == 0; s++) {
		if (s % 8 == 0 && s != 0) {
			printf("\n");
		}
		printf(" %ld\t", board[s]);
	}
}

void sprintDiagram( char *target, unsigned long board[] ){

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
	if( (board[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE ){
		castling[moveCursor++] = 'K';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE ){
		castling[moveCursor++] = 'Q';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE ){
		castling[moveCursor++] = 'k';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE ){
		castling[moveCursor++] = 'q';
	}

	sprintf(target, "\"%s\" %s %s", res, board[IDX_TURN] == WHITE_MASK ? "w" : "b", castling);
	//sprintf(target, "\"%s\" %s %lu %lu %lu", res, board[IDX_TURN] == WHITE_MASK ? "w" : "b", board[IDX_CASTLING], board[IDX_MOVE_ID], board[IDX_PARENT_MOVE_ID]);

}

void printDiagram(unsigned long board[]) {

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

	printf("\"%s\"", res);


	printf(" %s", board[IDX_TURN] == WHITE_MASK ? "w" : "b");
	char castling[] = "\0\0\0\0\0";

	int moveCursor = 0;
	if( (board[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE ){
		castling[moveCursor++] = 'K';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE ){
		castling[moveCursor++] = 'Q';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE ){
		castling[moveCursor++] = 'k';
	}
	if( (board[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE ){
		castling[moveCursor++] = 'q';
	}

	printf(" %s ", castling);

	printf(" %lu %lu\n", board[IDX_MOVE_ID], board[IDX_PARENT_MOVE_ID]);

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

void printBitBoard(unsigned long board[]) {
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
	printf("Move ID: %ld\n", board[IDX_MOVE_ID]);
	printf("Parent ID: %ld\n", board[IDX_PARENT_MOVE_ID]);


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
	printf("\n");

	fflush(stdout);

}

/*
0..63 = board
65 = Turn
67..71 castling
73 calculate to depth
rn..kbnrpppqpppp...........p........P.bP........PPPP.PP.RNB.KBNR w KQkq 4 1
*/

void diagramToBitBoard(unsigned long board[], char diagram[]) {

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

	board[IDX_CASTLING] =
			MASK_CASTLING_WHITE_QUEEN_SIDE |
			MASK_CASTLING_WHITE_KING_SIDE |
			MASK_CASTLING_BLACK_QUEEN_SIDE |
			MASK_CASTLING_BLACK_KING_SIDE;

	board[IDX_TURN] = WHITE_MASK;

	int pos = 0;

	for (int t = 0; t < len; t++) {
		switch (diagram[t]) {
			// BLACK PIECES
			case '.':
				pos++;
				break;
			case 'p':
				board[IDX_BLACK_PAWNS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'r':
				board[IDX_BLACK_ROOKS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'b':
				board[IDX_BLACK_BISHOPS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'n':
				board[IDX_BLACK_KNIGHTS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'q':
				board[IDX_BLACK_QUEENS] |= (1l << (63 - pos));
				pos++;
				break;
			case 'k':
				board[IDX_BLACK_KING] = (1l << (63 - pos));
				pos++;
				break;
				// WHITE PIECES
			case 'P':
				board[IDX_WHITE_PAWNS] |= (1l << (63 - pos));
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


} // diagramToBitBoard


void printCompactBoard(unsigned long board[]) {

	printf("compact");

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "maps.h"

typedef enum { FALSE, TRUE } boolean;

#define TEST_IF_ON_R1_R8

void diagramToByteBoard( unsigned long board[], char diagram[] );
void printDiagram( unsigned long board[] );
void printNumBoard( unsigned long board[] );
unsigned long printStats();

// TODO: skriv om alt til unsigned unsigned long!!


void dig(unsigned long board[]);
void count(unsigned long board[]);

// bitboard functions, highly speed sensitive
void makeNewBoard( unsigned long originalBoard[], unsigned long newBoard[] );

int calculateWhiteKingCheckStatus( unsigned long board[] );
int calculateBlackKingCheckStatus( unsigned long board[] );

int findAllPossibleMoves2( unsigned long originalBoard[]);

int moveWhitePawns( unsigned long b[] );
int moveWhiteRooksOrQueens( unsigned long b[], int pieceMapIndex );
int moveWhiteKnights( unsigned long b[] );
int moveWhiteBishopsOrQueens( unsigned long b[], int pieceMapIndex );
int moveWhiteKing( unsigned long[b] );

int moveBlackPawns( unsigned long b[] );
int moveBlackRooksOrQueens( unsigned long b[], int pieceMapIndex );
int moveBlackKnights( unsigned long b[] );
int moveBlackBishopsOrQueens( unsigned long b[], int pieceMapIndex );
int moveBlackKing( unsigned long[b] );


void makeWhiteMove( unsigned long move[], int pieceIndex, unsigned long moveToMap, unsigned long clearMap, unsigned long blackPieces );
void makeBlackMove( unsigned long move[], int pieceIndex, unsigned long moveToMap, unsigned long clearMap, unsigned long blackPieces );

void clearPieceAtIndex( unsigned long board[], int idx );
void clearWhitePieceAtIndex( unsigned long board[], int idx ); // todo: replace with map-version
void clearBlackPieceAtIndex( unsigned long board[], int idx ); // todo:  replace with map-version
void clearBlackPiecesWithClearMap( unsigned long board[], unsigned long clear );
void clearWhitePiecesWithClearMap( unsigned long board[], unsigned long clear );

int makeWhiteBitPromos( unsigned long board[], unsigned long map );
int makeBlackBitPromos( unsigned long board[], unsigned long map );

// util for board printing and conversion. not speed sensitive
void setBitsToChar( char *str, unsigned long bits, char c);
void printBitBoard( unsigned long board[] );
void diagramToBitBoard( unsigned long board[], char diagram[] );
void printunsigned longAsBitBoard( unsigned long bitstream );

int MAX_LEVEL = 2;
unsigned long numMoves[]	  = {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numCaptures[]   = {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numEP[]		 = {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numCastles[]	= {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numPromos[]	 = {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numChecks[]	 = {0,0,0,0,0,0,0,0,0,0,0};
unsigned long numCheckmates[] = {0,0,0,0,0,0,0,0,0,0,0};


unsigned long makeNewBoardInvocations = 0;
unsigned long isSquaresThreatenedByColorInvocations = 0;
unsigned long influenceMapForSquareInvocations = 0;
unsigned long moveLinearInvocations = 0;

char *workUnitId = NULL;


const unsigned long PIECE_COMBOS =
  (unsigned long)Pieces_QBK		|  (unsigned long)Pieces_QB <<  6 |
  (unsigned long)Pieces_QBPK << 12 |  (unsigned long)Pieces_QB << 18 |
  (unsigned long)Pieces_QRK <<  24 |  (unsigned long)Pieces_QR << 30 |
  (unsigned long)Pieces_QRK <<  36 |  (unsigned long)Pieces_QR << 42
  ;

int main( int argc, char **argv){


/*
  for( int idx=0;idx<64;idx++){
	printf("------- %d -------\n",idx);
	printunsigned longAsBitBoard( KING_ATTACK_MAPS[idx] );
	printunsigned longAsBitBoard( KNIGHT_ATTACK_MAPS[idx] );
	printunsigned longAsBitBoard( BLACK_PAWN_REVERSE_ATTACK_MAPS[idx] );
	printunsigned longAsBitBoard( WHITE_PAWN_ATTACK_MAPS[idx] );
	printunsigned longAsBitBoard( QB_ATTACK_MAPS[idx] );
	printunsigned longAsBitBoard( QR_ATTACK_MAPS[idx] );
  }
*/

  // "rnbqkbnr pppppppp ........ ........ ........ ........ PPPPPPPP RNBQKBNR"

  /*
  char *initialBoard = "\
					   r n b q k b n r\
					   p p p p p p p p\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . . . . . .\
					   P P P P P P P P\
					   R N B Q K B N R";*/

   /*char *initialBoard = "\
						. . . . . . . .\
						. . . . . . . .\
						. . . . . . . .\
						. . . . K . . .\
						. . . . . . . .\
						. . . r b n . .\
						. . . . P . . .\
						. . . . . . . .";*/




	char *initialBoard = "\
					   r . . . k . . r\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . . . . . .\
					   . . . P . . . .\
					   . . . . . K . .";


   /*char *initialBoard = "\
					  r . . . k . . r\
					  p . p p q p b .\
					  b n . . p n p .\
					  . . . P N . . .\
					  . p . . P . . .\
					  . . N . . Q . p\
					  P P P B B P P P\
					  R . . . K . . R";*/



	unsigned long board[NUM_BYTES];

	if( argc > 1 ){
	  if( strcmp(argv[1],"SLEEP") ==0 ){
		printf("SLEEPING");
		fflush(stdout);
		sleep(60);
		return 0;
	  }
	  initialBoard = argv[1];
	}

	diagramToBitBoard( board, initialBoard);
/*
	printBitBoard( board );
	unsigned unsigned long allOtherPieces = board[IDX_ALL_PIECES] & ~board[IDX_WHITE_PAWNS];
	unsigned unsigned long whitePawns = board[IDX_WHITE_PAWNS];

	printunsigned longAsBitBoard( board[IDX_ALL_PIECES] );

	printf("White pawns bitmap:\n");
	printunsigned longAsBitBoard( whitePawns );

	printf("All other pieces bitmap:\n");
	printunsigned longAsBitBoard( allOtherPieces );

	printf("All other pieces moved 8 bits to the right:\n");
	printunsigned longAsBitBoard( allOtherPieces >> 8 );

	printf("All other pieces moved 16 bits to the right:\n");
	printunsigned longAsBitBoard( allOtherPieces >> 16 );

	unsigned unsigned long pawnsThatCanMoveOneForward =  whitePawns & ~(allOtherPieces >> 8);
	printf("White pawns that to not crash with the other pieces that were moved 8 bits to the right:\n");
	printunsigned longAsBitBoard( pawnsThatCanMoveOneForward );

	printf("White pawns that to not crash with the other pieces that were moved 16 bits to the right:\n");
	unsigned unsigned long pawnsThatCanMoveTwoForward =  (pawnsThatCanMoveOneForward & R2) & ~(allOtherPieces >> 16);
	printunsigned longAsBitBoard( pawnsThatCanMoveTwoForward );

	return 0;*/
	board[IDX_CASTLING] = MASK_CASTLING_BLACK_KING_SIDE|MASK_CASTLING_BLACK_QUEEN_SIDE;

  //  printunsigned longAsBitBoard( board[IDX_ALL_PIECES]  );

	if( argc > 2 ){
	  if( strcmp(argv[2], "b" ) == 0 ){
		  board[IDX_TURN] = BLACK_MASK;
	  }
	  else {
		  board[IDX_TURN] = WHITE_MASK;
	  }
	}

	if( argc > 3 ){
		board[IDX_CASTLING] = atoi(argv[3]);
	}

	if( argc > 4 ){
		MAX_LEVEL = atoi(argv[4]);
	}

	if( argc > 5 ){
		workUnitId = argv[5];
		printf("ID:\"%s\" %s %s %s %s\n", argv[1],argv[2],argv[3],argv[4],argv[5] );
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

	unsigned long total = printStats();

	printf("\nTotal valid moves found : %lu \n" ,total);

	printf("Time spent: %ld.%09ld \n", (unsigned long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);

	printf( "%lu makeNewBoardInvocations\n", makeNewBoardInvocations );
	printf( "%lu isSquaresThreatenedByColorInvocations\n", isSquaresThreatenedByColorInvocations );
	printf( "%lu influenceMapForSquareInvocations\n", influenceMapForSquareInvocations );
	printf( "%lu moveLinearInvocations\n", moveLinearInvocations );

	/*printunsigned longAsBitBoard( MASK_CASTLING_BLACK_KING_SIDE );
	printunsigned longAsBitBoard( MASK_CASTLING_BLACK_QUEEN_SIDE );
	printunsigned longAsBitBoard( MASK_CASTLING_WHITE_KING_SIDE );
	printunsigned longAsBitBoard( MASK_CASTLING_WHITE_QUEEN_SIDE );*/

	return 0;
} // end main

unsigned long printStats(){

  printf( "%3s\t%20s\t%20s\t%10s\t%10s\t%10s\t%20s\t%20s\n",
		  "Depth",
		  "Nodes",
		  "Caps",
		  "E.p.",
		  "Castles",
		  "Promos",
		  "Checks",
		  "Mates"
  );

  static unsigned long total = 0;

  for (int t = 0; t <= MAX_LEVEL+1; t++) {
	  printf("%3d",t);
	  printf("\t%20lu",numMoves[t]);
	  printf("\t%20lu",numCaptures[t]);
	  printf("\t%10lu",numEP[t]);
	  printf("\t%10lu",numCastles[t]);
	  printf("\t%10lu",numPromos[t]);
	  printf("\t%20lu",numChecks[t]);
	  printf("\t%20lu\n",numCheckmates[t]);
	  total += numMoves[t];
  }
  printf("\n");
  fflush(stdout);
  return total;

}


/************************************************************************************************************
 **																										**
 **												 ENGINE												 **
 **																										**
 ************************************************************************************************************/


void dig(unsigned long board[]){

	if (board[IDX_MOVE_NUM] < MAX_LEVEL
//	   || (board[IDX_MOVE_NUM] == MAX_LEVEL && board[IDX_CHECK_STATUS] != 0) // Include to test for mates on the last level
	) {
		int numMoves = findAllPossibleMoves2(board);
		if (numMoves == 0 && board[IDX_CHECK_STATUS] != 0 ) {
			board[IDX_CHECK_STATUS] |= MASK_KING_IS_MATED;
		}
	}
	count(board);
	if( MAX_LEVEL <= 4 ){

		printBitBoard( board );

	}
	/*if( board[IDX_MOVE_NUM] == 2){
	  printStats();
	}*/
}

int findAllPossibleMoves2( unsigned long originalBoard[]) {

  int numMovesFound = 0;

  if (originalBoard[IDX_TURN] == WHITE_MASK) { // turn == white

	  numMovesFound += moveWhitePawns( originalBoard );
	  numMovesFound += moveWhiteRooksOrQueens( originalBoard, IDX_WHITE_QUEENS );
	  numMovesFound += moveWhiteRooksOrQueens( originalBoard, IDX_WHITE_ROOKS );
	  numMovesFound += moveWhiteBishopsOrQueens( originalBoard, IDX_WHITE_QUEENS );
	  numMovesFound += moveWhiteBishopsOrQueens( originalBoard, IDX_WHITE_BISHOPS );
	  numMovesFound += moveWhiteKnights( originalBoard );
	  //numMovesFound += moveWhiteKing( originalBoard );

  }
  else {

	  ///numMovesFound += moveBlackPawns( originalBoard );
	  ///numMovesFound += moveBlackRooksOrQueens( originalBoard, IDX_BLACK_QUEENS );
	  numMovesFound += moveBlackRooksOrQueens( originalBoard, IDX_BLACK_ROOKS );
	  //numMovesFound += moveBlackBishopsOrQueens( originalBoard, IDX_BLACK_QUEENS );
	  ///numMovesFound += moveBlackBishopsOrQueens( originalBoard, IDX_BLACK_BISHOPS );
 	  //numMovesFound += moveBlackKnights( originalBoard );
	  //numMovesFound += moveBlackKing( originalBoard );

  }

  return numMovesFound;

}


// ----------------------------------- WHITE KING ---------------------------------

int moveWhiteKing( unsigned long b[] ){

  int numMovesFound = 0;

  unsigned long pieceMap = b[IDX_WHITE_KING];

  int idx = __builtin_ctzll( pieceMap );

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  unsigned long clearMap = ~pieceMap;
  unsigned long moveToMaps = KING_ATTACK_MAPS[idx];

  int moveToIdx = 0;

  while( moveToMaps ){

	int moveToShift = __builtin_ctzll( moveToMaps );
	moveToIdx += moveToShift;

	unsigned long moveToMap = 1l<<moveToIdx;
	if( !(moveToMap & whitePieces) ){


	  makeNewBoard( b, move );
	  makeWhiteMove( move, IDX_WHITE_KING, moveToMap, clearMap, blackPieces );
	  move[IDX_WHITE_KING] = moveToMap;

	  if ( calculateWhiteKingCheckStatus(move) == 0) {

		#ifdef TEST_IF_ON_R1_R8
		if( moveToMap & R1_R8 ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves


		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numMovesFound++;
		dig(move);
	  }
	}
	moveToMaps >>= (moveToShift+1);
	moveToIdx++;

  }


  if( (b[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE ){
	  // calculate if squares on queen side are threatened and free
	  if( !(B1_C1_D1_MASK & allPieces) ){

		b[IDX_WHITE_KING] = B1_MASK;
		int threat = calculateWhiteKingCheckStatus( b );
		if( !threat ){
		  b[IDX_WHITE_KING] = C1_MASK;
		  threat = calculateWhiteKingCheckStatus( b );
		  if( !threat ){
			b[IDX_WHITE_KING] = D1_MASK;
			threat = calculateWhiteKingCheckStatus( b );
			if( !threat ){
			  b[IDX_WHITE_KING] = pieceMap;
			  if( !calculateWhiteKingCheckStatus( b ) ){
				// do Castling
				makeNewBoard( b, move );
				makeWhiteMove( move, IDX_WHITE_KING, C1_MASK, clearMap, blackPieces );
				makeWhiteMove( move, IDX_WHITE_ROOKS, D1_MASK, ~A1_MASK, blackPieces );
				move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
				move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE|MASK_CASTLING_WHITE_KING_SIDE);
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
				numMovesFound++;
				dig(move);
			  }
			}
		  }
		}
		b[IDX_WHITE_KING] = pieceMap;
	  }
	}

	if( (b[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) == MASK_CASTLING_WHITE_KING_SIDE ){
	  if( !(F1_G1_MASK & allPieces) ){

		  b[IDX_WHITE_KING] = F1_MASK;
		  int threat = calculateWhiteKingCheckStatus( b );
		  if( !threat ){
			b[IDX_WHITE_KING] = G1_MASK;
			threat = calculateWhiteKingCheckStatus( b );
			if( !threat ){
				b[IDX_WHITE_KING] = pieceMap;
				if( !calculateWhiteKingCheckStatus( b ) ){
					// do Castling
					makeNewBoard( b, move );
					makeWhiteMove( move, IDX_WHITE_KING, G1_MASK, clearMap, blackPieces );
					makeWhiteMove( move, IDX_WHITE_ROOKS, F1_MASK, ~H1_MASK, blackPieces );
					move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
					move[IDX_CASTLING] &= ~(MASK_CASTLING_WHITE_QUEEN_SIDE|MASK_CASTLING_WHITE_KING_SIDE);
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

int moveBlackKing( unsigned long b[] ){

  int numMovesFound = 0;

  unsigned long pieceMap = b[IDX_BLACK_KING];

  int idx = __builtin_ctzll( pieceMap );

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  unsigned long clearMap = ~pieceMap;
  unsigned long moveToMaps = KING_ATTACK_MAPS[idx];

  int moveToIdx = 0;

  while( moveToMaps ){

	int moveToShift = __builtin_ctzll( moveToMaps );
	moveToIdx += moveToShift;

	unsigned long moveToMap = 1l<<moveToIdx;
	if( !(moveToMap & whitePieces) ){


	  makeNewBoard( b, move );
	  makeBlackMove( move, IDX_BLACK_KING, moveToMap, clearMap, whitePieces );
	  move[IDX_BLACK_KING] = moveToMap;

	  if ( calculateBlackKingCheckStatus(move) == 0) {

		#ifdef TEST_IF_ON_R1_R8
		if( moveToMap & R1_R8 ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves


		move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		numMovesFound++;
		dig(move);
	  }
	}
	moveToMaps >>= (moveToShift+1);
	moveToIdx++;

  }


  if( (b[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE ){
	  // calculate if squares on queen side are threatened and free
	  if( !(B1_C1_D1_MASK & allPieces) ){

		b[IDX_BLACK_KING] = B8_MASK;
		int threat = calculateBlackKingCheckStatus( b );
		if( !threat ){
		  b[IDX_BLACK_KING] = C8_MASK;
		  threat = calculateBlackKingCheckStatus( b );
		  if( !threat ){
			b[IDX_BLACK_KING] = D8_MASK;
			threat = calculateBlackKingCheckStatus( b );
			if( !threat ){
			  b[IDX_BLACK_KING] = pieceMap;
			  if( !calculateBlackKingCheckStatus( b ) ){
				// do Castling
				makeNewBoard( b, move );
				makeWhiteMove( move, IDX_BLACK_KING, C8_MASK, clearMap, blackPieces );
				makeWhiteMove( move, IDX_BLACK_ROOKS, D8_MASK, ~A8_MASK, blackPieces );
				move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
				move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE|MASK_CASTLING_BLACK_KING_SIDE);
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
				numMovesFound++;
				dig(move);
			  }
			}
		  }
		}
		b[IDX_BLACK_KING] = pieceMap;
	  }
	}

	if( (b[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) == MASK_CASTLING_BLACK_KING_SIDE ){
	  if( !(F8_G8_MASK & allPieces) ){

		  b[IDX_BLACK_KING] = F8_MASK;
		  int threat = calculateBlackKingCheckStatus( b );
		  if( !threat ){
			b[IDX_BLACK_KING] = G8_MASK;
			threat = calculateBlackKingCheckStatus( b );
			if( !threat ){
				b[IDX_BLACK_KING] = pieceMap;
				if( !calculateBlackKingCheckStatus( b ) ){
					// do Castling
					makeNewBoard( b, move );
					makeBlackMove( move, IDX_BLACK_KING, G8_MASK, clearMap, whitePieces );
					makeBlackMove( move, IDX_BLACK_ROOKS, F8_MASK, ~H8_MASK, whitePieces );
					move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
					move[IDX_CASTLING] &= ~(MASK_CASTLING_BLACK_QUEEN_SIDE|MASK_CASTLING_BLACK_KING_SIDE);
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


//////////////////////////////////// MOVE WHITE PAWN //////////////
int moveWhitePawns( unsigned long b[] ){

  int numPawnMoves = 0;

  unsigned long pawns = b[IDX_WHITE_PAWNS];
  unsigned long originalPawns = pawns;

  // 1. Make moves for pawns that can move ONE forward
  // 2. Make moves for pawns that can move TWO forward and set EP index/map
  // 3. Make capturing moves for pawns with WHITE_PAWN_ATTACK_MAPS
  // 4. Make en passant captures for pawns with WHITE_PAWN_ATTACK_MAPS that matches EP Index/map

  unsigned long move[NUM_BYTES];

  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long pawnsThatCanMoveOneForward = pawns & ~(allPieces>>8);
  unsigned long pawnsThatCanMoveTwoForward = pawnsThatCanMoveOneForward  & R2;
  pawnsThatCanMoveTwoForward &= ~(allPieces>>16);

  int pieceIdx = 0;

  while( pawnsThatCanMoveOneForward ){
	int shift = __builtin_ctzll(pawnsThatCanMoveOneForward);
	pieceIdx += shift;
	unsigned long pieceMap = 1l << pieceIdx;
	unsigned long newPieceMap = pieceMap<<8;
	unsigned long clearOldPieceMap = ~pieceMap;
	makeNewBoard( b, move );
	move[IDX_WHITE_PAWNS] &= clearOldPieceMap;
	move[IDX_WHITE_PIECES] &= clearOldPieceMap;
	move[IDX_WHITE_PAWNS] |= newPieceMap;
	move[IDX_WHITE_PIECES] |= newPieceMap;
	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];

	if ( calculateWhiteKingCheckStatus(move) == 0) {
	  if( newPieceMap & R8 ){
		move[IDX_CASTLING] &= ~newPieceMap; // TODO: Ta med denne på alle R8/R1 moves
		numPawnMoves += makeWhiteBitPromos( move, newPieceMap );
	  }
	  else {
		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numPawnMoves++;
		dig(move);
	  }
	}

	pawnsThatCanMoveOneForward >>= (shift+1);
	pieceIdx++;

  }

  pieceIdx = 0;
  while( pawnsThatCanMoveTwoForward ){
	int shift = __builtin_ctzll(pawnsThatCanMoveTwoForward);
	pieceIdx += shift;
	unsigned long pieceMap = 1l << pieceIdx;
	unsigned long newPieceMap = pieceMap<<16;
	unsigned long clearOldPieceMap = ~pieceMap;

	makeNewBoard( b, move );
	move[IDX_WHITE_PAWNS] &= clearOldPieceMap;
	move[IDX_WHITE_PIECES] &= clearOldPieceMap;
	move[IDX_WHITE_PAWNS] |= newPieceMap;
	move[IDX_WHITE_PIECES] |= newPieceMap;
	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
	move[IDX_EP_IDX] = pieceMap<<8;

	if ( calculateWhiteKingCheckStatus(move) == 0) {
		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numPawnMoves++;
		dig(move);
	}

	pawnsThatCanMoveTwoForward >>= (shift+1);
	pieceIdx++;

  }

  // todo: if EP-index != 0, check if a pawn is on row 5 and their
  // attack maps match the EP-index.

  pieceIdx = 0;
  while( pawns ){
	int shift =  __builtin_ctzll( pawns );
	pieceIdx += shift;

	unsigned long attackMap = WHITE_PAWN_ATTACK_MAPS[pieceIdx];

	if( b[IDX_BLACK_PIECES] & attackMap ){

	  unsigned long pieceMap = 1l << pieceIdx;
	  unsigned long validAttacks = b[IDX_BLACK_PIECES] & attackMap;
	  int attackIndex = 0;

	  while( validAttacks ){
		int attackShift =  __builtin_ctzll( validAttacks );
		attackIndex += attackShift;

		unsigned long newPieceMap = 1L << attackShift;
		unsigned long clearOldPieceMap = ~pieceMap;

		makeNewBoard( b, move );
		clearBlackPieceAtIndex( move, attackIndex );
		move[IDX_WHITE_PAWNS] &= clearOldPieceMap;
		move[IDX_WHITE_PIECES] &= clearOldPieceMap;
		move[IDX_WHITE_PAWNS] |= newPieceMap;
		move[IDX_WHITE_PIECES] |= newPieceMap;
		move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
		move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

		if ( calculateWhiteKingCheckStatus(move) == 0) {

		  if( newPieceMap & R8 ){
			move[IDX_CASTLING] &= ~newPieceMap;
			numPawnMoves += makeWhiteBitPromos( move, newPieceMap );
		  }
		  else {
			move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
			numPawnMoves++;
			dig(move);
		  }

		}

		attackShift++;
		validAttacks >>= attackShift;
	  }
	}
	shift++;
	pawns >>= shift;
	pieceIdx++;
  }

  if( b[IDX_EP_IDX] != 0 ){
	  // TODO: test when black pieces are ready
	  // ep index is between A6 and H6, ie: R6
	  // check if any pawn on R5 has that mask in their attack-map.
	  unsigned long pawnsOnR5 = originalPawns & R5;
	  if( pawnsOnR5 ){
		  unsigned long epMap = b[IDX_EP_IDX];
		  // gi through attack maps for each pawn. check overlap of EP_index
		  int attackIndex = 0;
		  while( pawnsOnR5 ){
			  int attackShift =  __builtin_ctzll( pawnsOnR5 );
			  attackIndex += attackShift;
			  unsigned long attackingPawn = 1l << attackIndex;

			  if( attackingPawn == 1 ){
				  // TODO: remove, when not seg fault
				  printunsigned longAsBitBoard( attackingPawn | WHITE_PAWN_ATTACK_MAPS[attackIndex]  );
			  }

			  if( (WHITE_PAWN_ATTACK_MAPS[attackIndex] & epMap) == epMap ){
				  makeNewBoard( b, move );
				  clearBlackPiecesWithClearMap( move, epMap << 8 );
				  move[IDX_WHITE_PAWNS] &= ~attackingPawn;
				  move[IDX_WHITE_PIECES] &= ~attackingPawn;
				  move[IDX_WHITE_PAWNS] |= epMap;
				  move[IDX_WHITE_PIECES] |= epMap;
				  move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
				  move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

				  if ( calculateWhiteKingCheckStatus(move) == 0) {
					  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
					  numPawnMoves++;
					  dig(move);
				  }
			  }
			  attackIndex++;
			  pawnsOnR5 >>= attackIndex;
		  }
	  }
  }

  return numPawnMoves;

}

//////////////////////////////////// MOVE BLACK PAWNs //////////////

int moveBlackPawns( unsigned long b[] ){

  int numPawnMoves = 0;

  unsigned long pawns = b[IDX_BLACK_PAWNS];
  unsigned long originalPawns = pawns;

  // 1. Make moves for pawns that can move ONE forward
  // 2. Make moves for pawns that can move TWO forward and set EP index/map
  // 3. Make capturing moves for pawns with BLACK_PAWN_ATTACK_MAPS
  // 4. Make en passant captures for pawns with BLACK_PAWN_ATTACK_MAPS that matches EP Index/map

  unsigned long move[NUM_BYTES];

  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long pawnsThatCanMoveOneForward = pawns & ~(allPieces<<8);
  unsigned long pawnsThatCanMoveTwoForward = pawnsThatCanMoveOneForward  & R7;
  pawnsThatCanMoveTwoForward &= ~(allPieces<<16);

  int pieceIdx = 0;

  while( pawnsThatCanMoveOneForward ){
	int shift = __builtin_ctzll(pawnsThatCanMoveOneForward);
	pieceIdx += shift;
	unsigned long pieceMap = 1l << pieceIdx;
	unsigned long newPieceMap = pieceMap>>8;
	unsigned long clearOldPieceMap = ~pieceMap;
	makeNewBoard( b, move );
	move[IDX_BLACK_PAWNS] &= clearOldPieceMap;
	move[IDX_BLACK_PIECES] &= clearOldPieceMap;
	move[IDX_BLACK_PAWNS] |= newPieceMap;
	move[IDX_BLACK_PIECES] |= newPieceMap;
	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];

	if ( calculateBlackKingCheckStatus(move) == 0) {
		if( newPieceMap & R1 ){
			move[IDX_CASTLING] &= ~newPieceMap; // TODO: Ta med denne på alle R8/R1 moves
			numPawnMoves += makeBlackBitPromos( move, newPieceMap );
		}
	 	else {
			move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
			numPawnMoves++;
			dig(move);
		}
	}

	pawnsThatCanMoveOneForward >>= (shift+1);
	pieceIdx++;

	}

  pieceIdx = 0;
  while( pawnsThatCanMoveTwoForward ){
	int shift = __builtin_ctzll(pawnsThatCanMoveTwoForward);
	pieceIdx += shift;
	unsigned long pieceMap = 1l << pieceIdx;
	unsigned long newPieceMap = pieceMap>>16;
	unsigned long clearOldPieceMap = ~pieceMap;

	makeNewBoard( b, move );
	move[IDX_BLACK_PAWNS] &= clearOldPieceMap;
	move[IDX_BLACK_PIECES] &= clearOldPieceMap;
	move[IDX_BLACK_PAWNS] |= newPieceMap;
	move[IDX_BLACK_PIECES] |= newPieceMap;
	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
	move[IDX_EP_IDX] = pieceMap>>8;

	if ( calculateBlackKingCheckStatus(move) == 0) {
		move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		numPawnMoves++;
		dig(move);
	}

	pawnsThatCanMoveTwoForward >>= (shift+1);
	pieceIdx++;

  }

  // todo: if EP-index != 0, check if a pawn is on row 5 and their
  // attack maps match the EP-index.

  	pieceIdx = 0;
		while( pawns ){
			int shift =  __builtin_ctzll( pawns );
			pieceIdx += shift;

			unsigned long attackMap = BLACK_PAWN_ATTACK_MAPS[pieceIdx];

			if( b[IDX_WHITE_PIECES] & attackMap ){

				unsigned long pieceMap = 1l << pieceIdx;
				unsigned long validAttacks = b[IDX_WHITE_PIECES] & attackMap;
				int attackIndex = 0;

				while( validAttacks ){
				int attackShift =  __builtin_ctzll( validAttacks );
				attackIndex += attackShift;

				unsigned long newPieceMap = 1L << attackShift;
				unsigned long clearOldPieceMap = ~pieceMap;

				makeNewBoard( b, move );
				clearWhitePieceAtIndex( move, attackIndex );
				move[IDX_BLACK_PAWNS] &= clearOldPieceMap;
				move[IDX_BLACK_PIECES] &= clearOldPieceMap;
				move[IDX_BLACK_PAWNS] |= newPieceMap;
				move[IDX_BLACK_PIECES] |= newPieceMap;
				move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
				move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

				if ( calculateBlackKingCheckStatus(move) == 0) {

					if( newPieceMap & R1 ){
						move[IDX_CASTLING] &= ~newPieceMap;
						numPawnMoves += makeBlackBitPromos( move, newPieceMap );
					}
					else {
						move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
						numPawnMoves++;
						dig(move);
					}

				}

				attackShift++;
				validAttacks >>= attackShift;
			}
		}
		shift++;
		pawns >>= shift;
		pieceIdx++;
	}

	if( b[IDX_EP_IDX] != 0 ){
		// TODO: test when black pieces are ready
		// ep index is between A6 and H6, ie: R6
		// check if any pawn on R5 has that mask in their attack-map.
		unsigned long pawnsOnR4 = originalPawns & R4;
		if( pawnsOnR4 ){
			unsigned long epMap = b[IDX_EP_IDX];
			// gi through attack maps for each pawn. check overlap of EP_index
			int attackIndex = 0;
			while( pawnsOnR4 ){
				int attackShift =  __builtin_ctzll( pawnsOnR4 );
				attackIndex += attackShift;
				unsigned long attackingPawn = 1l << attackIndex;

				if( attackingPawn == 1 ){
					// TODO: remove, when not seg fault
					printunsigned longAsBitBoard( attackingPawn | BLACK_PAWN_ATTACK_MAPS[attackIndex]  );
				}

				if( (BLACK_PAWN_ATTACK_MAPS[attackIndex] & epMap) == epMap ){
					makeNewBoard( b, move );
					clearWhitePiecesWithClearMap( move, epMap >> 8 );
					move[IDX_BLACK_PAWNS] &= ~attackingPawn;
					move[IDX_BLACK_PIECES] &= ~attackingPawn;
					move[IDX_BLACK_PAWNS] |= epMap;
					move[IDX_BLACK_PIECES] |= epMap;
					move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
					move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

					if ( calculateBlackKingCheckStatus(move) == 0) {
						move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
						numPawnMoves++;
						dig(move);
					}
				}
				attackIndex++;
				pawnsOnR4 >>= attackIndex;
			}
		}
	}

	return numPawnMoves;
}

//////////////////////////////////// MOVE WHITE BISHOPS /////////////////////////////////////
int moveWhiteBishopsOrQueens( unsigned long b[], int pieceMapIndex ){

  int numMovesFound = 0;

  unsigned long pieces = b[pieceMapIndex];
  unsigned long originalPieces = pieces;
  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;

	unsigned long moveToMap = pieceMap;
	unsigned long clearMap = ~pieceMap;

	//--

	int rank = idx >> 3;
	int file = idx & 7;

	// TODO: regn up hvor langt testen skal gå. ikke noe vits i å teste for hver file&dRank
	// pluss at vi kan bruke bitmap i stedet for
	int maxM = 7-rank;
	maxM = 7-file < maxM ? 7-file : maxM;
	unsigned long testMap = 0;
	for( int m=0;m<maxM;m++){
		moveToMap <<= 9;
		if( moveToMap & whitePieces ){
		  break;
		}
		makeNewBoard( b, move );
		makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

		if ( calculateWhiteKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & blackPieces ){
		  break;
		}

		testMap |= moveToMap;

	}

	maxM = 7-rank;
	maxM = file < maxM ? file : maxM;

	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap <<= 7;
		if( moveToMap & whitePieces ){
		  break;
		}
		makeNewBoard( b, move );
		makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

		if ( calculateWhiteKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & blackPieces ){
		  break;
		}

		testMap |= moveToMap;
	}


	maxM = rank;
	maxM = maxM > (7-file) ? (7-file) : maxM;

	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap >>= 7;
		if( moveToMap & whitePieces ){
		  break;
		}
		makeNewBoard( b, move );
		makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

		if ( calculateWhiteKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & blackPieces ){
		  break;
		}

		testMap |= moveToMap;
	}

	maxM = rank;
	maxM = maxM > file ? file : maxM;
	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap  >>= 9;
		if( moveToMap & whitePieces ){
		  break;
		}
		makeNewBoard( b, move );
		makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

		if ( calculateWhiteKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & blackPieces ){
		  break;
		}

		testMap |= moveToMap;

	}

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;
}

//////////////////////////////////// MOVE BLACK BISHOPS /////////////////////////////////////
int moveBlackBishopsOrQueens( unsigned long b[], int pieceMapIndex ){

  int numMovesFound = 0;

  unsigned long pieces = b[pieceMapIndex];
  unsigned long originalPieces = pieces;
  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;

	unsigned long moveToMap = pieceMap;
	unsigned long clearMap = ~pieceMap;

	//--

	int rank = idx >> 3;
	int file = idx & 7;

	// TODO: regn up hvor langt testen skal gå. ikke noe vits i å teste for hver file&dRank
	// pluss at vi kan bruke bitmap i stedet for
	int maxM = 7-rank;
	maxM = 7-file < maxM ? 7-file : maxM;

	for( int m=0;m<maxM;m++){
		moveToMap <<= 9;

		if( moveToMap & blackPieces ){
		  break;
		}

		makeNewBoard( b, move );
		makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

		if ( calculateBlackKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & whitePieces ){
		  break;
		}

	}

	maxM = 7-rank;
	maxM = file < maxM ? file : maxM;

	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap <<= 7;
		if( moveToMap & blackPieces ){
		  break;
		}

		makeNewBoard( b, move );
		makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

		if ( calculateBlackKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & whitePieces ){
		  break;
		}

	}

	maxM = rank;
	maxM = maxM > (7-file) ? (7-file) : maxM;

	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap >>= 7;

		if( moveToMap & blackPieces ){
		  break;
		}

		makeNewBoard( b, move );
		makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

		if ( calculateBlackKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & whitePieces ){
		  break;
		}

	}

	maxM = rank;
	maxM = maxM > file ? file : maxM;
	moveToMap = pieceMap;

	for( int m=0;m<maxM;m++){
		moveToMap  >>= 9;
		if( moveToMap & blackPieces ){
		  break;
		}

		makeNewBoard( b, move );
		makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

		if ( calculateBlackKingCheckStatus(move) == 0) {
		  #ifdef TEST_IF_ON_R1_R8
		  if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		  #endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}

		if( moveToMap & whitePieces ){
		  break;
		}


	}

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;
}

///////////////////////////////////// MOVE WHITE ROOKS //////////////////////////////////////
int moveWhiteRooksOrQueens( unsigned long b[], int pieceMapIndex  ){

  int numMovesFound = 0;

  unsigned long pieces = b[pieceMapIndex];
  unsigned long originalPieces = pieces;

  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;
	// for hver piece. flytt h&v til vi treffe noe
	int file = idx & 7;
	int rank = idx >> 3;

	unsigned long moveToMap = pieceMap;
	unsigned long clearMap = ~pieceMap;

	// up
	for( int m=0;m<7-rank;m++){
	  moveToMap <<= 8;

	  if( moveToMap & whitePieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

	  if ( calculateWhiteKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves


		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & blackPieces ){
		break;
	  }

	}

	// left
	moveToMap = pieceMap;
	for( int m=0;m<7-file;m++){
	  moveToMap <<= 1;

	  if( moveToMap & whitePieces ){
		break;
	  }
	  makeNewBoard( b, move );
	  makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

	  if ( calculateWhiteKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & blackPieces ){
		break;
	  }

	}

	// right
	moveToMap = pieceMap;
	for( int m=0;m<file;m++){
	  moveToMap >>= 1;

	  if( moveToMap & whitePieces ){
		break;
	  }
	  makeNewBoard( b, move );
	  makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

	  if ( calculateWhiteKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & blackPieces ){
		break;
	  }

	}

	// down
	moveToMap = pieceMap;
	for( int m=0;m<rank;m++){
	  moveToMap >>= 8;

	  if( moveToMap & whitePieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeWhiteMove( move, pieceMapIndex, moveToMap, clearMap, blackPieces );

	  if ( calculateWhiteKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & blackPieces ){
		break;
	  }

	}

	//printunsigned longAsBitBoard( testMap );

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;

}

///////////////////////////////////// MOVE BLACK ROOKS //////////////////////////////////////
int moveBlackRooksOrQueens( unsigned long b[], int pieceMapIndex  ){

  int numMovesFound = 0;

  unsigned long pieces = b[pieceMapIndex];
  unsigned long originalPieces = pieces;

  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];



  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;

	printf("Moving white rook:\n");
	printunsigned longAsBitBoard( pieces );
	printunsigned longAsBitBoard( pieceMap );
	printf("**********************\n");

	// for hver piece. flytt h&v til vi treffe noe
	int file = idx & 7;
	int rank = idx >> 3;

	unsigned long moveToMap = pieceMap;
	unsigned long clearMap = ~pieceMap;

	// up
	for( int m=0;m<7-rank;m++){
	  moveToMap <<= 8;

	  if( moveToMap & blackPieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

	  if ( calculateBlackKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & whitePieces ){
		break;
	  }

	}

	// left
	moveToMap = pieceMap;

	for( int m=0;m<7-file;m++){
	  moveToMap <<= 1;

	  if( moveToMap & blackPieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

	  	  if( move[IDX_MOVE_ID] == 11 || move[IDX_MOVE_ID] == 10 ){
	  		  printf("***************** <<=1 %ld \n",move[IDX_MOVE_ID]);
	  		  printBitBoard( move );
	  	  }

	  if ( calculateBlackKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & whitePieces ){
		break;
	  }

	}

	// right
	moveToMap = pieceMap;

	for( int m=0;m<file;m++){
	  moveToMap >>= 1;

	  if( moveToMap & blackPieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

	  if ( calculateBlackKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		dig(move);

	  }

	  if( moveToMap & whitePieces ){
		break;
	  }

	}

	// down
	moveToMap = pieceMap;

	for( int m=0;m<rank;m++){
	  moveToMap >>= 8;

	  if( moveToMap & blackPieces ){
		break;
	  }

	  makeNewBoard( b, move );
	  makeBlackMove( move, pieceMapIndex, moveToMap, clearMap, whitePieces );

	  if( move[IDX_MOVE_ID] == 10 ){
	  	printf("******************** >>=8 %ld \n",move[IDX_MOVE_ID]);
	  	printBitBoard( move );
	  }

	  if ( calculateBlackKingCheckStatus(move) == 0) {
		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
		numMovesFound++;
		dig(move);

	  }

	  if( moveToMap & whitePieces ){
		break;
	  }
	}

	//printunsigned longAsBitBoard( testMap );

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;

}

///////////////////////////////////// MOVE WHITE KNIGHTS //////////////////////////////////////
int moveWhiteKnights( unsigned long b[] ){

  int numMovesFound = 0;

  unsigned long pieces = b[IDX_WHITE_KNIGHTS];
  unsigned long originalPieces = pieces;

  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;
	unsigned long clearMap = ~pieceMap;

	unsigned long moveToMaps = KNIGHT_ATTACK_MAPS[idx];

	int moveToIdx = 0;

	while( moveToMaps ){

	  int moveToShift = __builtin_ctzll( moveToMaps );
	  moveToIdx += moveToShift;

	  unsigned long moveToMap = 1l<<moveToIdx;
	  if( !(moveToMap & whitePieces) ){

		makeNewBoard( b, move );
		if( moveToMap & blackPieces ){
		  move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
		  clearBlackPiecesWithClearMap( move, ~moveToMap );
		}

		move[IDX_WHITE_KNIGHTS] &= clearMap;
		move[IDX_WHITE_KNIGHTS] |= moveToMap;
		move[IDX_WHITE_PIECES] &= clearMap;
		move[IDX_WHITE_PIECES] |= moveToMap;

		move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|move[IDX_BLACK_PIECES];
		if ( calculateWhiteKingCheckStatus(move) == 0) {

		#ifdef TEST_IF_ON_R1_R8
		if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
		#endif
			move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

		  move[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(move);
		  numMovesFound++;
		  dig(move);

		}
	  }

	  moveToMaps >>= (moveToShift+1);
	  moveToIdx++;

	}

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;

}

///////////////////////////////////// MOVE BLACK KNIGHTS //////////////////////////////////////
int moveBlackKnights( unsigned long b[] ){

  int numMovesFound = 0;

  unsigned long pieces = b[IDX_BLACK_KNIGHTS];
  unsigned long originalPieces = pieces;

  int idx = 0;

  unsigned long move[NUM_BYTES];
  unsigned long allPieces = b[IDX_ALL_PIECES];
  unsigned long blackPieces = b[IDX_BLACK_PIECES];
  unsigned long whitePieces = b[IDX_WHITE_PIECES];

  while( pieces ){
	int shift =  __builtin_ctzll( pieces );
	idx += shift;
	unsigned long pieceMap = 1l << idx;
	unsigned long clearMap = ~pieceMap;

	unsigned long moveToMaps = KNIGHT_ATTACK_MAPS[idx];

	int moveToIdx = 0;

	while( moveToMaps ){

	  int moveToShift = __builtin_ctzll( moveToMaps );
	  moveToIdx += moveToShift;

	  unsigned long moveToMap = 1l<<moveToIdx;
	  if( !(moveToMap & blackPieces) ){

		makeNewBoard( b, move );
		makeBlackMove( move, IDX_BLACK_KNIGHTS, moveToMap, clearMap, whitePieces );

		if ( calculateBlackKingCheckStatus(move) == 0) {
			#ifdef TEST_IF_ON_R1_R8
			if( (pieceMap | moveToMap) & (R1_R8) ) // TODO: test om det er kjappere å ikke teste på dette.
			#endif
				move[IDX_CASTLING] &= ~(pieceMap | moveToMap); // TODO: Ta med denne på alle R8/R1 moves

			  move[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(move);
			  numMovesFound++;
			  dig(move);
    	}

	  }

	  moveToMaps >>= (moveToShift+1);
	  moveToIdx++;

	}

	pieces >>= (shift+1);
	idx++;

  }

  return numMovesFound;

}

//////////////////////////////// ----------- white bit promos ---------
int makeWhiteBitPromos( unsigned long board[], unsigned long newPieceMap ){

  int numMoves = 0;
  unsigned long clearPiece = ~newPieceMap;

  board[IDX_WHITE_PAWNS] &= clearPiece;
  board[IDX_WHITE_ROOKS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_WHITE_ROOKS] &= clearPiece;
  board[IDX_WHITE_QUEENS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_WHITE_QUEENS] &= clearPiece;
  board[IDX_WHITE_BISHOPS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_WHITE_BISHOPS] &= clearPiece;
  board[IDX_WHITE_KNIGHTS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(board);
  numMoves++;
  dig( board );

  return numMoves;
}

int makeBlackBitPromos( unsigned long board[], unsigned long newPieceMap ){

  int numMoves = 0;
  unsigned long clearPiece = ~newPieceMap;

  board[IDX_BLACK_PAWNS] &= clearPiece;
  board[IDX_BLACK_ROOKS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_BLACK_ROOKS] &= clearPiece;
  board[IDX_BLACK_QUEENS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_BLACK_QUEENS] &= clearPiece;
  board[IDX_BLACK_BISHOPS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
  numMoves++;
  dig( board );

  board[IDX_BLACK_BISHOPS] &= clearPiece;
  board[IDX_BLACK_KNIGHTS] |= newPieceMap;
  board[IDX_CHECK_STATUS] = calculateWhiteKingCheckStatus(board);
  numMoves++;
  dig( board );

  return numMoves;
}


void makeNewBoard( unsigned long oldBoard[], unsigned long newBoard[]) {

	makeNewBoardInvocations++;

	// TODO: Prøv å sette 0 på bare de bytsa som ikke blir aktivt kopiert, alle etter NUM_BYTES_TO_COPY til NUM_BYTES.
	memset(newBoard+NUM_BYTES_TO_COPY, 0, sizeof(unsigned long)*(NUM_BYTES-NUM_BYTES_TO_COPY)); // sizeof(int)*(NUM_BYTES-NUM_BYTES_TO_COPY)
	memcpy(newBoard, oldBoard,  sizeof(unsigned long)*NUM_BYTES_TO_COPY); // sizeof(int)*NUM_BYTES_TO_COPY

	newBoard[IDX_MOVE_ID] = makeNewBoardInvocations;
	newBoard[IDX_PARENT_MOVE_ID] = oldBoard[IDX_MOVE_ID];
 	newBoard[IDX_MOVE_NUM]++;
	newBoard[IDX_TURN] = oldBoard[IDX_TURN] ^ 4095;
	newBoard[IDX_CHECK_STATUS] = 0;

}



void makeWhiteMove( unsigned long move[], int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap, unsigned long blackPieces ){
	move[pieceMapIndex] &= clearMap;
	move[pieceMapIndex] |= moveToMap;
	move[IDX_WHITE_PIECES] &= clearMap;
	move[IDX_WHITE_PIECES] |= moveToMap;
	move[IDX_ALL_PIECES] = move[IDX_WHITE_PIECES]|blackPieces;

	if( moveToMap & blackPieces ){
		move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
		clearBlackPiecesWithClearMap( move, ~moveToMap );
	}
}

void makeBlackMove( unsigned long move[], int pieceMapIndex, unsigned long moveToMap, unsigned long clearMap, unsigned long whitePieces ){
	move[pieceMapIndex] &= clearMap;
	move[pieceMapIndex] |= moveToMap;
	move[IDX_BLACK_PIECES] &= clearMap;
	move[IDX_BLACK_PIECES] |= moveToMap;
	move[IDX_ALL_PIECES] = move[IDX_BLACK_PIECES]|whitePieces;

	if( moveToMap & whitePieces ){
		move[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
		clearWhitePiecesWithClearMap( move, ~moveToMap );
	}
}


int calculateWhiteKingCheckStatus( unsigned long board[] ){

  unsigned long king = board[IDX_WHITE_KING];
  unsigned long idx = __builtin_ctzll( king );

  if( KNIGHT_ATTACK_MAPS[idx] & board[IDX_BLACK_KNIGHTS] ){
	return MASK_WHITE_KING_CHECKED;
  }

  if( BLACK_PAWN_REVERSE_ATTACK_MAPS[idx] & board[IDX_BLACK_PAWNS] ){
	return MASK_WHITE_KING_CHECKED;
  }

  if( KING_ATTACK_MAPS[idx] & board[IDX_BLACK_KING] ){
	return MASK_WHITE_KING_CHECKED;
  }

  // now for the vector threats. a piece may be on the map, but can be blocked
  if( QB_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS])){

	// need to check each direction for a block
	unsigned long qb = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS]);
	unsigned long allPieces = board[IDX_ALL_PIECES];

	// if the only pieces intersecting the attackmap is a Q or B, then white
	// is in check
	unsigned long test = allPieces & QB_ATTACK_MAPS[idx];


	if( __builtin_popcountll (test ) == 1){
		return MASK_WHITE_KING_CHECKED;
	}

	int rank = idx >> 3;
	int file = idx & 7;

	unsigned long testMap = king;

	int maxM = 7-rank;
	maxM = 7-file < maxM ? 7-file : maxM;
	for( int m=0;m<maxM;m++){
		testMap <<= 9;

		if( testMap & qb ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}

	maxM = 7-rank;
	maxM = file < maxM ? file : maxM;
	testMap = king;
	for( int m=0;m<maxM;m++){

		testMap <<= 7;

		if( testMap & qb ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}


	maxM = rank;
	maxM = maxM > (7-file) ? (7-file) : maxM;
	testMap = king;
	for( int m=0;m<maxM;m++){
		testMap >>= 7;

		if( testMap & qb ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}

	maxM = rank;
	maxM = maxM > file ? file : maxM;
	testMap = king;
	for( int m=0;m<maxM;m++){
		testMap >>= 9;

		if( testMap & qb ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}
  }
  // now for the vector threats. a piece may be on the map, but can be blocked
  if( QR_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS])){

	// need to check each direction for a block
	unsigned long qr = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS]);
	unsigned long allPieces = board[IDX_ALL_PIECES];

	// if the only pieces intersecting the attackmap is a Q or B, then white
	// is in check
	unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
	if( __builtin_popcountll (test ) == 1){
	  return MASK_WHITE_KING_CHECKED;
	}

	// TODO : also hit if threatmap & piecemap == piecemap

	int rank = idx >> 3;
	int file = idx & 7;

	int maxM = file;
	unsigned long testMap = king;

	for( int m=0;m<maxM;m++){
		testMap >>= 1;
		if( testMap & qr ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}

	maxM = 7-file;
	testMap = king;
	for( int m=0;m<maxM;m++){
		testMap <<= 1;
		if( testMap & qr ){
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}

	maxM = rank;
	testMap = king;

	for( int m=0;m<maxM;m++){
		testMap >>= 8;
		if( testMap & qr ){
			//printf( "KING IS THREATENED BY QR ATTACK MAP by >> 8\n");
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}

	maxM = 7-rank;
	testMap = king;

	for( int m=0;m<maxM;m++){
		testMap <<= 8;
		if( testMap & qr ){
			//printf( "KING IS THREATENED BY QR ATTACK MAP by >> 8\n");
			return MASK_WHITE_KING_CHECKED;
		}
		if( testMap & allPieces){
			break;
		}
	}
  }

  return 0;

}

int calculateBlackKingCheckStatus( unsigned long board[] ){

	unsigned long king = board[IDX_BLACK_KING];
    unsigned long idx = __builtin_ctzll( king );

    if( KNIGHT_ATTACK_MAPS[idx] & board[IDX_WHITE_KNIGHTS] ){
  		return MASK_BLACK_KING_CHECKED;
    }

	// Yes, it is ok to use black pawn attack maps here.
    if( BLACK_PAWN_ATTACK_MAPS[idx] & board[IDX_WHITE_PAWNS] ){
  		return MASK_BLACK_KING_CHECKED;
    }

    if( KING_ATTACK_MAPS[idx] & king ){
  		return MASK_BLACK_KING_CHECKED;
    }

    // now for the vector threats. a piece may be on the map, but can be blocked
    if( QB_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS])){

	  	// need to check each direction for a block
	  	unsigned long qb = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_BISHOPS]);
	  	unsigned long allPieces = board[IDX_ALL_PIECES];

	  	// if the only pieces intersecting the attackmap is a Q or B, then white
	  	// is in check
	  	unsigned long test = allPieces & QB_ATTACK_MAPS[idx];

	  	if( __builtin_popcountll (test ) == 1){
		 	return MASK_BLACK_KING_CHECKED;
	  	}

	  	int rank = idx >> 3;
	  	int file = idx & 7;

	  	unsigned long testMap = king;
	  	// TODO: regn up hvor langt testen skal gå. ikke noe vits i å teste for hver file&dRank
	  	// pluss at vi kan bruke bitmap i stedet for
	  	int maxM = 7-rank;
	  	maxM = 7-file < maxM ? 7-file : maxM;
	  	for( int m=0;m<maxM;m++){
	  		testMap <<= 9;

	  		if( testMap & qb ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

	  	maxM = 7-rank;
	  	maxM = file < maxM ? file : maxM;
	  	testMap = king;
	  	for( int m=0;m<maxM;m++){
	  		testMap <<= 7;

	  		if( testMap & qb ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}


	  	maxM = rank;
	  	maxM = maxM > (7-file) ? (7-file) : maxM;
	  	testMap = king;
	  	for( int m=0;m<maxM;m++){
	  		testMap >>= 7;

	  		if( testMap & qb ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

	  	maxM = rank;
	  	maxM = maxM > file ? file : maxM;
	  	testMap = king;
	  	for( int m=0;m<maxM;m++){
	  		testMap >>= 9;

	  		if( testMap & qb ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}
    }
    // now for the vector threats. a piece may be on the map, but can be blocked
    if( QR_ATTACK_MAPS[idx] & (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS])){

	  	// need to check each direction for a block
	  	unsigned long qr = (board[IDX_WHITE_QUEENS] | board[IDX_WHITE_ROOKS]);
	  	unsigned long allPieces = board[IDX_ALL_PIECES];

	  	// if the only pieces intersecting the attackmap is a Q or B, then white
	  	// is in check
	  	unsigned long test = allPieces & QR_ATTACK_MAPS[idx];
	  	if( __builtin_popcountll (test ) == 1){
	  	  return MASK_BLACK_KING_CHECKED;
	  	}

	  	// TODO : also hit if threatmap & piecemap == piecemap

	  	int rank = idx >> 3;
	  	int file = idx & 7;

	  	int maxM = file;
	  	unsigned long testMap = king;

	  	for( int m=0;m<maxM;m++){
	  		testMap >>= 1;
	  		if( testMap & qr ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

	  	maxM = 7-file;
	  	testMap = king;
	  	for( int m=0;m<maxM;m++){
	  		testMap <<= 1;
	  		if( testMap & qr ){
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

	  	maxM = rank;
	  	testMap = king;

	  	for( int m=0;m<maxM;m++){
	  		testMap >>= 8;
	  		if( testMap & qr ){
	  			//printf( "KING IS THREATENED BY QR ATTACK MAP by >> 8\n");
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

	  	maxM = 7-rank;
	  	testMap = king;

	  	for( int m=0;m<maxM;m++){
	  		testMap <<= 8;
	  		if( testMap & qr ){
	  			//printf( "KING IS THREATENED BY QR ATTACK MAP by >> 8\n");
	  			return MASK_BLACK_KING_CHECKED;
	  		}
	  		if( testMap & allPieces){
	  			break;
	  		}
	  	}

    }

    return 0;
}



void influenceMapForSquare(unsigned long b[], int idx) {

	influenceMapForSquareInvocations++;

	unsigned long* influenceMap = b+IDX_START_INFLUENCE_MAP;

	for( int t=0;t<16;t++){
	  influenceMap[t] = 0;
	}

	// the 8 first are the results from linear runs.
	// the next 8 results are possible knight positions

	int rank = idx >> 3;
	int file = idx & 7;

	int mapping[] = {2,0,2,0,0,1,0,1};
	for (int direction = 0; !(direction & 8 ) ; direction ++) {

		int dd = direction << 1;
		int newFile = file;
		int newRank = rank;

		for (int n = 1; (n & 8) == 0; n++) {

			newFile += MOVE_MATRIX[ dd ];
			newRank += MOVE_MATRIX[ dd | 1];

			// Check that 0 >= newRank < 8 && 0 >= newFile < 8
			if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
				int checkIdx = newRank << 3 | newFile;
				if (b[checkIdx] != 0) {

				  unsigned long checkPiece = b[checkIdx];

				  if ( (checkPiece & Pieces_PpKk ) != 0 && n > 1) {
					  break;
				  }

				  if  ( (checkPiece & Pieces_Nn) == 0 ) {
					  influenceMap[direction] = checkPiece;
				  }
				  break;
				}
			}
			else {
				break;
			}

		}
	}

	for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
		int newFile = file + KNIGHT_MOVE_MATRIX[t];
		int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];

		// Check that 0 > newRank|newFile < 8
		if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
			int checkIdx = newRank << 3 | newFile;
			unsigned long checkPiece = b[checkIdx];
			if ( (checkPiece & Pieces_Nn) != 0) {
				influenceMap[8 | (t >> 1)] = checkPiece;
			}
		}
	}

}

void clearBlackPiecesWithClearMap( unsigned long board[], unsigned long clear ){
	for( int t=IDX_BLACK_PAWNS;t<=IDX_BLACK_KING;t++){ // no need to clear the king, ever. hopefully. but test forst. todo:test
		board[t] &= clear;
	}
	board[IDX_BLACK_PIECES] &= clear;
}

void clearWhitePiecesWithClearMap( unsigned long board[], unsigned long clear ){
	for( int t=IDX_WHITE_PAWNS;t<=IDX_WHITE_KING;t++){ // no need to clear the king, ever. hopefully. but test forst. todo:test
		board[t] &= clear;
	}
	board[IDX_WHITE_PIECES] &= clear;
}


void clearBlackPieceAtIndex( unsigned long board[], int idx ){

	unsigned long clear = ~(1l<<idx);
	for( int t=IDX_BLACK_PAWNS;t<=IDX_BLACK_KING;t++){ // no need to clear the king, ever. hopefully. but test forst. todo:test
	  board[t] &= clear;
	}
	board[IDX_ALL_PIECES] &= clear;
	board[IDX_BLACK_PIECES] &= clear;
}

void clearWhitePieceAtIndex( unsigned long board[], int idx ){
	unsigned long clear = ~(1l<<idx);
	for( int t=IDX_WHITE_PAWNS;t<=IDX_WHITE_KING;t++){ // no need to clear the king, ever. hopefully. but test first. todo:test
	  board[t] &= clear;
	}
	board[IDX_ALL_PIECES] &= clear;
	board[IDX_WHITE_PIECES] &= clear;
}


void clearPieceAtIndex( unsigned long board[], int idx ){
	unsigned long clear = ~(1l<<idx);
	for( int t=IDX_WHITE_PAWNS;t<=IDX_BLACK_KING;t++){
	  board[t] &= clear;
	}
}


void count(unsigned long b[]) {

	const int level = b[IDX_MOVE_NUM];
	numMoves[level]++;
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CAPTURE) ) {
		numCaptures[level]++;
	}
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_EP_STRIKE) ) {
		numEP[level]++;
	}
	if ((b[IDX_LAST_MOVE_WAS] & (MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE | MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE)) ) {
		numCastles[level]++;
	}
	if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_PROMO) ) {
		numPromos[level]++;
	}
	if ((b[IDX_CHECK_STATUS] & (MASK_WHITE_KING_CHECKED | MASK_BLACK_KING_CHECKED)) ) {
		numChecks[level]++;
	}
	if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_MATED) ) {
		numCheckmates[level]++;
	}

	/*if( level == 3 && b[IDX_CHECK_STATUS] & (MASK_WHITE_KING_CHECKED | MASK_BLACK_KING_CHECKED) ){
	  printBoard( b);
	}*/
}





/************************************************************************************************************
 **																										**
 **												  UTILS												 **
 **																										**
 ************************************************************************************************************/

void printNumBoard( unsigned long board[] ){
	for( int s=0;(s & 64) == 0;s++){
		if( s % 8 == 0 && s != 0 ){
			printf("\n");
		}
		printf( " %ld\t", board[s] );
	}
}

void printDiagram( unsigned long board[] ){
  printf("\"");
  for( int s=0;(s & 64) == 0;s++){

	if( s % 8 == 0 && s != 0 ){
		printf(" ");
	}

	  switch( board[s] ){
		  case Piece_P:printf("P");break;
		  case Piece_R:printf("R");break;
		  case Piece_N:printf("N");break;
		  case Piece_B:printf("B");break;
		  case Piece_Q:printf("Q");break;
		  case Piece_K:printf("K");break;
		  case Piece_p:printf("p");break;
		  case Piece_r:printf("r");break;
		  case Piece_n:printf("n");break;
		  case Piece_b:printf("b");break;
		  case Piece_q:printf("q");break;
		  case Piece_k:printf("k");break;
		  default:
			  printf(".");
	  }

  }
  printf( "\"");
  printf( " %s", board[IDX_TURN] == WHITE_MASK ? "w" : "b" );
  printf( " %ld", board[IDX_CASTLING] );

  printf("\n");

}

void printunsigned longAsBitBoard( unsigned long bitstream ){
  printf( "  A B C D E F G H");
  unsigned long backup = bitstream;
  char str[65] = "................................................................\0";
  for( int t=0;t<64;t++){
	if( 1 & bitstream ){
		str[63-t] = '1';
	}
	bitstream >>= 1;
  }

  for( int s=0;(s & 64) == 0;s++){
	  if( s % 8 == 0 ){
		  printf("\n%d ",8-(s>>3));
	  }
	  printf("%c ", str[s] );
  }
  printf("\n");
  printf("\n%#lx",backup);
  printf("\n%lu\n",backup);
  printf("\n\n");
}

void setBitsToChar( char *str, unsigned long bits, char c){

	unsigned long originalBits = bits;
  int idx = 0;
  while( bits && idx < 64){
	int shift =  __builtin_clzll( bits );
	idx += shift;
	bits <<= (shift+1);
	str[idx] = c;
	idx++;
  }

}

void printBitBoard( unsigned long board[] ){
	printf( "\n  A B C D E F G H");
	char str[] = ".................................................................\0";

	setBitsToChar( str, board[IDX_WHITE_PAWNS],   'P' );
	setBitsToChar( str, board[IDX_WHITE_ROOKS],   'R' );
	setBitsToChar( str, board[IDX_WHITE_BISHOPS], 'B' );
	setBitsToChar( str, board[IDX_WHITE_KNIGHTS], 'N' );
	/*if( board[IDX_MOVE_ID] == 14){
		printf(" abouut to crash\n");
		printunsigned longAsBitBoard( board[IDX_WHITE_QUEENS] );
	 	fflush(stdout);
	}*/
	setBitsToChar( str, board[IDX_WHITE_QUEENS],  'Q' );
	setBitsToChar( str, board[IDX_WHITE_KING],    'K' );

	setBitsToChar( str, board[IDX_BLACK_PAWNS],   'p' );
	setBitsToChar( str, board[IDX_BLACK_ROOKS],   'r' );
	setBitsToChar( str, board[IDX_BLACK_BISHOPS], 'b' );
	setBitsToChar( str, board[IDX_BLACK_KNIGHTS], 'n' );
	setBitsToChar( str, board[IDX_BLACK_QUEENS],  'q' );
	setBitsToChar( str, board[IDX_BLACK_KING],    'k' );

	if( board[IDX_EP_IDX] ){
		setBitsToChar( str, board[IDX_EP_IDX], '*' );
	}

	for( int s=0;(s & 64) == 0;s++){
		if( s % 8 == 0 ){
			printf("\n%d ",8-(s>>3));
		}
		printf("%c ", str[s] );
	}
	printf("\n");
	printf( "Move num: %ld\n", board[IDX_MOVE_NUM] );
	printf( "Move ID: %ld\n", board[IDX_MOVE_ID]);
	printf( "Parent ID: %ld\n", board[IDX_PARENT_MOVE_ID]);


	printf( "Turn : %s\n", board[IDX_TURN] == WHITE_MASK ? "White" : "Black" );


	if ((board[IDX_CHECK_STATUS] & MASK_KING_IS_MATED) ) {
		printf( "There is a MATE\n");
	}
	if( board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED ) {
		printf( "White is in check\n");
	}
	if( board[IDX_CHECK_STATUS] & MASK_BLACK_KING_CHECKED ) {
		printf( "Black is in check\n");
	}
	if( board[IDX_EP_IDX]){
		printf( "EP Index:%d\n",__builtin_ctzll(board[IDX_EP_IDX]));
	}

	printf( "Castling: %s%s%s%s\n",
	  ( board[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE)  == MASK_CASTLING_WHITE_KING_SIDE  ? "K" : "",
	  ( board[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) == MASK_CASTLING_WHITE_QUEEN_SIDE ? "Q" : "",
	  ( board[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE)  == MASK_CASTLING_BLACK_KING_SIDE  ? "k" : "",
	  ( board[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) == MASK_CASTLING_BLACK_QUEEN_SIDE ? "q" : ""
	)  ;
	printf("\n");

	fflush(stdout);

}


void diagramToBitBoard( unsigned long board[], char diagram[] ){

	//
	// 8  0,0,0,0,0,0,0,0,
	// 7  0,0,0,0,0,0,0,0,
	// 6  0,0,0,0,0,0,0,0,
	// 5  0,0,0,0,0,0,0,0,
	// 4  0,0,0,0,0,0,0,0,
	// 3  0,0,0,0,0,0,0,0,
	// 2  0,0,0,0,0,0,0,0,
	// 1  0,0,0,0,0,0,0,0,
	//	A B C D E F G H

	int len = strlen( diagram );

	memset(board, 0,  sizeof(unsigned long)*NUM_BYTES);
	board[IDX_CASTLING] =
	  MASK_CASTLING_WHITE_QUEEN_SIDE |
	  MASK_CASTLING_WHITE_KING_SIDE |
	  MASK_CASTLING_BLACK_QUEEN_SIDE |
	  MASK_CASTLING_BLACK_KING_SIDE;

	board[IDX_TURN] = WHITE_MASK;

	int pos = 0;

	for( int t=0;t<len;t++){
	  switch( diagram[t] ){
	  // BLACK PIECES
		  case '.':
			  pos++;
			  break;
		  case 'p':
			  board[IDX_BLACK_PAWNS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'r':
			  board[IDX_BLACK_ROOKS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'b':
			  board[IDX_BLACK_BISHOPS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'n':
			  board[IDX_BLACK_KNIGHTS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'q':
			  board[IDX_BLACK_QUEENS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'k':
			  board[IDX_BLACK_KING] = (1l << (63-pos));
			  pos++;
			  break;
	  // WHITE PIECES
		  case 'P':
			  board[IDX_WHITE_PAWNS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'R':
			  board[IDX_WHITE_ROOKS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'N':
			  board[IDX_WHITE_KNIGHTS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'B':
			  board[IDX_WHITE_BISHOPS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'Q':
			  board[IDX_WHITE_QUEENS] |= (1l << (63-pos));
			  pos++;
			  break;
		  case 'K':
			  board[IDX_WHITE_KING] = (1l << (63-pos));
			  pos++;
			  break;

		}// switch

	} // 0..len

	board[IDX_WHITE_PIECES] = board[IDX_WHITE_PAWNS]|
							  board[IDX_WHITE_ROOKS]|
							  board[IDX_WHITE_KNIGHTS]|
							  board[IDX_WHITE_BISHOPS]|
							  board[IDX_WHITE_QUEENS]|
							  board[IDX_WHITE_KING];

	board[IDX_BLACK_PIECES] = board[IDX_BLACK_PAWNS]|
							  board[IDX_BLACK_ROOKS]|
							  board[IDX_BLACK_KNIGHTS]|
							  board[IDX_BLACK_BISHOPS]|
							  board[IDX_BLACK_QUEENS]|
							  board[IDX_BLACK_KING];

	board[IDX_ALL_PIECES]   = board[IDX_WHITE_PIECES]|
							  board[IDX_BLACK_PIECES];



} // diagramToBitBoard

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"

typedef enum { FALSE, TRUE } boolean;

void diagramToByteBoard( long board[], char diagram[] );
void printBoard( long board[] );
void printDiagram( long board[] );
void printNumBoard( long board[] );
long printStats();

// Variations
// - Memory-variations:
//   - Byte Board
//   - Bitboard
// - Strategy-Variations
//   - Proactive, testing for check after each possible move generation.
//   - Retroactive, testing for check by testing if an attacking piece hits a
//     king in a possible move, making the PRREVIOUS move illegal.

// Engine methods
void dig(long board[]);
void count(long board[]);
void influenceMapForSquare(long b[], int idx);
int findAllPossibleMoves(long originalBoard[]);
boolean isSquaresThreatenedByColor(long board[], int indices[], int color);
void makeNewBoard( long originalBoard[], long newBoard[] );
int calculateCheckStatus(long board[]);
int makeBlackPromotions(long b[], int from, int to, int moveMask, int castlingMask);
int makeWhitePromotions(long b[], int from, int to, int moveMask, int castlingMask);
int moveLinear(long b[], int fromIdx, const int moveMatrix[], const int moveMatrixLength);

// bitboard functions
void diagramToBitBoard( long board[], char diagram[] );
void printBitBoard( long board[] );
int calculateWhiteKingCheckStatus( long board[] );
int calculateBlackKingCheckStatus( long board[] );
void printLongAsBitBoard( long bitstream );
int isOneBitSet( int i );
int numberOfSetBits(int i);
void clearPieceAtIndex( long board[], int idx );

int MAX_LEVEL = 1;
long numMoves[]      = {0,0,0,0,0,0,0,0,0,0,0};
long numCaptures[]   = {0,0,0,0,0,0,0,0,0,0,0};
long numEP[]         = {0,0,0,0,0,0,0,0,0,0,0};
long numCastles[]    = {0,0,0,0,0,0,0,0,0,0,0};
long numPromos[]     = {0,0,0,0,0,0,0,0,0,0,0};
long numChecks[]     = {0,0,0,0,0,0,0,0,0,0,0};
long numCheckmates[] = {0,0,0,0,0,0,0,0,0,0,0};

long calculateCheckStatusInvocations = 0;
long makeNewBoardInvocations = 0;
long isSquaresThreatenedByColorInvocations = 0;
long influenceMapForSquareInvocations = 0;
long moveLinearInvocations = 0;

char *workUnitId = NULL;


const long PIECE_COMBOS =
  (long)Pieces_QBK        |  (long)Pieces_QB <<  6 |
  (long)Pieces_QBPK << 12 |  (long)Pieces_QB << 18 |
  (long)Pieces_QRK <<  24 |  (long)Pieces_QR << 30 |
  (long)Pieces_QRK <<  36 |  (long)Pieces_QR << 42
  ;

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

   char *initialBoard = "\
                        . . . . . . . .\
                        . . . . . . . .\
                        . . . . . . . .\
                        . . . . K . . .\
                        . . . . . . . .\
                        . . . r b n . .\
                        . . . . P . . .\
                        . . . . . . . .";


   /*char *initialBoard = "\
                      r . . . k . . r\
                      p . p p q p b .\
                      b n . . p n p .\
                      . . . P N . . .\
                      . p . . P . . .\
                      . . N . . Q . p\
                      P P P B B P P P\
                      R . . . K . . R";*/

    long board[NUM_BYTES];

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
  //  printLongAsBitBoard( board[IDX_ALL_PIECES]  );

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

    long total = printStats();

    printf("\nTotal valid moves found : %lu \n" ,total);

    printf("Time spent: %ld.%09ld \n", (long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);

    printf( "%lu calculateCheckStatusInvocations\n", calculateCheckStatusInvocations );
    printf( "%lu makeNewBoardInvocations\n", makeNewBoardInvocations );
    printf( "%lu isSquaresThreatenedByColorInvocations\n", isSquaresThreatenedByColorInvocations );
    printf( "%lu influenceMapForSquareInvocations\n", influenceMapForSquareInvocations );
    printf( "%lu moveLinearInvocations\n", moveLinearInvocations );

    return 0;
} // end main

long printStats(){

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

  static long total = 0;

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
 **                                                                                                        **
 **                                                 ENGINE                                                 **
 **                                                                                                        **
 ************************************************************************************************************/


void dig(long board[]){

    if (board[IDX_MOVE_NUM] < MAX_LEVEL
       || (board[IDX_MOVE_NUM] == MAX_LEVEL && board[IDX_CHECK_STATUS] != 0) // Include to test for mates on the last level
    ) {
        int numMoves = findAllPossibleMoves(board);
        if (numMoves == 0 && board[IDX_CHECK_STATUS] != 0 ) {
            board[IDX_CHECK_STATUS] |= MASK_KING_IS_MATED;
        }
    }
    count(board);
    if( MAX_LEVEL <= 3 ){
      if( board[IDX_MOVE_NUM] == MAX_LEVEL){
        printBitBoard( board );
      }
    }
    /*if( board[IDX_MOVE_NUM] == 2){
      printStats();
    }*/
}


int findAllPossibleMoves(long originalBoard[]) {

    int numMovesFound = 0;

    for (int pieceIndex = 0; pieceIndex<32 ; pieceIndex++) {

        long pieceInfo = originalBoard[pieceIndex];
        long fromIdx = pieceInfo >> 12;
        long p = pieceInfo & 0xFFF;
        long pieceMask = 1l<<fromIdx;

        if (p == 0) {
            continue;
        }

        int rank = fromIdx >> 3;
        int file = fromIdx & 7;

        if (originalBoard[IDX_TURN] == WHITE_MASK) { // turn == white
            /*
             *******************************************************************************************
             ***************************************  WHITE  *******************************************
             *******************************************************************************************
             */
            if (p == Piece_P) {

                long fromIdxm8Map = 1l<<(fromIdx-8);
                long fromIdxm7Map = 1l<<(fromIdx-7);
                long fromIdxm9Map = 1l<<(fromIdx-9);
                long fromIdxm16Map = 1l<<(fromIdx-16);

                // White pawn
                // Check if one and/or two moves ahead can be made
                long m[NUM_BYTES];

                long allPieces = originalBoard[IDX_ALL_PIECES];

                if ( (allPieces & fromIdxm8Map) == 0 ) { // go one up
                    if (rank > 1) {
                        makeNewBoard(originalBoard,m);
                        clearPieceAtIndex( m, fromIdx );
                        clearPieceAtIndex( m, fromIdx-8 );
                        m[pieceIndex] = ((fromIdx-8)<<12)|Piece_P ;
                        m[IDX_WHITE_PAWNS] |= fromIdxm8Map;
                        m[IDX_WHITE_PIECES] |= fromIdxm8Map;
                        m[IDX_ALL_PIECES] = m[IDX_WHITE_PIECES]|m[IDX_BLACK_PIECES];

                        // update pawns & all white pieces
                        if ( calculateWhiteKingCheckStatus(m) == 0) {
                            m[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(m);;
                            numMovesFound++;
                            dig(m);
                        }
                    }
                    else {
                        // if target = any one rook corner, adjust castling aproprieately
                        /*int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm8Map == A8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_QUEEN_SIDE;
                        }
                        else if (fromIdxm8Map == H8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_KING_SIDE;
                        }
                        numMovesFound += makeWhitePromotions(originalBoard, fromIdx, fromIdxm8Map, MASK_EMPTY, castlingMask);
                        */
                    }
                }
                if (rank == 6) {
                    if ( (allPieces & fromIdxm8Map) == 0 && (allPieces & fromIdxm16Map) == 0) {

                        makeNewBoard(originalBoard,m);
                        m[pieceIndex] = ((fromIdx-16)<<12)|Piece_P ;
                        m[IDX_WHITE_PAWNS] ^= pieceMask;
                        m[IDX_WHITE_PAWNS] |= fromIdxm16Map;
                        m[IDX_WHITE_PIECES] ^=  pieceMask;
                        m[IDX_WHITE_PIECES] |= fromIdxm16Map;
                        m[IDX_ALL_PIECES] = m[IDX_WHITE_PIECES]|m[IDX_BLACK_PIECES];
                        m[IDX_EP_IDX] = fromIdx-8;
                        if ( calculateWhiteKingCheckStatus(m) == 0) {
                            m[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(m);;
                            numMovesFound++;
                            dig(m);
                        }
                    }
                }

                if (file > 0 && (originalBoard[IDX_BLACK_PIECES] & fromIdxm9Map ) != 0) { // strike left

                  if (rank > 1) {
                      makeNewBoard(originalBoard,m);
                      clearPieceAtIndex( m, fromIdx );
                      clearPieceAtIndex( m, fromIdx-9 );
                      m[pieceIndex] = ((fromIdx-9)<<12)|Piece_P ;
                      m[IDX_WHITE_PAWNS] |= fromIdxm9Map;
                      m[IDX_WHITE_PIECES] |= fromIdxm9Map;
                      m[IDX_ALL_PIECES] = m[IDX_WHITE_PIECES] | m[IDX_BLACK_PIECES];

                      // update pawns & all white pieces
                      if ( calculateWhiteKingCheckStatus(m) == 0) {
                          m[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(m);
                          numMovesFound++;
                          dig(m);
                      }
                  }/*
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm9Map == A8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_QUEEN_SIDE;
                        }
                        numMovesFound += makeWhitePromotions(originalBoard, fromIdx, fromIdxm9Map, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }*/

                }

                if (file < 7 && (originalBoard[IDX_BLACK_PIECES] & fromIdxm7Map) != 0) { // strike right

                  if (rank > 1) {
                      makeNewBoard(originalBoard,m);
                      clearPieceAtIndex( m, fromIdx );
                      clearPieceAtIndex( m, fromIdx-7 );
                      m[pieceIndex] = ((fromIdx-7)<<12)|Piece_P ;
                      m[IDX_WHITE_PAWNS] |= fromIdxm7Map;
                      m[IDX_WHITE_PIECES] |= fromIdxm7Map;
                      m[IDX_ALL_PIECES] = m[IDX_WHITE_PIECES] | m[IDX_BLACK_PIECES];

                      // update pawns & all white pieces
                      if ( calculateWhiteKingCheckStatus(m) == 0) {
                          m[IDX_CHECK_STATUS] = calculateBlackKingCheckStatus(m);
                          numMovesFound++;
                          dig(m);
                      }
                  }
                  /*
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm7Map == H8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_KING_SIDE;
                        }
                        numMovesFound += makeWhitePromotions(originalBoard, fromIdx, fromIdxm7Map, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }*/
                }/*
                if (originalBoard[IDX_EP_IDX] != 0 && rank == 3) {
                    if (file < 7 && fromIdxm7Map == originalBoard[IDX_EP_IDX]) {

                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxm7Map] = p;
                        m[fromIdx + 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;
                        const int checkStatus = calculateCheckStatus(m);
                        m[IDX_CHECK_STATUS] = checkStatus;
                        if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                            numMovesFound++;
                            dig(m);
                        }

                    }
                    else if (file > 0 && fromIdxm9Map == originalBoard[IDX_EP_IDX]) {

                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxm9Map] = p;
                        m[fromIdx - 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        const int checkStatus = calculateCheckStatus(m);
                        m[IDX_CHECK_STATUS] = checkStatus;
                        if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                            numMovesFound++;
                            dig(m);
                        }
                    }
                }*/
            }
/*
            else if (p == Piece_N) {
                // White Knight

                for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + KNIGHT_MOVE_MATRIX[t];
                    int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];

                    if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & WHITE_MASK) == 0) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;

                            const int checkStatus = calculateCheckStatus(newBoard);
                            newBoard[IDX_CHECK_STATUS] = checkStatus;
                            if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                                numMovesFound++;
                                dig(newBoard);
                            }

                        }
                    }
                }
            }
            else if (p == Piece_R) {
                // White rook
                numMovesFound += moveLinear(originalBoard, fromIdx, ROOK_MOVE_MATRIX,ROOK_MOVE_MATRIX_LENGTH);

            }
            else if (p == Piece_B) {
                // White Bishop
                numMovesFound += moveLinear(originalBoard, fromIdx, BISHOP_MOVE_MATRIX, BISHOP_MOVE_MATRIX_LENGTH);

            }
            else if (p == Piece_Q) {
                // White Queen
                numMovesFound += moveLinear(originalBoard, fromIdx, MOVE_MATRIX, MOVE_MATRIX_LENGTH);
            }
            else if (p == Piece_K) {

                // White King Normal Moves

                for (int t = 0; t < MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + MOVE_MATRIX[t];
                    int newRank = rank + MOVE_MATRIX[t | 1];

                    if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
                        int toIdx = newRank << 3 | newFile;

                        if (originalBoard[toIdx] == 0 || (originalBoard[toIdx] & BLACK_MASK) != 0) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if ((originalBoard[toIdx] & BLACK_MASK) != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;
                            newBoard[IDX_WHITE_KING_INDEX] = toIdx;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_KING_SIDE | MASK_CASTLING_WHITE_QUEEN_SIDE);

                            const int checkStatus = calculateCheckStatus(newBoard);

                            if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                                newBoard[IDX_CHECK_STATUS] = checkStatus;
                                numMovesFound++;
                                dig(newBoard);

                            }
                        }
                    }
                }


                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) != 0) {
                    // Just do it. Then mark board as did castle queen side, check of squares were threatened on next
                    if (originalBoard[A1] == Piece_R && originalBoard[B1] == 0 && originalBoard[C1] == 0 && originalBoard[D1] == 0 && originalBoard[E1] == Piece_K) {

                        int squares[] = {C1, D1};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, BLACK_MASK)) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
                            newBoard[A1] = 0;
                            newBoard[E1] = 0;
                            newBoard[C1] = Piece_K;
                            newBoard[D1] = Piece_R;
                            newBoard[IDX_WHITE_KING_INDEX] = C1;

                            const int checkStatus = calculateCheckStatus(newBoard);
                            if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                                newBoard[IDX_CHECK_STATUS] = checkStatus;
                                numMovesFound++;
                                dig(newBoard);
                            }
                        }

                    }
                }
                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) != 0) {

                    if (originalBoard[H1] == Piece_R && originalBoard[G1] == 0 && originalBoard[F1] == 0 && originalBoard[E1] == Piece_K) {

                        int squares[] = {G1, F1};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, BLACK_MASK)) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
                            newBoard[H1] = 0;
                            newBoard[E1] = 0;
                            newBoard[G1] = Piece_K;
                            newBoard[F1] = Piece_R;
                            newBoard[IDX_WHITE_KING_INDEX] = G1;

                            const int checkStatus = calculateCheckStatus(newBoard);
                            if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                                newBoard[IDX_CHECK_STATUS] = checkStatus;
                                numMovesFound++;
                                dig(newBoard);
                            }
                        }
                    }
                }
            }*/
        }
        /*
         *******************************************************************************************
         ***************************************  BLACK  *******************************************
         *******************************************************************************************
         */
        else { // black's turn
            if (p == Piece_p) {

                int fromIdxp7 = fromIdx + 7;
                int fromIdxp8 = fromIdx + 8;
                int fromIdxp9 = fromIdx + 9;
                int fromIdxp16 = fromIdx + 16;


                // Black pawn
                // Check if one and/or two moves ahead can be made
                if (originalBoard[fromIdxp8] == 0) {
                    if (rank < 6) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp8] = p;

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }

                    }
                    else { // will move to promotion
                        // if target = any one rook corner, adjust castling aproprieately
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxp8 == A1) {
                            castlingMask &= 0xf ^ MASK_CASTLING_WHITE_QUEEN_SIDE;
                        }
                        else if (fromIdxp8 == H1) {
                            castlingMask &= 0xf ^ MASK_CASTLING_WHITE_KING_SIDE;
                        }
                        numMovesFound += makeBlackPromotions(originalBoard, fromIdx, (fromIdxp8), MASK_EMPTY, castlingMask);
                    }
                }
                if (rank == 1) {
                    if (originalBoard[fromIdxp16] == 0 && originalBoard[fromIdxp8] == 0) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp16] = p;
                        m[IDX_EP_IDX] = fromIdxp8; // set en passant strike index

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }

                    }
                }
                if (file < 7 && (originalBoard[fromIdxp9] & WHITE_MASK) != 0) { // strike left as seen from black

                    if (rank < 6) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp9] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }

                    }
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxp9 == H1) {
                            castlingMask &= 0xf ^ MASK_CASTLING_WHITE_KING_SIDE;
                        }
                        numMovesFound += makeBlackPromotions(originalBoard, fromIdx, fromIdxp9, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }

                }
                if (file > 0 && (originalBoard[fromIdxp7] & WHITE_MASK) != 0) { // strike right as seen from black

                    if (rank < 6) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxp7] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }

                    }
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxp7 == A1) {
                            castlingMask &= 0xf ^ MASK_CASTLING_WHITE_QUEEN_SIDE;
                        }

                        numMovesFound += makeBlackPromotions(originalBoard, fromIdx, fromIdxp7, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }
                }
                if (originalBoard[IDX_EP_IDX] != 0 && rank == 4) {
                    if (file < 7 && fromIdxp9 == originalBoard[IDX_EP_IDX]) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxp9] = p;
                        m[fromIdx + 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }

                    }
                    else if (file > 0 && fromIdxp7 == originalBoard[IDX_EP_IDX]) {
                        long m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp7] = p;
                        m[fromIdx - 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        int checkStatus = calculateCheckStatus(m);
                        if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                            m[IDX_CHECK_STATUS] = checkStatus;
                            numMovesFound++;
                            dig(m);
                        }
                    }
                }
            }
            else if (p == Piece_r) {
                // Black rook
                numMovesFound += moveLinear(originalBoard, fromIdx, ROOK_MOVE_MATRIX, ROOK_MOVE_MATRIX_LENGTH);

            }
            else if (p == Piece_n) {
                // Black Knight

                for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + KNIGHT_MOVE_MATRIX[t];
                    int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];
                    if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & BLACK_MASK) == 0) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;

                            int checkStatus = calculateCheckStatus(newBoard);
                            if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                                newBoard[IDX_CHECK_STATUS] = checkStatus;
                                numMovesFound++;
                                dig(newBoard);
                            }

                        }
                    }
                }

            }
            else if (p == Piece_b) {
                // Black Bishop
                numMovesFound += moveLinear(originalBoard, fromIdx, BISHOP_MOVE_MATRIX, BISHOP_MOVE_MATRIX_LENGTH);

            }
            else if (p == Piece_q) {
                // Black Queen
                numMovesFound += moveLinear(originalBoard, fromIdx, MOVE_MATRIX, MOVE_MATRIX_LENGTH);

            }
            else if (p == Piece_k) {
                // Black King

                for (int t = 0; t < 16; t += 2) {
                    int newFile = file + MOVE_MATRIX[t];
                    int newRank = rank + MOVE_MATRIX[t | 1];
                    if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & BLACK_MASK) == 0) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;
                            newBoard[IDX_BLACK_KING_INDEX] = toIdx;

                            int checkStatus = calculateCheckStatus(newBoard);
                            if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
                                newBoard[IDX_CHECK_STATUS] = checkStatus;

                                numMovesFound++;
                                dig(newBoard);
                            }
                        }
                    }
                }


                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) != 0) {

                    if (originalBoard[A8] == Piece_r && originalBoard[B8] == 0 && originalBoard[C8] == 0 && originalBoard[D8] == 0 && originalBoard[E8] == Piece_k) {

                        int squares[] = {C8,D8};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_BLACK_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, WHITE_MASK)) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
                            newBoard[A8] = 0;
                            newBoard[E8] = 0;
                            newBoard[C8] = Piece_k;
                            newBoard[D8] = Piece_r;
                            newBoard[IDX_BLACK_KING_INDEX] = C8;

                            numMovesFound++;
                            dig(newBoard);
                        }
                    }
                }

                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_BLACK_KING_SIDE) != 0) {
                    // Just do it. Then mark board as did castle queen side, check of squares were threatened on next

                    int squares[] = {F8,G8};

                    if (originalBoard[H8] == Piece_r && originalBoard[G8] == 0 && originalBoard[F8] == 0 && originalBoard[E8] == Piece_k) {

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_BLACK_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, WHITE_MASK)) {

                            long newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_BLACK_QUEEN_SIDE | MASK_CASTLING_BLACK_KING_SIDE);
                            newBoard[H8] = 0;
                            newBoard[E8] = 0;
                            newBoard[G8] = Piece_k;
                            newBoard[F8] = Piece_r;
                            newBoard[IDX_BLACK_KING_INDEX] = G8;

                            numMovesFound++;
                            dig(newBoard);
                        }
                    }
                }
            }
        }

    }

    return numMovesFound;
}


int moveLinear(long b[], int fromIdx, const int moveMatrix[], const int moveMatrixLength) {

    moveLinearInvocations++;

    int numMovesFound = 0;
    const int rank = fromIdx >> 3;
    const int file = fromIdx & 7;
    const int p = b[fromIdx];
    const int color = b[IDX_TURN];

    for (int t = 0; t < moveMatrixLength; t += 2) {
        int newFile = file;
        int newRank = rank;
        int dFile = moveMatrix[t];
        int dRank = moveMatrix[t | 1];
        for (int n = 1; (n & 8) == 0; n++) {
            newFile += dFile;
            newRank += dRank;
            if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
                int toIdx = newRank << 3 | newFile;
                int pieceAtIdx = b[toIdx];

                if ((pieceAtIdx & color) == 0) {
                    long newBoard[NUM_BYTES];
                    makeNewBoard(b,newBoard);
                    int breakAfterThis = 0;
                    if (pieceAtIdx != 0) {
                        newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                        breakAfterThis = 1;
                    }
                    newBoard[toIdx] = p;
                    newBoard[fromIdx] = 0;

                    const int checkStatus = calculateCheckStatus(newBoard);
                    newBoard[IDX_CHECK_STATUS] = checkStatus;
                    if (color == WHITE_MASK && (checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
                        if (p == Piece_R) {
                            if (fromIdx == A1) {
                                newBoard[IDX_CASTLING] &= 0xf ^ MASK_CASTLING_WHITE_QUEEN_SIDE;
                            }
                            else if (fromIdx == H1) {
                                newBoard[IDX_CASTLING] &= 0xf ^ MASK_CASTLING_WHITE_KING_SIDE;
                            }
                        }
                        numMovesFound++;
                        dig(newBoard);
                    }
                    else if (color == BLACK_MASK && (checkStatus & MASK_BLACK_KING_CHECKED) == 0) {

                        if (p == Piece_r) {
                            if (fromIdx == A8) {
                                newBoard[IDX_CASTLING] &= 0xf ^ MASK_CASTLING_BLACK_QUEEN_SIDE;
                            }
                            else if (fromIdx == H8) {
                                newBoard[IDX_CASTLING] &= 0xf ^ MASK_CASTLING_BLACK_KING_SIDE;
                            }
                        }
                        numMovesFound++;
                        dig(newBoard);
                    }
                    if (breakAfterThis == 1) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
    }
    return numMovesFound;
}


int makeWhitePromotions(long b[], int from, int to, int moveMask, int castlingMask) {

    int numMovesMade = 0;

    const int lastMove = (MASK_LAST_MOVE_WAS_PROMO | moveMask);

    long promo[NUM_BYTES];
    makeNewBoard(b, promo );

    promo[from] = 0;
    promo[to] = Piece_Q;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    int checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }

    //int[] queen =makeNewBoard(b);
    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_B;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }


    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_R;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }


    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_N;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_WHITE_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }

    return numMovesMade;

}

 int makeBlackPromotions(long b[], int from, int to, int moveMask, int castlingMask) {

    const int lastMove = MASK_LAST_MOVE_WAS_PROMO | moveMask;
    int numMovesMade = 0;
    long promo[NUM_BYTES];
    makeNewBoard(b,promo);

    promo[from] = 0;
    promo[to] = Piece_q;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    int checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }


    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_b;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }

    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_r;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }


    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_n;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    checkStatus = calculateCheckStatus(promo);
    if ((checkStatus & MASK_BLACK_KING_CHECKED) == 0) {
        promo[IDX_CHECK_STATUS] = checkStatus;
        dig(promo);
        numMovesMade++;
    }

    return numMovesMade;

}


void makeNewBoard(long oldBoard[], long newBoard[]) {

    makeNewBoardInvocations++;

    // TODO: Prøv å sette 0 på bare de bytsa som ikke blir aktivt kopiert, alle etter NUM_BYTES_TO_COPY til NUM_BYTES.
    memset(newBoard+NUM_BYTES_TO_COPY, 0, sizeof(long)*(NUM_BYTES-NUM_BYTES_TO_COPY)); // sizeof(int)*(NUM_BYTES-NUM_BYTES_TO_COPY)
    memcpy(newBoard, oldBoard,  sizeof(long)*NUM_BYTES_TO_COPY); // sizeof(int)*NUM_BYTES_TO_COPY

    newBoard[IDX_MOVE_NUM]++;
    newBoard[IDX_TURN] = oldBoard[IDX_TURN] ^ 4095;
    newBoard[IDX_CHECK_STATUS] = 0;


}


boolean isSquaresThreatenedByColor(long board[], int indices[], int color) {
    // only invoked just before castling
    isSquaresThreatenedByColorInvocations ++;

    int offset = color == WHITE_MASK ? 0 : 64;

    for (int i=0;i<2;i++){

        int rank = indices[i] >> 3;
        int file = indices[i] & 7;

        for (int direction = 0; direction < MOVE_MATRIX_LENGTH; direction += 2) {
            int newFile = file;
            int newRank = rank;

            for (int n = 1; (n & 8) == 0; n++) {

                newFile += MOVE_MATRIX[direction];
                newRank += MOVE_MATRIX[direction | 1];

                // Check that 0 > newRank|newFile < 8
                if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {

                    int checkIdx = newRank << 3 | newFile;
                    int checkPiece = board[checkIdx];

                    if (checkPiece == 0) {
                        continue;
                    }

                    int comparePiece = allComparePieces[offset + (direction << 2 | n)];

                    if ((checkPiece & comparePiece) != 0) {
                        return TRUE;
                    }
                    break; // own color piece, stop search line
                }
                else {
                    break; // edge detected, continue to next direction
                }

            }
        }

        for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
            int newFile = file + KNIGHT_MOVE_MATRIX[t];
            int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];

            // Check that 0 > newRank|newFile < 8
            if ( ((newRank | newFile) & 0xFFFFFFF8) == 0 ) {
                int checkIdx = newRank << 3 | newFile;
                int checkPiece = board[checkIdx];
                if (color == BLACK_MASK && checkPiece == Piece_n) {
                    return TRUE;
                }
                if (color == WHITE_MASK && checkPiece == Piece_N) {
                    return TRUE;
                }
            }
        }

    }

    return FALSE;
}


 const int WHITE_ATTACK_MAP_PIECES[] = {
  Pieces_QBK,  Pieces_QB,
  Pieces_QBK,  Pieces_QB,
  Pieces_QBPK, Pieces_QB,
  Pieces_QBPK, Pieces_QB,
  Pieces_QRK,  Pieces_QR,
  Pieces_QRK,  Pieces_QR,
  Pieces_QRK,  Pieces_QR,
  Pieces_QRK,  Pieces_QR,
};
 const int BLACK_ATTACK_MAP_PIECES[] = {
  Pieces_qbpk,Pieces_qb,
  Pieces_qbpk,Pieces_qb,
  Pieces_qbk, Pieces_qb,
  Pieces_qbk, Pieces_qb,
  Pieces_qrk, Pieces_qr,
  Pieces_qrk, Pieces_qr,
  Pieces_qrk, Pieces_qr,
  Pieces_qrk, Pieces_qr,
};

 const int ATTACK_MAP_INDEXES[] = {
   -1,-1,
   -1,1,
    1,-1,
    1,1,
    0,-1,
    -1,0,
    1,0,
    0,1,
};



int calculateWhiteKingCheckStatus( long board[] ){

  // first the one-of threats.
  long idx = board[IDX_WHITE_KING_INDEX];
  long king = 1l << idx;
  printf( "King position:%ld\n%lu\n%lu\n",idx,king,board[IDX_WHITE_KING]);

  if( N_ATTACK_MAPS[idx] & board[IDX_BLACK_KNIGHTS] ){
    printf("KING ATTACKED BY BLACK KNIGHT(S)\n");
    printBitBoard(board);
    printf("N_ATTACK_MAP:\n");
    printLongAsBitBoard(N_ATTACK_MAPS[idx]);
    printf("IDX_BLACK_KNIGHTS:\n");
    printLongAsBitBoard(board[IDX_BLACK_KNIGHTS]);
    printf("IDX_BLACK_PIECES:\n");
    printLongAsBitBoard(board[IDX_BLACK_PIECES]);
    printf("IDX_BLACK_ROOKS:\n");
    printLongAsBitBoard(board[IDX_BLACK_ROOKS]);
    return MASK_WHITE_KING_CHECKED;
  }

  if( BLACK_PAWN_ATTACK_MAPS[idx] & board[IDX_BLACK_PAWNS] ){
    printf("KING ATTACKED BY BLACK PAWN(S)\n");
    return MASK_WHITE_KING_CHECKED;
  }

  if( KING_ATTACK_MAPS[idx] & board[IDX_BLACK_KING] ){
    printf("KING ATTACKED BY BLACK KING\n");
    return MASK_WHITE_KING_CHECKED;
  }


  // now for the vector threats. a piece may be on the map, but can be blocked
  if( QB_ATTACK_MAPS[idx] & (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS])){

    // need to check each direction for a block
    long qb = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_BISHOPS]);
    long allPieces = board[IDX_ALL_PIECES];

    // if the only pieces intersecting the attackmap is a Q or B, then white
    // is in check
    long test = allPieces & QB_ATTACK_MAPS[idx];

    if( __builtin_popcountll (test ) == 1){
      return MASK_WHITE_KING_CHECKED;
    }

    int rank = idx >> 3;
    int file = idx & 7;

    long testMap = king;
    // TODO: regn up hvor langt testen skal gå. ikke noe vits i å teste for hver file&dRank
    // pluss at vi kan bruke bitmap i stedet for
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
    long qr = (board[IDX_BLACK_QUEENS] | board[IDX_BLACK_ROOKS]);
    long allPieces = board[IDX_ALL_PIECES];

    // if the only pieces intersecting the attackmap is a Q or B, then white
    // is in check
    long test = allPieces & QR_ATTACK_MAPS[idx];
    if( __builtin_popcountll (test ) == 1){
      return MASK_WHITE_KING_CHECKED;
    }

    // TODO : also hit if threatmap & piecemap == piecemap

    int rank = idx >> 3;
    int file = idx & 7;

    int maxM = file;
    long testMap = king;

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
    printf( "Doing >> 8 test with map:\n");
    printLongAsBitBoard( testMap );
    for( int m=0;m<maxM;m++){
        testMap >>= 8;
        printLongAsBitBoard( testMap );
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
    printf( "Doing << 8 test with map:\n");
    printLongAsBitBoard( testMap );
    for( int m=0;m<maxM;m++){
        testMap <<= 8;
        printLongAsBitBoard( testMap );
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

int calculateBlackKingCheckStatus( long board[] ){
  return 0;
}

// #rewrite
int calculateCheckStatus( long board[] ){

  calculateCheckStatusInvocations++;

  int result = 0;

  // -------------------------------- WHITE KING -----------------------
  boolean whiteKingIsInCheck = FALSE;
  boolean blackKingIsInCheck = FALSE;
  int idx = board[IDX_WHITE_KING_INDEX];




  if (whiteKingIsInCheck) {
      result = MASK_WHITE_KING_CHECKED;
  }

  if (blackKingIsInCheck) {
      result |= MASK_BLACK_KING_CHECKED;
  }

  return result;
}



void influenceMapForSquare(long b[], int idx) {

    influenceMapForSquareInvocations++;

    long* influenceMap = b+IDX_START_INFLUENCE_MAP;

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

                  long checkPiece = b[checkIdx];

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
            long checkPiece = b[checkIdx];
            if ( (checkPiece & Pieces_Nn) != 0) {
                influenceMap[8 | (t >> 1)] = checkPiece;
            }
        }
    }

}

void clearPieceAtIndex( long board[], int idx ){
    long match = idx << 12;

    for( int t=0;t<32;t++){
        if( (board[t] & 0x3F000) == match ){
            board[t] = 0l;
            break;
        }
    }
    long clear = 1l<<idx;
    for( int t=IDX_WHITE_PAWNS;t<=IDX_BLACK_KING;t++){
      board[t] &= ~clear;
    }

}


void count(long b[]) {

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
 **                                                                                                        **
 **                                                  UTILS                                                 **
 **                                                                                                        **
 ************************************************************************************************************/

void printNumBoard( long board[] ){
    for( int s=0;(s & 64) == 0;s++){
        if( s % 8 == 0 && s != 0 ){
            printf("\n");
        }
        printf( " %ld\t", board[s] );
    }
}

void printDiagram( long board[] ){
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

void printLongAsBitBoard( long bitstream ){
  printf( "  A B C D E F G H");
  char str[65] = "................................................................\0";
  for( int t=0;t<64;t++){
    if( 1 & bitstream ){
        str[t] = '1';
    }
    bitstream >>= 1;
  }

  for( int s=0;(s & 64) == 0;s++){
      if( s % 8 == 0 ){
          printf("\n%d ",8-(s>>3));
      }
      printf("%c ", str[s] );
  }
  printf("\n\n\n");
}

void printBitBoard( long board[] ){
    printf( "\n\n  A B C D E F G H");
    char str[65] = "................................................................\0";
    for( int t=0;t<32;t++){
      long pieceInfo = board[t];
      long piece = pieceInfo & 0xFFF;
      long pos = pieceInfo >> 12;
      fflush(stdout);
      switch( piece ){
        case Piece_P: str[pos] = 'P'; break;
        case Piece_R: str[pos] = 'R'; break;
        case Piece_N: str[pos] = 'N'; break;
        case Piece_B: str[pos] = 'B'; break;
        case Piece_Q: str[pos] = 'Q'; break;
        case Piece_K: str[pos] = 'K'; break;
        case Piece_p: str[pos] = 'p'; break;
        case Piece_r: str[pos] = 'r'; break;
        case Piece_n: str[pos] = 'n'; break;
        case Piece_b: str[pos] = 'b'; break;
        case Piece_q: str[pos] = 'q'; break;
        case Piece_k: str[pos] = 'k'; break;
      }
    }

    for( int s=0;(s & 64) == 0;s++){
        if( s % 8 == 0 ){
            printf("\n%d ",8-(s>>3));
        }
        printf("%c ", str[s] );
    }
    printf("\n");
    printf( "Move num: %ld\n", board[IDX_MOVE_NUM] );
    printf( "Turn : %s\n", board[IDX_TURN] == WHITE_MASK ? "White" : "Black" );



    if ((board[IDX_CHECK_STATUS] & MASK_KING_IS_MATED) ) {
        printf( "There is a MATE\n");
    }
    if( board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED ) {
        printf( "White is in check\n");
    }
    if( board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED ) {
        printf( "Black is in check\n");
    }

    fflush(stdout);

}

void printBoard( long board[] ){

    printf( "\n\n  A B C D E F G H");

    for( int s=0;(s & 64) == 0;s++){
        if( s % 8 == 0 ){
            printf("\n%d ",8-(s>>3));
        }

        switch( board[s] ){
            case Piece_P:printf("P ");break;
            case Piece_R:printf("R ");break;
            case Piece_N:printf("N ");break;
            case Piece_B:printf("B ");break;
            case Piece_Q:printf("Q ");break;
            case Piece_K:printf("K ");break;
            case Piece_p:printf("p ");break;
            case Piece_r:printf("r ");break;
            case Piece_n:printf("n ");break;
            case Piece_b:printf("b ");break;
            case Piece_q:printf("q ");break;
            case Piece_k:printf("k ");break;
            default:
                printf(". ");
        }
    }
    printf("\n");
    printf( "Move num: %ld\n", board[IDX_MOVE_NUM] );
    printf( "Turn : %s\n", board[IDX_TURN] == WHITE_MASK ? "White" : "Black" );



    if ((board[IDX_CHECK_STATUS] & MASK_KING_IS_MATED) ) {
        printf( "There is a MATE\n");
    }
    if( board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED ) {
        printf( "White is in check\n");
    }
    if( board[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED ) {
        printf( "Black is in check\n");
    }

    fflush(stdout);
}

void diagramToBitBoard( long board[], char diagram[] ){

    int len = strlen( diagram );

    memset(board, 0,  sizeof(long)*NUM_BYTES);
    board[IDX_CASTLING] = 0b00001111;
    board[IDX_TURN] = WHITE_MASK;

    int pos = 0;
    int pieceIdx = 0;
    for( int t=0;t<len;t++){
      switch( diagram[t] ){
          // BLACK PIECES
          case ' ':
              break;
          case '.':
              pos++;
              break;
          case 'p':
              board[pieceIdx] = Piece_p;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_PAWNS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'r':
              board[pieceIdx] = Piece_r;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_ROOKS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'b':
              board[pieceIdx] = Piece_b;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_BISHOPS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'n':
              board[pieceIdx] = Piece_n;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_KNIGHTS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'q':
              board[pieceIdx] = Piece_q;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_QUEENS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'k':
              board[pieceIdx] = Piece_k;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_BLACK_KING_INDEX] = pos;
              board[IDX_BLACK_KING] = (1l << pos);
              pieceIdx++;
              pos++;
              break;
              // WHITE PIECES
          case 'P':
              board[pieceIdx] = Piece_P;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_PAWNS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'R':
              board[pieceIdx] = Piece_R;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_ROOKS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'N':
              board[pieceIdx] = Piece_N;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_KNIGHTS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'B':
              board[pieceIdx] = Piece_B;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_BISHOPS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'Q':
              board[pieceIdx] = Piece_Q;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_QUEENS] |= (1l << pos);
              pieceIdx++;
              pos++;
              break;
          case 'K':
              board[pieceIdx] = Piece_K;
              board[pieceIdx] |= (pos << 12l);
              board[IDX_WHITE_KING_INDEX] = pos;
              board[IDX_WHITE_KING] = (1l << pos);
              pieceIdx++;
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

void diagramToByteBoard( long board[], char diagram[] ) {
    int len = strlen( diagram );

    memset(board, 0,  sizeof(long)*NUM_BYTES);
    board[IDX_CASTLING] = 0b00001111;
    board[IDX_TURN] = WHITE_MASK;

    int pos = 0;
    for( int t=0;t<len;t++){
        switch( diagram[t] ){
            case 'p':
                board[pos++] = Piece_p; break;
            case 'r':
                board[pos++] = Piece_r; break;
            case 'b':
                board[pos++] = Piece_b; break;
            case 'n':
                board[pos++] = Piece_n; break;
            case 'q':
                board[pos++] = Piece_q; break;
            case 'k':
                board[pos] = Piece_k;
                board[IDX_BLACK_KING_INDEX] = pos++;
                break;
            case 'P':
                board[pos++] = Piece_P; break;
            case 'R':
                board[pos++] = Piece_R; break;
            case 'N':
                board[pos++] = Piece_N; break;
            case 'B':
                board[pos++] = Piece_B; break;
            case 'Q':
                board[pos++] = Piece_Q; break;
            case 'K':
                board[pos] = Piece_K;
                board[IDX_WHITE_KING_INDEX] = pos++;
                break;
            case '.':
                board[pos++] = 0;
                break;
        }
    }
}

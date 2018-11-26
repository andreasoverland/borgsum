#include <stdio.h>
#include <string.h>
#include <time.h>

#include "constants.h"

typedef enum { FALSE, TRUE } boolean;

void diagramToByteBoard( int board[], char diagram[] );
void printBoard( int board[] );
void printNumBoard( int board[] );
void doSomethingToArray( int board[] );

// Engine methods
void dig(int board[]);
void count(int board[]);
void influenceMapForSquare(int b[], int idx);
int findAllPossibleMoves(int originalBoard[]);
boolean isSquaresThreatenedByColor(int board[], int indices[], int color);
void makeNewBoard( int originalBoard[], int newBoard[] );
int calculateCheckStatus(int board[]);
int makeBlackPromotions(int b[], int from, int to, int moveMask, int castlingMask);
int makeWhitePromotions(int b[], int from, int to, int moveMask, int castlingMask);
int moveLinear(int b[], int fromIdx, const int moveMatrix[], const int moveMatrixLength);


int MAX_LEVEL = 5;
long numMoves[]      = {0,0,0,0,0,0,0,0};
long numCaptures[]   = {0,0,0,0,0,0,0,0};
long numEP[]         = {0,0,0,0,0,0,0,0};
long numCastles[]    = {0,0,0,0,0,0,0,0};
long numPromos[]     = {0,0,0,0,0,0,0,0};
long numChecks[]     = {0,0,0,0,0,0,0,0};
long numCheckmates[] = {0,0,0,0,0,0,0,0};

int main(){

    int board[NUM_BYTES];
    diagramToByteBoard( board, "\
                       r . . . k . . r\
                       p . p p q p b .\
                       b n . . p n p .\
                       . . . P N . . .\
                       . p . . P . . .\
                       . . N . . Q . p\
                       P P P B B P P P\
                       R . . . K . . R");

    printBoard( board );

    struct timespec ts1, ts2;
    clock_gettime(CLOCK_REALTIME, &ts1);

    dig(board);

    clock_gettime(CLOCK_REALTIME, &ts2);
    if (ts2.tv_nsec < ts1.tv_nsec) {
        ts2.tv_nsec += 1000000000;
        ts2.tv_sec--;
    }

    printf("Time spent: %ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);

    int l = 14;

    printf( "\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
            "Depth",
            "Nodes",
            "Caps",
            "E.p.",
            "Castles",
            "Promos",
            "Checks",
            "Mates"
    );

    long total = 0;

    for (int t = 0; t <= MAX_LEVEL; t++) {
        printf("\t%d",t);
        printf("\t%lu",numMoves[t]);
        printf("\t%lu",numCaptures[t]);
        printf("\t%lu",numEP[t]);
        printf("\t%lu",numCastles[t]);
        printf("\t%lu",numPromos[t]);
        printf("\t%lu",numChecks[t]);
        printf("\t%lu\n",numCheckmates[t]);
        total += numMoves[t];
    }

    printf("\nTotal valid moves found : %lu \n" ,total);

    return 0;
}

/************************************************************************************************************
 **                                                                                                        **
 **                                                 ENGINE                                                 **
 **                                                                                                        **
 ************************************************************************************************************/



void dig(int board[]){

    // Uncomment to include testing for mates on the last level
    boolean doCount = TRUE;
    if (board[IDX_MOVE_NUM] < MAX_LEVEL /* || (board[IDX_MOVE_NUM] == MAX_LEVEL && board[IDX_CHECK_STATUS] != 0) */ ) {
        int numMoves = findAllPossibleMoves(board);
        if( numMoves == -1 ){
            doCount = FALSE;
        }
        else {
          const int checkStatus = calculateCheckStatus(board);
          board[IDX_CHECK_STATUS] = checkStatus;
          if (numMoves == 0 && board[IDX_CHECK_STATUS] != 0) {
              board[IDX_CHECK_STATUS] |= MASK_KING_IS_MATED;
          }
        }

    }
    if( doCount ){
      count(board);
    }

}


int findAllPossibleMoves(int originalBoard[]) {

    int numMovesFound = 0;

    // problemet med retroaktive er at vi gjør mange flytt, selv om kongen står i sjakk
    // så vi vil egentlig gjøre denne sjekken vi gjør mange unødvendige flytt.
    // kan det gjøres smartere sjakk-tester
    // det hadde vært kult å vite hvilke brikker som påvirket hvilke vektorer
    // er det kult med bit-boards likevel, for å kjapt sjekke attack?

    for (int fromIdx = 0; (fromIdx & 64) == 0; fromIdx++) {

        const int p = originalBoard[fromIdx];

        if (p == Piece_e) {
            continue;
        }

        const int rank = fromIdx >> 3;
        const int file = fromIdx & 7;

        if (originalBoard[IDX_TURN] == 0) { // turn == white
            /*
             *******************************************************************************************
             ***************************************  WHITE  *******************************************
             *******************************************************************************************
             */


            if (p == Piece_P) {

                int fromIdxm7 = fromIdx - 7;
                int fromIdxm8 = fromIdx - 8;
                int fromIdxm9 = fromIdx - 9;
                int fromIdxm16 = fromIdx - 16;


                // White pawn
                // Check if one and/or two moves ahead can be made
                int m[NUM_BYTES];

                if (originalBoard[fromIdxm8] == 0) {
                    if (rank > 1) {

                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxm8] = p;
                        numMovesFound++;
                        dig(m);

                    }
                    else {
                        // if target = any one rook corner, adjust castling aproprieately
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm8 == A8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_QUEEN_SIDE;
                        }
                        else if (fromIdxm8 == H8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_KING_SIDE;
                        }
                        makeWhitePromotions(originalBoard, fromIdx, (fromIdxm8), MASK_EMPTY, castlingMask);
                    }
                }
                if (rank == 6) {
                    if (originalBoard[fromIdxm16] == 0 && originalBoard[fromIdxm8] == 0) {
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxm16] = p;
                        m[IDX_EP_IDX] = fromIdxm8; // set en passant strike index
                        numMovesFound++;
                        dig(m);
                    }
                }
                if (file > 0 && (originalBoard[fromIdxm9] & BLACK_MASK) != 0) { // strike left

                    if( originalBoard[fromIdxm9] == Piece_k ){
                       return -1; // The previous move was illegal
                    }

                    if (rank > 1) {
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxm9] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                        numMovesFound++;
                        dig(m);
                    }
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm9 == A8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_QUEEN_SIDE;
                        }
                        numMovesFound += makeWhitePromotions(originalBoard, fromIdx, fromIdxm9, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }

                }
                if (file < 7 && (originalBoard[fromIdxm7] & BLACK_MASK) != 0) { // strike right

                    if( originalBoard[fromIdxm7] == Piece_k ){
                       return -1; // The previous move was illegal
                    }

                    if (rank > 1) {

                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxm7] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                        numMovesFound++;
                        dig(m);

                    }
                    else {
                        int castlingMask = originalBoard[IDX_CASTLING];
                        if (fromIdxm7 == H8) {
                            castlingMask &= 0xf ^ MASK_CASTLING_BLACK_KING_SIDE;
                        }
                        numMovesFound += makeWhitePromotions(originalBoard, fromIdx, fromIdxm7, MASK_LAST_MOVE_WAS_CAPTURE, castlingMask);
                    }
                }
                if (originalBoard[IDX_EP_IDX] != 0 && rank == 3) {
                    if (file < 7 && fromIdxm7 == originalBoard[IDX_EP_IDX]) {

                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxm7] = p;
                        m[fromIdx + 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        numMovesFound++;
                        dig(m);

                    }
                    else if (file > 0 && fromIdxm9 == originalBoard[IDX_EP_IDX]) {

                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxm9] = p;
                        m[fromIdx - 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        numMovesFound++;
                        dig(m);

                    }
                }
            }
            else if (p == Piece_R) {
                // White rook
                int numLinearMoves = moveLinear(originalBoard, fromIdx, ROOK_MOVE_MATRIX,ROOK_MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound += numLinearMoves;
            }
            else if (p == Piece_N) {
                // White Knight

                for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + KNIGHT_MOVE_MATRIX[t];
                    int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];

                    if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & WHITE_MASK) == 0) {

                            if( originalBoard[toIdx] == Piece_k ){
                              return -1;
                            }

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;

                            numMovesFound++;
                            dig(newBoard);

                        }
                    }
                }
            }
            else if (p == Piece_B) {
                // White Bishop
                int numLinearMoves = moveLinear(originalBoard, fromIdx, BISHOP_MOVE_MATRIX, BISHOP_MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound += numLinearMoves;

            }
            else if (p == Piece_Q) {
                // White Queen
                int numLinearMoves = moveLinear(originalBoard, fromIdx, MOVE_MATRIX, MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound += numLinearMoves;
            }
            else if (p == Piece_K) {

                // White King Normal Moves

                int numKingMoves = 0;

                for (int t = 0; t < MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + MOVE_MATRIX[t];
                    int newRank = rank + MOVE_MATRIX[t | 1];

                    if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                        int toIdx = newRank << 3 | newFile;

                        if (originalBoard[toIdx] == 0 || (originalBoard[toIdx] & BLACK_MASK) != 0) {

                            if( originalBoard[toIdx] == Piece_k ){
                              return -1;
                            }

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if ((originalBoard[toIdx] & BLACK_MASK) != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;
                            newBoard[IDX_WHITE_KING_INDEX] = toIdx;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_KING_SIDE | MASK_CASTLING_WHITE_QUEEN_SIDE);

                            numKingMoves++;
                            dig(newBoard);
                        }
                    }
                }

                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_WHITE_QUEEN_SIDE) != 0) {
                    // Just do it. Then mark board as did castle queen side, check of squares were threatened on next
                    if (originalBoard[A1] == Piece_R && originalBoard[B1] == 0 && originalBoard[C1] == 0 && originalBoard[D1] == 0 && originalBoard[E1] == Piece_K) {

                        int squares[] = {C1, D1};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, COLOR_BLACK)) {

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
                            newBoard[A1] = 0;
                            newBoard[E1] = 0;
                            newBoard[C1] = Piece_K;
                            newBoard[D1] = Piece_R;
                            newBoard[IDX_WHITE_KING_INDEX] = C1;

                            numKingMoves++;
                            dig(newBoard);
                        }

                    }
                }
                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_WHITE_KING_SIDE) != 0) {

                    if (originalBoard[H1] == Piece_R && originalBoard[G1] == 0 && originalBoard[F1] == 0 && originalBoard[E1] == Piece_K) {

                        int squares[] = {G1, F1};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_WHITE_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, COLOR_BLACK)) {

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE;
                            newBoard[IDX_CASTLING] &= 0xf ^ (MASK_CASTLING_WHITE_QUEEN_SIDE | MASK_CASTLING_WHITE_KING_SIDE);
                            newBoard[H1] = 0;
                            newBoard[E1] = 0;
                            newBoard[G1] = Piece_K;
                            newBoard[F1] = Piece_R;
                            newBoard[IDX_WHITE_KING_INDEX] = G1;

                            numKingMoves++;
                            dig(newBoard);

                        }
                    }
                }

                // Sjekk om vi sto i sjakk, og om det ble noen kongemoves
                numMovesFound+=numKingMoves;

            }
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
                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp8] = p;

                        numMovesFound++;
                        dig(m);

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

                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp16] = p;
                        m[IDX_EP_IDX] = fromIdxp8; // set en passant strike index

                        numMovesFound++;
                        dig(m);


                    }
                }
                if (file < 7 && (originalBoard[fromIdxp9] & WHITE_MASK) != 0) { // strike left as seen from black

                    if( originalBoard[fromIdxp9] == Piece_K ){
                      return -1;
                    }

                    if (rank < 6) {
                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp9] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

                        numMovesFound++;
                        dig(m);

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

                    if( originalBoard[fromIdxp7] == Piece_K ){
                      return -1;
                    }

                    if (rank < 6) {
                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxp7] = p;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;

                        numMovesFound++;
                        dig(m);

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
                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);
                        m[fromIdx] = 0;
                        m[fromIdxp9] = p;
                        m[fromIdx + 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        numMovesFound++;
                        dig(m);

                    }
                    else if (file > 0 && fromIdxp7 == originalBoard[IDX_EP_IDX]) {
                        int m[NUM_BYTES];
                        makeNewBoard(originalBoard,m);

                        m[fromIdx] = 0;
                        m[fromIdxp7] = p;
                        m[fromIdx - 1] = 0;
                        m[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE | MASK_LAST_MOVE_WAS_EP_STRIKE;

                        numMovesFound++;
                        dig(m);
                    }
                }
            }
            else if (p == Piece_r) {
                // Black rook
                int numLinearMoves = moveLinear(originalBoard, fromIdx, ROOK_MOVE_MATRIX, ROOK_MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound+=numLinearMoves;

            }
            else if (p == Piece_n) {
                // Black Knight

                for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
                    int newFile = file + KNIGHT_MOVE_MATRIX[t];
                    int newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];
                    if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & BLACK_MASK) == 0) {

                            if( originalBoard[toIdx] == Piece_K ){
                              return -1;
                            }

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;

                            numMovesFound++;
                            dig(newBoard);

                        }
                    }
                }

            }
            else if (p == Piece_b) {
                // Black Bishop
                int numLinearMoves = moveLinear(originalBoard, fromIdx, BISHOP_MOVE_MATRIX, BISHOP_MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound+=numLinearMoves;
            }
            else if (p == Piece_q) {
                // Black Queen
                int numLinearMoves = moveLinear(originalBoard, fromIdx, MOVE_MATRIX, MOVE_MATRIX_LENGTH);
                if( numLinearMoves == -1 ){
                  return -1;
                }
                numMovesFound+=numLinearMoves;
            }
            else if (p == Piece_k) {
                // Black King

                for (int t = 0; t < 16; t += 2) {
                    int newFile = file + MOVE_MATRIX[t];
                    int newRank = rank + MOVE_MATRIX[t | 1];
                    if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                        int toIdx = newRank << 3 | newFile;

                        if ((originalBoard[toIdx] & BLACK_MASK) == 0) {

                            if( originalBoard[toIdx] == Piece_K ){
                              return -1;
                            }

                            int newBoard[NUM_BYTES];
                            makeNewBoard(originalBoard,newBoard);

                            if (originalBoard[toIdx] != 0) {
                                newBoard[IDX_LAST_MOVE_WAS] = MASK_LAST_MOVE_WAS_CAPTURE;
                            }
                            newBoard[toIdx] = p;
                            newBoard[fromIdx] = 0;
                            newBoard[IDX_BLACK_KING_INDEX] = toIdx;

                            numMovesFound++;
                            dig(newBoard);

                        }
                    }
                }


                if ((originalBoard[IDX_CASTLING] & MASK_CASTLING_BLACK_QUEEN_SIDE) != 0) {

                    if (originalBoard[A8] == Piece_r && originalBoard[B8] == 0 && originalBoard[C8] == 0 && originalBoard[D8] == 0 && originalBoard[E8] == Piece_k) {

                        int squares[] = {C8,D8};

                        if ((originalBoard[IDX_CHECK_STATUS] & MASK_BLACK_KING_CHECKED) == 0 &&
                            !isSquaresThreatenedByColor(originalBoard, squares, COLOR_WHITE)) {

                            int newBoard[NUM_BYTES];
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
                            !isSquaresThreatenedByColor(originalBoard, squares, COLOR_WHITE)) {

                            int newBoard[NUM_BYTES];
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


int moveLinear(int b[], int fromIdx, const int moveMatrix[], const int moveMatrixLength) {

    int numMovesFound = 0;
    const int rank = fromIdx >> 3;
    const int file = fromIdx & 7;
    const int p = b[fromIdx];
    const int color = ((p & BLACK_MASK) != 0) ? BLACK_MASK : WHITE_MASK;


    for (int t = 0; t < moveMatrixLength; t += 2) {
        int newFile = file;
        int newRank = rank;
        for (int n = 1; (n & 8) == 0; n++) {
            newFile += moveMatrix[t];
            newRank += moveMatrix[t | 1];
            if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                int toIdx = newRank << 3 | newFile;
                int pieceAtIdx = b[toIdx];

                if ((pieceAtIdx & color) == 0) {

                    if( pieceAtIdx == (Piece_K | Piece_k) ){
                      return -1;
                    }

                    int newBoard[NUM_BYTES];
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


int makeWhitePromotions(int b[], int from, int to, int moveMask, int castlingMask) {

    int numMovesMade = 0;

    const int lastMove = (MASK_LAST_MOVE_WAS_PROMO | moveMask);

    int promo[NUM_BYTES];
    makeNewBoard(b, promo );

    promo[from] = 0;
    promo[to] = Piece_Q;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    //int[] queen =makeNewBoard(b);
    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_B;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_R;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    makeNewBoard(b, promo );
    promo[from] = 0;
    promo[to] = Piece_N;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    return numMovesMade;
}

 int makeBlackPromotions(int b[], int from, int to, int moveMask, int castlingMask) {

    const int lastMove = MASK_LAST_MOVE_WAS_PROMO | moveMask;
    int numMovesMade = 0;
    int promo[NUM_BYTES];
    makeNewBoard(b,promo);

    promo[from] = 0;
    promo[to] = Piece_q;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;


    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_b;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_r;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;


    makeNewBoard(b, promo);
    promo[from] = 0;
    promo[to] = Piece_n;
    promo[IDX_LAST_MOVE_WAS] = lastMove;
    promo[IDX_CASTLING] = castlingMask;

    dig(promo);
    numMovesMade++;

    return numMovesMade;

}


void makeNewBoard(int oldBoard[], int newBoard[]) {

    // TODO: Prøv å sette 0 på bare de bytsa som ikke blir aktivt kopiert, alle etter NUM_BYTES_TO_COPY til NUM_BYTES.
    memset(newBoard+NUM_BYTES_TO_COPY, 0,  sizeof(int)*(NUM_BYTES-NUM_BYTES_TO_COPY));
    memcpy(newBoard, oldBoard,  sizeof(int)*NUM_BYTES_TO_COPY);

    newBoard[IDX_MOVE_NUM]++;
    newBoard[IDX_TURN] = (oldBoard[IDX_TURN] == 0 ? 1 : 0);
    newBoard[IDX_CHECK_STATUS] = 0;

}


boolean isSquaresThreatenedByColor(int board[], int indices[], int color) {

    int offset = color == COLOR_WHITE ? 0 : 64;

    for (int i=0;i<2;i++){

        const int rank = indices[i] >> 3;
        const int file = indices[i] & 7;

        for (int direction = 0; direction < MOVE_MATRIX_LENGTH; direction += 2) {
            int newFile = file;
            int newRank = rank;

            for (int n = 1; (n & 8) == 0; n++) {

                newFile += MOVE_MATRIX[direction];
                newRank += MOVE_MATRIX[direction | 1];

                // Check that 0 > newRank|newFile < 8
                if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {

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
            if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
                int checkIdx = newRank << 3 | newFile;
                int checkPiece = board[checkIdx];
                if (color == COLOR_BLACK && checkPiece == Piece_n) {
                    return TRUE;
                }
                if (color == COLOR_WHITE && checkPiece == Piece_N) {
                    return TRUE;
                }
            }
        }

    }

    return FALSE;
}



int calculateCheckStatus(int board[]) {

    int result = 0;

    influenceMapForSquare(board, board[IDX_WHITE_KING_INDEX] );

    //long startTime = System.nanoTime();

    boolean whiteKingIsInCheck = FALSE;

    int* influemceMapPtr = board+IDX_START_INFLUENCE_MAP;


    if (((influemceMapPtr[0] | influemceMapPtr[2]) & Pieces_qbpk) != 0) {
        whiteKingIsInCheck = TRUE;
    } else if (((influemceMapPtr[1] | influemceMapPtr[3] | influemceMapPtr[4] | influemceMapPtr[6]) & Pieces_qrk) != 0) {
        whiteKingIsInCheck = TRUE;
    } else if (((influemceMapPtr[5] | influemceMapPtr[7]) & Pieces_qbk) != 0) {
        whiteKingIsInCheck = TRUE;
    }

    for (int t = 8; t < 16 && !whiteKingIsInCheck; t++) {
        if (influemceMapPtr[t] == Piece_n) {
            whiteKingIsInCheck = TRUE;
            break;
        }
    }

    influenceMapForSquare(board, board[IDX_BLACK_KING_INDEX] );

    boolean blackKingIsInCheck = FALSE;

    // go through all and check all 1..6 piece entries

    if (((influemceMapPtr[1] | influemceMapPtr[3] | influemceMapPtr[4] | influemceMapPtr[6]) & Pieces_QRK) != 0) {
        blackKingIsInCheck = TRUE;
    } else if (((influemceMapPtr[5] | influemceMapPtr[7]) & Pieces_QBPK) != 0) {
        blackKingIsInCheck = TRUE;
    } else if (((influemceMapPtr[0] | influemceMapPtr[2]) & Pieces_QBK) != 0) {
        blackKingIsInCheck = TRUE;
    }


    for (int t = 8; t < 16 && !blackKingIsInCheck; t++) {
        if (influemceMapPtr[t] == Piece_N) {
            blackKingIsInCheck = TRUE;
            break;
        }
    }

    if (whiteKingIsInCheck) {
        result = MASK_WHITE_KING_CHECKED;
    }

    if (blackKingIsInCheck) {
        result |= MASK_BLACK_KING_CHECKED;
    }

    return result;
}



void influenceMapForSquare(int b[], int idx) {

    int* influenceMap = b+IDX_START_INFLUENCE_MAP;

    memset(influenceMap, 0, sizeof(int)<<4);

    // the 8 first are the results from linear runs.
    // the next 8 results are possible knight positions

    const int rank = idx >> 3;
    const int file = idx & 7;

    int newFile;
    int newRank;

    int checkIdx;
    int checkPiece;

    int pieces_Nn = Piece_N | Piece_n;
    for (int direction = 0; direction < 16; direction += 2) {

        newFile = file;
        newRank = rank;

        for (int n = 1; (n & 8) == 0; n++) {

            newFile += MOVE_MATRIX[direction];
            newRank += MOVE_MATRIX[direction | 1];

            // Check that 0 >= newRank < 8 && 0 >= newFile < 8
            if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {

                checkIdx = newRank << 3 | newFile;
                checkPiece = b[checkIdx];

                if (checkPiece == 0) {
                    continue;
                }

                if (n > 1) {
                    // Only register pawns and kings if they are 1 square away,
                    // otherwise, just stop scanning that direction
                    // without registering them as threat
                    int pieces_PpKk = Piece_P | Piece_p | Piece_K | Piece_k;
                    if ( (checkPiece & pieces_PpKk) != 0 ) {
                        break;
                    }
                }

                if  ( (checkPiece & pieces_Nn) == 0 ) {
                    influenceMap[direction >> 1] = checkPiece;
                }
                break;

            } else {
                // Continue to next direction
                break;
            }

        }
    }

    for (int t = 0; t < KNIGHT_MOVE_MATRIX_LENGTH; t += 2) {
        newFile = file + KNIGHT_MOVE_MATRIX[t];
        newRank = rank + KNIGHT_MOVE_MATRIX[t | 1];

        // Check that 0 > newRank|newFile < 8
        if ((newRank & 0xFFFFFFF8) == 0 && (newFile & 0xFFFFFFF8) == 0) {
            checkIdx = newRank << 3 | newFile;
            checkPiece = b[checkIdx];
            if ( (checkPiece & pieces_Nn) != 0) {
                influenceMap[8 | (t >> 1)] = checkPiece;
            }
        }
    }
    //long endTime = System.nanoTime();
    //influenceMapsCalcTime += endTime - startTime;

}






void count(int b[]) {

    const int level = b[IDX_MOVE_NUM];
    numMoves[level]++;
    if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_CAPTURE) != 0) {
        numCaptures[level]++;
    }
    if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_EP_STRIKE) != 0) {
        numEP[level]++;
    }
    if ((b[IDX_LAST_MOVE_WAS] & (MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE | MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE)) != 0) {
        numCastles[level]++;
    }
    if ((b[IDX_LAST_MOVE_WAS] & MASK_LAST_MOVE_WAS_PROMO) != 0) {
        numPromos[level]++;
    }
    if ((b[IDX_CHECK_STATUS] & (MASK_WHITE_KING_CHECKED | MASK_BLACK_KING_CHECKED)) != 0) {
        numChecks[level]++;
    }
    if ((b[IDX_CHECK_STATUS] & MASK_KING_IS_MATED) != 0) {
        numCheckmates[level]++;
    }
}





/************************************************************************************************************
 **                                                                                                        **
 **                                                  UTILS                                                 **
 **                                                                                                        **
 ************************************************************************************************************/

void printNumBoard( int board[] ){
    for( int s=0;(s & 64) == 0;s++){
        if( s % 8 == 0 && s != 0 ){
            printf("\n");
        }
        printf( " %u\t", board[s] );
    }
}

void printBoard( int board[] ){
    printf( "  A B C D E F G H");
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
    printf( "Move num: %d", board[IDX_MOVE_NUM] );
    printf( "\n");
}


void diagramToByteBoard( int board[], char diagram[] ) {
    int len = strlen( diagram );

    memset(board, 0,  sizeof(int)*NUM_BYTES);
    board[IDX_CASTLING] = 0b00001111;

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

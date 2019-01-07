

const int Piece_P = 1;
const int Piece_R = 2;
const int Piece_N = 4;
const int Piece_B = 8;
const int Piece_Q = 16;
const int Piece_K = 32;

const int Piece_p = 64;
const int Piece_r = 128;
const int Piece_n = 256;
const int Piece_b = 512;
const int Piece_q = 1024;
const int Piece_k = 2048;

const int WHITE_MASK = 0b000000111111;
const int BLACK_MASK = 0b111111000000;

const int MASK_LAST_MOVE_WAS_CAPTURE             = 0b00001;
const int MASK_LAST_MOVE_WAS_EP_STRIKE           = 0b00010;
const int MASK_LAST_MOVE_WAS_PROMO               = 0b00100;
const int MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE = 0b01000;
const int MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE  = 0b10000;

const int MASK_EMPTY = 0;

const int MASK_WHITE_KING_CHECKED = 0b0000000111111;
const int MASK_BLACK_KING_CHECKED = 0b0111111000000;
const int MASK_KING_IS_MATED      = 0b1000000000000;

// everything below 30 is zero..
// TODO: shift stuff down to 0

const int IDX_MOVE_ID 	     =  0;
const int IDX_PARENT_MOVE_ID =  1;
const int IDX_WHITE_PIECES   =  2;
const int IDX_BLACK_PIECES   =  3;
const int IDX_ALL_PIECES     =  4;

const int IDX_WHITE_PAWNS    =  5;
const int IDX_WHITE_ROOKS    =  6;
const int IDX_WHITE_KNIGHTS  =  7;
const int IDX_WHITE_BISHOPS  =  8;
const int IDX_WHITE_QUEENS   =  9;
const int IDX_WHITE_KING     = 10;

const int IDX_BLACK_PAWNS    = 11;
const int IDX_BLACK_ROOKS    = 12;
const int IDX_BLACK_KNIGHTS  = 13;
const int IDX_BLACK_BISHOPS  = 14;
const int IDX_BLACK_QUEENS   = 15;
const int IDX_BLACK_KING     = 16;

// indexes 47..54 are free

// back to regular indexes

const int IDX_CASTLING = 17;
const int IDX_MOVE_NUM = 18;
const int IDX_TURN = 19;
const int IDX_WHITE_KING_INDEX = 20;
const int IDX_BLACK_KING_INDEX = 21;

const int IDX_CHECK_STATUS = 22;
const int IDX_LAST_MOVE_WAS = 23;
const int IDX_EP_IDX = 24;

const int NUM_BYTES_TO_COPY = 22;
const int NUM_BYTES = 25; // REALLY NUM LONGS 

const int A1 = 7;
const int B1 = 6;
const int C1 = 5;
const int D1 = 4;
const int E1 = 3;
const int F1 = 2;
const int G1 = 1;
const int H1 = 0;

const int A8 = 63;
const int B8 = 62;
const int C8 = 61;
const int D8 = 60;
const int E8 = 59;
const int F8 = 58;
const int G8 = 57;
const int H8 = 56;

const unsigned long A1_MASK = (1L << A1);
const unsigned long B1_MASK = (1L << B1);
const unsigned long C1_MASK = (1L << C1);
const unsigned long D1_MASK = (1L << D1);
const unsigned long E1_MASK = (1L << E1);
const unsigned long F1_MASK = (1L << F1);
const unsigned long G1_MASK = (1L << G1);
const unsigned long H1_MASK = (1L << H1);

const unsigned long A8_MASK = (1L << A8);
const unsigned long B8_MASK = (1L << B8);
const unsigned long C8_MASK = (1L << C8);
const unsigned long D8_MASK = (1L << D8);
const unsigned long E8_MASK = (1L << E8);
const unsigned long F8_MASK = (1L << F8);
const unsigned long G8_MASK = (1L << G8);
const unsigned long H8_MASK = (1L << H8);

const unsigned long B1_C1_D1_MASK = B1_MASK|C1_MASK|D1_MASK;
const unsigned long F1_G1_MASK    = F1_MASK|G1_MASK;

const unsigned long B8_C8_D8_MASK = B8_MASK|C8_MASK|D8_MASK;
const unsigned long F8_G8_MASK    = F8_MASK|G8_MASK;

const unsigned long MASK_CASTLING_WHITE_QUEEN_SIDE = A1_MASK|E1_MASK;
const unsigned long MASK_CASTLING_WHITE_KING_SIDE  = H1_MASK|E1_MASK;
const unsigned long MASK_CASTLING_BLACK_QUEEN_SIDE = A8_MASK|E8_MASK;
const unsigned long MASK_CASTLING_BLACK_KING_SIDE  = H8_MASK|E8_MASK;

// BITMAPS

const unsigned long R8 = 0xFF00000000000000;
const unsigned long R7 = 0x00FF000000000000;
const unsigned long R6 = 0x0000FF0000000000;
const unsigned long R5 = 0x000000FF00000000;
const unsigned long R4 = 0x00000000FF000000;
const unsigned long R3 = 0x0000000000FF0000;
const unsigned long R2 = 0x000000000000FF00;
const unsigned long R1 = 0x00000000000000FF;

const unsigned long R1_R8 = R8|R1;

const unsigned long FA = 0x8080808080808080;
const unsigned long FB = 0x4040404040404040;
const unsigned long FC = 0x2020202020202020;
const unsigned long FD = 0x1010101010101010;
const unsigned long FE = 0x0808080808080808;
const unsigned long FF = 0x0404040404040404;
const unsigned long FG = 0x0202020202020202;
const unsigned long FH = 0x0101010101010101;


const int Piece_e = 0;

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

const int Pieces_Nn = 4 | 256;
const int Pieces_PpKk = 1 | 64 | 32 | 2048;

const int Pieces_qbpk = 1024 | 512 | 2048 | 64 ;
const int Pieces_qbk  = 1024 | 512 | 2048;
const int Pieces_qrk  = 1024 | 128 | 2048;
const int Pieces_qb   = 1024 | 512;
const int Pieces_qr   = 1024 | 128;


const int Pieces_QBPK = 16 | 8 | 1 | 32;
const int Pieces_QBK =  16 | 8 | 32;
const int Pieces_QRK =  16 | 2 | 32;
const int Pieces_QB =   16 | 8;
const int Pieces_QR =   16 | 2;

const int WHITE_MASK = 0b000000111111;
const int BLACK_MASK = 0b111111000000;

const long MASK_CASTLING_WHITE_QUEEN_SIDE = (1l << 7) | (1l << 3);
const long MASK_CASTLING_WHITE_KING_SIDE = 1 | (1l << 3);
const long MASK_CASTLING_BLACK_QUEEN_SIDE = (1l << 56) | (1l << 59);
const long MASK_CASTLING_BLACK_KING_SIDE = (1l << 63) | (1l << 59);

const int MASK_LAST_MOVE_WAS_CAPTURE = 1;
const int MASK_LAST_MOVE_WAS_EP_STRIKE = 2;
const int MASK_LAST_MOVE_WAS_PROMO = 4;
const int MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE = 8;
const int MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE = 16;

const int MASK_EMPTY = 0;

const int MASK_WHITE_KING_CHECKED = 0b0000000111111;
const int MASK_BLACK_KING_CHECKED = 0b0111111000000;
const int MASK_KING_IS_MATED      = 0b1000000000000;

// bitboard spesific indexes
const int IDX_WHITE_PIECES = 32;
const int IDX_BLACK_PIECES = 33;
const int IDX_ALL_PIECES = 34;

const int IDX_WHITE_PAWNS = 35;
const int IDX_WHITE_ROOKS = 36;
const int IDX_WHITE_KNIGHTS = 37;
const int IDX_WHITE_BISHOPS = 38;
const int IDX_WHITE_QUEENS = 39;
const int IDX_WHITE_KING = 40;

const int IDX_BLACK_PAWNS = 41;
const int IDX_BLACK_ROOKS = 42;
const int IDX_BLACK_KNIGHTS = 43;
const int IDX_BLACK_BISHOPS = 44;
const int IDX_BLACK_QUEENS = 45;
const int IDX_BLACK_KING = 46;

// indexes 47..54 are free

// back to regular indexes

const int IDX_CASTLING = 65;
const int IDX_MOVE_NUM = 66;
const int IDX_TURN = 68;
const int IDX_WHITE_KING_INDEX = 69;
const int IDX_BLACK_KING_INDEX = 70;

const int IDX_CHECK_STATUS = 71;
const int IDX_LAST_MOVE_WAS = 72;
const int IDX_EP_IDX = 73;

const int IDX_START_INFLUENCE_MAP = 74;

const int NUM_BYTES_TO_COPY = 71;
const int NUM_BYTES = 91;

const int A1 = 56;
const int B1 = 57;
const int C1 = 58;
const int D1 = 59;
const int E1 = 60;
const int F1 = 61;
const int G1 = 62;
const int H1 = 63;

const int A8 = 0;
const int B8 = 1;
const int C8 = 2;
const int D8 = 3;
const int E8 = 4;
const int F8 = 5;
const int G8 = 6;
const int H8 = 7;

const long A1_MASK = (1L <<(63-A1));
const long B1_MASK = (1L <<(63-B1));
const long C1_MASK = (1L <<(63-C1));
const long D1_MASK = (1L <<(63-D1));
const long E1_MASK = (1L <<(63-E1));
const long F1_MASK = (1L <<(63-F1));
const long G1_MASK = (1L <<(63-G1));
const long H1_MASK = (1L <<(63-H1));

const long B1_C1_D1_MASK = B1_MASK|C1_MASK|D1_MASK;
const long F1_G1_MASK = F1_MASK|G1_MASK;

const long B8_C8_D8_MASK = (1L << (63-B8)) | (1L << (63-C8)) | (1L << (63-D8));
const long F8_G8_MASK = (1L << (63-F8)) | (1L << (63-G8));


const int MOVE_MATRIX_LENGTH = 16;
const int MOVE_MATRIX[] = {
    -1, -1, 0, -1, 1, -1,
    -1,  0,        1,  0,
    -1,  1, 0,  1, 1,  1
};

const int KNIGHT_MOVE_MATRIX_LENGTH = 16;
const int KNIGHT_MOVE_MATRIX[] = {
    -1, -2, -1, 2, -2, -1, -2, 1,
     1, -2,  1, 2,  2, -1,  2, 1
};
const int KNIGHT_MOVE_MATRIX_2[] = {
  -1, -2, -1, 2, 1, -2, 1, 2, -1
};

const int BISHOP_MOVE_MATRIX_LENGTH = 8;
const int BISHOP_MOVE_MATRIX[] = {
    -1, -1, 1, -1,
    -1,  1, 1,  1
};

const int ROOK_MOVE_MATRIX_LENGTH = 8;
const int ROOK_MOVE_MATRIX[] = {
    -1, 0, 0, -1,
     1, 0, 0,  1
};

const int a = Piece_q | Piece_b;
const int b = Piece_q | Piece_b | Piece_k | Piece_p;
const int c = Piece_q | Piece_b | Piece_k;
const int d = Piece_q | Piece_r;
const int e = Piece_q | Piece_r | Piece_k;

const int A = Piece_Q | Piece_B;
const int B = Piece_Q | Piece_B | Piece_K | Piece_P;
const int C = Piece_Q | Piece_B | Piece_K;
const int D = Piece_Q | Piece_R;
const int E = Piece_Q | Piece_R | Piece_K;

int allComparePieces[8*16] = {
    0, C, A, A, A, A, A, A,
    0, E, D, D, D, D, D, D,
    0, C, A, A, A, A, A, A,
    0, E, D, D, D, D, D, D,
    0, E, D, D, D, D, D, D,
    0, B, A, A, A, A, A, A,
    0, E, D, D, D, D, D, D,
    0, B, A, A, A, A, A, A,
    0, b, a, a, a, a, a, a,
    0, e, d, d, d, d, d, d,
    0, b, a, a, a, a, a, a,
    0, e, d, d, d, d, d, d,
    0, e, d, d, d, d, d, d,
    0, c, a, a, a, a, a, a,
    0, e, d, d, d, d, d, d,
    0, c, a, a, a, a, a, a,
};

// BITMAPS

const long R8 = 0xFF00000000000000;
const long R7 = 0x00FF000000000000;
const long R6 = 0x0000FF0000000000;
const long R5 = 0x000000FF00000000;
const long R4 = 0x00000000FF000000;
const long R3 = 0x0000000000FF0000;
const long R2 = 0x000000000000FF00;
const long R1 = 0x00000000000000FF;

const long R1_R8 = R8|R1;

const long FA = 0x8080808080808080;
const long FB = 0x4040404040404040;
const long FC = 0x2020202020202020;
const long FD = 0x1010101010101010;
const long FE = 0x0808080808080808;
const long FF = 0x0404040404040404;
const long FG = 0x0202020202020202;
const long FH = 0x0101010101010101;

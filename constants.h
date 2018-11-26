
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

const int Pieces_QBPK = 16 | 8 | 1 | 32;
const int Pieces_QBK =  16 | 8 | 32;
const int Pieces_QRK =  16 | 2 | 32;

const int WHITE_MASK = 0b000000111111;
const int BLACK_MASK = 0b111111000000;

const int MASK_CASTLING_WHITE_QUEEN_SIDE = 1;
const int MASK_CASTLING_WHITE_KING_SIDE = 2;
const int MASK_CASTLING_BLACK_QUEEN_SIDE = 4;
const int MASK_CASTLING_BLACK_KING_SIDE = 8;

const int MASK_LAST_MOVE_WAS_CAPTURE = 1;
const int MASK_LAST_MOVE_WAS_EP_STRIKE = 2;
const int MASK_LAST_MOVE_WAS_PROMO = 4;
const int MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE = 8;
const int MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE = 16;

const int MASK_EMPTY = 0;

const int MASK_WHITE_KING_CHECKED = 1;
const int MASK_BLACK_KING_CHECKED = 2;
const int MASK_KING_IS_MATED = 4;

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


const int MOVE_MATRIX_LENGTH = 16;
const int MOVE_MATRIX[] = {
    -1, -1, 0, -1, 1, -1,
    -1,  0,        1,  0,
    -1,  1, 0,  1, 1,  1
};


const int KNIGHT_MOVE_MATRIX_LENGTH = 16;
const int KNIGHT_MOVE_MATRIX[] = {
    -1, -2, -1, +2, -2, -1, -2, +1,
    1, -2, 1, +2, 2, -1, 2, +1
};

const int BISHOP_MOVE_MATRIX_LENGTH = 8;
const int BISHOP_MOVE_MATRIX[] = {
    -1, -1, 1, -1,
    -1,  1, 1,  1
};

const int ROOK_MOVE_MATRIX_LENGTH = 8;
const int ROOK_MOVE_MATRIX[] = {
    -1, 0, 0, -1,
    1, 0, 0, 1
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

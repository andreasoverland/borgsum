
static const int Piece_e = 0;

static const int Piece_P = 1;
static const int Piece_R = 2;
static const int Piece_N = 4;
static const int Piece_B = 8;
static const int Piece_Q = 16;
static const int Piece_K = 32;

static const int Piece_p = 64;
static const int Piece_r = 128;
static const int Piece_n = 256;
static const int Piece_b = 512;
static const int Piece_q = 1024;
static const int Piece_k = 2048;

static const int Pieces_Nn = 4 | 256;
static const int Pieces_PpKk = 1 | 64 | 32 | 2048;

static const int Pieces_qbpk = 1024 | 512 | 2048 | 64 ;
static const int Pieces_qbk  = 1024 | 512 | 2048;
static const int Pieces_qrk  = 1024 | 128 | 2048;
static const int Pieces_qb   = 1024 | 512;
static const int Pieces_qr   = 1024 | 128;


static const int Pieces_QBPK = 16 | 8 | 1 | 32;
static const int Pieces_QBK =  16 | 8 | 32;
static const int Pieces_QRK =  16 | 2 | 32;
static const int Pieces_QB =   16 | 8;
static const int Pieces_QR =   16 | 2;

static const int WHITE_MASK = 0b000000111111;
static const int BLACK_MASK = 0b111111000000;

static const int MASK_CASTLING_WHITE_QUEEN_SIDE = 1;
static const int MASK_CASTLING_WHITE_KING_SIDE = 2;
static const int MASK_CASTLING_BLACK_QUEEN_SIDE = 4;
static const int MASK_CASTLING_BLACK_KING_SIDE = 8;

static const int MASK_LAST_MOVE_WAS_CAPTURE = 1;
static const int MASK_LAST_MOVE_WAS_EP_STRIKE = 2;
static const int MASK_LAST_MOVE_WAS_PROMO = 4;
static const int MASK_LAST_MOVE_WAS_CASTLING_QUEEN_SIDE = 8;
static const int MASK_LAST_MOVE_WAS_CASTLING_KING_SIDE = 16;

static const int MASK_EMPTY = 0;

static const int MASK_WHITE_KING_CHECKED = 0b000000111111;
static const int MASK_BLACK_KING_CHECKED = 0b111111000000;
static const int MASK_KING_IS_MATED = 0b1000000000000;

static const int IDX_CASTLING = 65;
static const int IDX_MOVE_NUM = 66;
static const int IDX_TURN = 68;
static const int IDX_WHITE_KING_INDEX = 69;
static const int IDX_BLACK_KING_INDEX = 70;

static const int IDX_CHECK_STATUS = 71;
static const int IDX_LAST_MOVE_WAS = 72;
static const int IDX_EP_IDX = 73;

static const int IDX_START_INFLUENCE_MAP = 74;

static const int NUM_BYTES_TO_COPY = 71;
static const int NUM_BYTES = 91;

static const int A1 = 56;
static const int B1 = 57;
static const int C1 = 58;
static const int D1 = 59;
static const int E1 = 60;
static const int F1 = 61;
static const int G1 = 62;
static const int H1 = 63;

static const int A8 = 0;
static const int B8 = 1;
static const int C8 = 2;
static const int D8 = 3;
static const int E8 = 4;
static const int F8 = 5;
static const int G8 = 6;
static const int H8 = 7;


static const int MOVE_MATRIX_LENGTH = 16;
static const int MOVE_MATRIX[] = {
    -1, -1, 0, -1, 1, -1,
    -1,  0,        1,  0,
    -1,  1, 0,  1, 1,  1
};

static const int KNIGHT_MOVE_MATRIX_LENGTH = 16;
static const int KNIGHT_MOVE_MATRIX[] = {
    -1, -2, -1, 2, -2, -1, -2, 1,
     1, -2,  1, 2,  2, -1,  2, 1
};
static const int KNIGHT_MOVE_MATRIX_2[] = {
  -1, -2, -1, 2, 1, -2, 1, 2, -1
};

static const int BISHOP_MOVE_MATRIX_LENGTH = 8;
static const int BISHOP_MOVE_MATRIX[] = {
    -1, -1, 1, -1,
    -1,  1, 1,  1
};

static const int ROOK_MOVE_MATRIX_LENGTH = 8;
static const int ROOK_MOVE_MATRIX[] = {
    -1, 0, 0, -1,
     1, 0, 0,  1
};

static const int a = Piece_q | Piece_b;
static const int b = Piece_q | Piece_b | Piece_k | Piece_p;
static const int c = Piece_q | Piece_b | Piece_k;
static const int d = Piece_q | Piece_r;
static const int e = Piece_q | Piece_r | Piece_k;

static const int A = Piece_Q | Piece_B;
static const int B = Piece_Q | Piece_B | Piece_K | Piece_P;
static const int C = Piece_Q | Piece_B | Piece_K;
static const int D = Piece_Q | Piece_R;
static const int E = Piece_Q | Piece_R | Piece_K;

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

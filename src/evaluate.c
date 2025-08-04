/*
// evaluate.c
#include "stdio.h"
#include "defs.h"
#include <string.h>
#include <stdbool.h>

// Define piece types for array indices
const int PAWN = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK = 4;
const int QUEEN = 5;
const int KING = 6;
const int PieceToType[13] = {0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
// Phased piece values (centipawns, inspired by Stockfish/PeSTO)
const int mg_value[7] = { 0, 82, 337, 365, 477, 1025, 0 };
const int eg_value[7] = { 0, 94, 281, 297, 512, 936, 0 };
// Phase weights
const int KnightPhase = 1;
const int BishopPhase = 1;
const int RookPhase = 2;
const int QueenPhase = 4;
const int TotalPhase = 24; // Hardcoded to fix constant expression issue
// PeSTO-inspired PSTs (midgame and endgame separate for all pieces)
const int mg_pawn_table[64] = {
  0,   0,   0,   0,   0,   0,  0,   0,
98, 134,  61,  95,  68, 126, 34, -11,
 -6,   7,  26,  31,  65,  56, 25, -20,
-14,  13,   6,  21,  23,  12, 17, -23,
-27,  -2,  -5,  12,  17,   6, 10, -25,
-26,  -4,  -4, -10,   3,   3, 33, -12,
-35,  -1, -20, -23, -15,  24, 38, -22,
  0,   0,   0,   0,   0,   0,  0,   0,
};
const int eg_pawn_table[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
178, 173, 158, 134, 147, 132, 165, 187,
94, 100,  85,  67,  56,  53,  82,  84,
32,  24,  13,   5,  -2,   4,  17,  17,
13,   9,  -3,  -7,  -7,  -8,   3,  -1,
 4,   7,  -6,   1,   0,  -5,  -1,  -8,
13,   8,   8,  10,  13,   0,   2,  -7,
  0,   0,   0,   0,   0,   0,   0,   0,
};
const int mg_knight_table[64] = {
-167, -89, -34, -49,  61, -97, -15, -107,
 -73, -41,  72,  36,  23,  62,   7,  -17,
 -47,  60,  37,  65,  84, 129,  73,   44,
  -9,  17,  19,  53,  37,  69,  18,   22,
 -13,   4,  16,  13,  28,  19,  21,   -8,
 -23,  -9,  12,  10,  19,  17,  25,  -16,
 -29, -53, -12,  -3,  -1,  18, -14,  -19,
-105, -21, -58, -33, -17, -28, -19,  -23,
};
const int eg_knight_table[64] = {
-58, -38, -13, -28, -31, -27, -63, -99,
-25,  -8, -25,  -2,  -9, -25, -24, -52,
-24, -20,  10,   9,  -1,  -9, -19, -41,
-17,   3,  22,  22,  22,  11,   8, -18,
-18,  -6,  16,  25,  16,  17,   4, -18,
-23,  -3,  -1,  15,  10,  -3, -20, -22,
-42, -20, -10,  -5,  -2, -20, -23, -44,
-29, -51, -23, -15, -22, -18, -50, -64,
};
const int mg_bishop_table[64] = {
 -29,   4, -82, -37, -25, -42,   7,  -8,
 -26,  16, -18, -13,  30,  59,  18, -47,
 -16,  37,  43,  40,  35,  50,  37,  -2,
  -4,   5,  19,  50,  37,  37,   7,  -2,
  -6,  13,  13,  26,  19,  15,  12,  -5,
 -10,  27,  24,  27,  32,  25,  13,  -8,
 -20,   0,  -7,  -4,  -6, -10,  -3, -26,
 -48,  -4, -42, -51, -83, -88, -30, -43,
};
const int eg_bishop_table[64] = {
 -14, -21, -11,  -8, -7,  -9, -17, -24,
  -8,  -4,   7, -12, -3, -13,  -4, -14,
   2,  -8,   0,  -1,  -2,   6,   0,   4,
  -3,   9,  12,   9,  14,  10,   3,   2,
  -5,   1,  -6,  11,   3,   3,   2,  -1,
  -3,  -9,  -3,  -9,   2,  -1,   2,  -2,
  -5,  -8, -11,  -4,  -5,  -6,  -8, -11,
 -6, -23, -15, -27, -9, -23, -21, -25,
};
const int mg_rook_table[64] = {
  32,  42,  32,  51, 63, 9,  31,  43,
  27,  32,  58,  62, 80, 67,  26,  44,
 -5,  19,  26,  36, 17, 45,  61,  16,
-24, -11,   7,  26, 24, 35,  -8, -20,
-36, -18, -5,  19, 26, 3,  30,  -9,
-25, -30, -10,  -7, -1, 11, -16, -19,
-26, -31,   0,   7, -5, -9, -26, -46,
 -4, -10, -18, 106, 87, -1, -24,   0,
};
const int eg_rook_table[64] = {
 13, 10, 18, 15, 12,  12,   8,   5,
 11, 13, 13, 11, -3,   3,   8,   3,
  7,  7,  7,  5,  4,  -3,  -5,  -3,
  4,  3, 13,  1,  2,   1,  -1,   2,
  3,  5,  8,  4, -5,  -6,  -8, -11,
 -4,  0, -5, -1, -7, -12,  -8, -16,
 -6, -6,  0,  2, -9,  -9, -11,  -3,
 -9,  2,  3, -1, -5, -13,   4, -20,
};
const int mg_queen_table[64] = {
 -28,   0, 29, 12, 59, 44, 43, 45,
 -24, -39,  -5,   1, -16, 57, 28, 54,
 -13, -17,   7,   8, 29, 56, 47, 57,
 -27, -27, -16, -16,  -1, 17,  -2,   1,
 -9, -26,  -9, -10,  -2,  -4,   3,  -3,
 -14,   2, -11,  -2,  -5,   2, 14,   5,
 -35,  -8, 11,   2,   8, 15,  -3,   1,
 -1, -18,  -9, 10, -15, -25, -31, -50,
};
const int eg_queen_table[64] = {
 -9, 22, 22, 27, 27, 19, 10, 20,
 -17, 20, 32, 41, 58, 25, 30, 0,
 -20, 6, 9, 49, 47, 35, 19, 9,
3, 22, 24, 45, 57, 40, 57, 36,
 -18, 28, 19, 47, 31, 34, 39, 23,
 -16, -27, 15, 6, 9, 17, 10, 5,
 -22, -23, -30, -16, -16, -23, -36, -32,
 -33, -28, -22, -43,  -5, -32, -20, -41,
};
const int mg_king_table[64] = {
 -65, 23, 16, -15, -56, -34, 2, 13,
 29, -1, -20, -7, -8, -4, -38, -29,
 -9, 24, 2, -16, -20, 6, 22, -22,
 -17, -20, -12, -27, -30, -25, -14, -36,
 -49, -1, -27, -39, -46, -44, -33, -51,
 -14, -14, -22, -46, -44, -30, -15, -27,
1, 7, -8, -64, -43, -16, 9, 8,
 -15, 36, 12, -54, 8, -28, 24, 14,
};
const int eg_king_table[64] = {
 -74, -35, -18, -18, -11, 15, 4, -17,
 -12, 17, 14, 17, 17, 38, 23, 11,
10, 17, 23, 15, 20, 45, 44, 13,
 -8, 22, 24, 27, 26, 33, 26, 3,
 -18, -4, 21, 24, 27, 23, 9, -11,
 -19, -3, 11, 21, 23, 16, 7, -9,
 -27, -20, 4, -1, 4, -1, 0, -7,
 -41, -7, -8, -25, -8, -25, -26, -36,
};
// Pawn structure bonuses/penalties (tuned values)
const int PawnIsolatedMg = -5;
const int PawnIsolatedEg = -10;
const int PawnDoubledMg = -10;
const int PawnDoubledEg = -20;
const int PawnBackwardMg = -8;
const int PawnBackwardEg = -14;
const int PawnPassedMg[8] = {0, 3, 6, 15, 40, 80, 150, 0};
const int PawnPassedEg[8] = {0, 10, 20, 50, 100, 150, 200, 0};
// Piece bonuses
const int BishopPairMg = 30;
const int BishopPairEg = 50;
const int RookOpenFileMg = 43;
const int RookOpenFileEg = 21;
const int RookSemiOpenFileMg = 19;
const int RookSemiOpenFileEg = 10;
const int QueenOpenFileMg = 5;
const int QueenOpenFileEg = 3;
const int KnightOutpostBonusMg = 22;
const int KnightOutpostBonusEg = 10;
const int RookSeventhMg = 20;
const int RookSeventhEg = 10;
// Mobility bonuses (from Stockfish-like tuned)
const int KnightMobMg[9] = { -62, -53, -12, -4, 3, 13, 22, 28, 33 };
const int KnightMobEg[9] = { -81, -56, -31, -16, 5, 11, 17, 20, 25 };
const int BishopMobMg[14] = { -48, -40, -16, -5, 1, 8, 16, 23, 29, 36, 41, 46, 50, 54 };
const int BishopMobEg[14] = { -59, -41, -25, -11, 6, 12, 21, 32, 40, 48, 53, 58, 63, 68 };
const int RookMobMg[15] = { -60, -37, -26, -16, -5, 4, 9, 14, 21, 25, 35, 41, 46, 50, 54 };
const int RookMobEg[15] = { -78, -45, -27, -15, -5, 3, 7, 15, 21, 24, 27, 32, 36, 41, 46 };
const int QueenMobMg[28] = { -30, -24, -18, -14, -8, -4, 1, 4, 9, 12, 15, 20, 23, 26, 30, 32, 35, 38, 41, 43, 45, 48, 50, 52, 53, 55, 56, 58 };
const int QueenMobEg[28] = { -40, -30, -20, -15, -10, -5, -1, 2, 5, 9, 13, 16, 19, 23, 26, 29, 32, 35, 38, 41, 44, 47, 50, 52, 55, 57, 59, 61 };
// King safety
const int ShelterBonusMg = 17; // Per pawn in shelter
const int AttackWeight[7] = { 0, 0, 45, 45, 40, 55, 0 }; // EMPTY, P, N, B, R, Q, K
const int KingDangerScale = 2;
// Tropism (distance to opp king)
const int TropismMg[7] = { 0, 0, 2, 1, 3, 4, 0 }; // EMPTY, P, N, B, R, Q, K
const int TropismEg[7] = { 0, 0, 1, 1, 2, 3, 0 };
// Direction deltas for attacks/mobility
const int knight_delta[8] = { -21, -19, -12, -8, 8, 12, 19, 21 };
const int bishop_delta[4] = { -11, -9, 9, 11 };
const int rook_delta[4] = { -10, -1, 1, 10 };
const int queen_delta[8] = { -11, -10, -9, -1, 1, 9, 10, 11 };
const int king_delta[8] = { -11, -10, -9, -1, 1, 9, 10, 11 };
// Helper for LSB/MSB using PopBit
int GetLsb(U64 bb) {
    if (bb == 0) return -1;
    return PopBit(&bb);
}
int GetMsb(U64 bb) {
    int msb = -1;
    while (bb) {
        msb = PopBit(&bb);
    }
    return msb;
}
// Chebyshev distance
int ChebDist(int sq1, int sq2) {
    int fd = abs(FilesBrd[sq1] - FilesBrd[sq2]);
    int rd = abs(RanksBrd[sq1] - RanksBrd[sq2]);
    return fd > rd ? fd : rd;
}
// Pawn attacks bitboard
U64 PawnAttacks(int side, int sq) {
    U64 attacks = 0ULL;
    int dir = (side == WHITE) ? 10 : -10;
    if (FilesBrd[sq] != FILE_A) SETBIT(attacks, SQ64(sq + dir - 1));
    if (FilesBrd[sq] != FILE_H) SETBIT(attacks, SQ64(sq + dir + 1));
    return attacks;
}
// Piece attacks bitboard (simple, without occupancy for non-sliders, but for safety, use sliding with occupancy simulation)
U64 PieceAttacks(int pce, int sq, const S_BOARD *pos) {
    int type = PieceToType[pce];
    U64 attacks = 0ULL;
    int deltas[8];
    int num_d = 0;
    bool is_slider = (type == BISHOP || type == ROOK || type == QUEEN);
    int own_side = PieceCol[pce];
    int opp_side = (own_side == WHITE) ? BLACK : WHITE;

    if (type == KNIGHT) {
        num_d = 8;
        memcpy(deltas, knight_delta, sizeof(int)*8);
    } else if (type == BISHOP) {
        num_d = 4;
        memcpy(deltas, bishop_delta, sizeof(int)*4);
    } else if (type == ROOK) {
        num_d = 4;
        memcpy(deltas, rook_delta, sizeof(int)*4);
    } else if (type == QUEEN) {
        num_d = 8;
        memcpy(deltas, queen_delta, sizeof(int)*8);
    } else if (type == KING) {
        num_d = 8;
        memcpy(deltas, king_delta, sizeof(int)*8);
    } else if (type == PAWN) {
        return PawnAttacks(own_side == WHITE ? WHITE : BLACK, sq);
    } else return 0ULL;

    for (int i = 0; i < num_d; ++i) {
        int tsq = sq + deltas[i];
        if (!SqOnBoard(tsq)) continue;

        if (!is_slider) {
            // Leapers: Single step, include only empty or enemy
            int target_pce = pos->pieces[tsq];
            if (target_pce == EMPTY || PieceCol[target_pce] == opp_side) {
                SETBIT(attacks, SQ64(tsq));
            }
        } else {
            // Sliders: Ray-trace, include empty/enemy, stop (exclude) on own
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == opp_side) {
                        SETBIT(attacks, SQ64(tsq));
                    }
                    break;  // Always break on occupied (own or enemy)
                }
                SETBIT(attacks, SQ64(tsq));
                tsq += deltas[i];
            }
        }
    }
    return attacks;
}
int MaterialDraw(const S_BOARD *pos) {
    ASSERT(CheckBoard(pos));
    // (same as original)
    return FALSE; // Placeholder, use original code
}
int EvalPosition(const S_BOARD *pos) {
    ASSERT(CheckBoard(pos));
    if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }
    // Game phase
    int game_phase = 0;
    game_phase += (pos->pceNum[wN] + pos->pceNum[bN]) * KnightPhase;
    game_phase += (pos->pceNum[wB] + pos->pceNum[bB]) * BishopPhase;
    game_phase += (pos->pceNum[wR] + pos->pceNum[bR]) * RookPhase;
    game_phase += (pos->pceNum[wQ] + pos->pceNum[bQ]) * QueenPhase;
    game_phase = (game_phase * 256 + (TotalPhase / 2)) / TotalPhase;
    if (game_phase > 256) game_phase = 256;
    int mg_score = 0, eg_score = 0;
    int pce, pceNum, sq, sq64, file, rank, mob, dist;
    int wk_sq = pos->KingSq[WHITE];
    int bk_sq = pos->KingSq[BLACK];
    // Material
    mg_score += (pos->pceNum[wP] - pos->pceNum[bP]) * mg_value[PAWN];
    eg_score += (pos->pceNum[wP] - pos->pceNum[bP]) * eg_value[PAWN];
    mg_score += (pos->pceNum[wN] - pos->pceNum[bN]) * mg_value[KNIGHT];
    eg_score += (pos->pceNum[wN] - pos->pceNum[bN]) * eg_value[KNIGHT];
    mg_score += (pos->pceNum[wB] - pos->pceNum[bB]) * mg_value[BISHOP];
    eg_score += (pos->pceNum[wB] - pos->pceNum[bB]) * eg_value[BISHOP];
    mg_score += (pos->pceNum[wR] - pos->pceNum[bR]) * mg_value[ROOK];
    eg_score += (pos->pceNum[wR] - pos->pceNum[bR]) * eg_value[ROOK];
    mg_score += (pos->pceNum[wQ] - pos->pceNum[bQ]) * mg_value[QUEEN];
    eg_score += (pos->pceNum[wQ] - pos->pceNum[bQ]) * eg_value[QUEEN];
    // Pawn structure
    int white_isolated = 0, white_doubled = 0, white_backward = 0;
    for (file = FILE_A; file <= FILE_H; ++file) {
        U64 wp_file = pos->pawns[WHITE] & FileBBMask[file];
        int count = CNT(wp_file);
        if (count > 1) white_doubled += (count - 1);
        if (count == 0) continue;
        if ((IsolatedMask[GetLsb(wp_file)] & pos->pawns[WHITE]) == 0) white_isolated += count;
        if (count) {
            int pawn_sq = SQ120(GetLsb(wp_file));
            int advance_sq = pawn_sq + 10;
            if (RanksBrd[pawn_sq] < RANK_5 && SqAttacked(advance_sq, BLACK, pos) && !SqAttacked(advance_sq, WHITE, pos)) white_backward++;
        }
    }
    mg_score += white_doubled * PawnDoubledMg + white_backward * PawnBackwardMg + white_isolated * PawnIsolatedMg;
    eg_score += white_doubled * PawnDoubledEg + white_backward * PawnBackwardEg + white_isolated * PawnIsolatedEg;
    int black_isolated = 0, black_doubled = 0, black_backward = 0;
    for (file = FILE_A; file <= FILE_H; ++file) {
        U64 bp_file = pos->pawns[BLACK] & FileBBMask[file];
        int count = CNT(bp_file);
        if (count > 1) black_doubled += (count - 1);
        if (count == 0) continue;
        if ((IsolatedMask[GetMsb(bp_file)] & pos->pawns[BLACK]) == 0) black_isolated += count;
        if (count) {
            int pawn_sq = SQ120(GetMsb(bp_file));
            int advance_sq = pawn_sq - 10;
            if (RanksBrd[pawn_sq] > RANK_4 && SqAttacked(advance_sq, WHITE, pos) && !SqAttacked(advance_sq, BLACK, pos)) black_backward++;
        }
    }
    mg_score -= black_doubled * PawnDoubledMg + black_backward * PawnBackwardMg + black_isolated * PawnIsolatedMg;
    eg_score -= black_doubled * PawnDoubledEg + black_backward * PawnBackwardEg + black_isolated * PawnIsolatedEg;
    // Pawns and passed
    pce = wP;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = RanksBrd[sq];
        mg_score += mg_pawn_table[sq64];
        eg_score += eg_pawn_table[sq64];
        if ((WhitePassedMask[sq64] & pos->pawns[BLACK]) == 0) {
            mg_score += PawnPassedMg[rank];
            eg_score += PawnPassedEg[rank];
        }
    }
    pce = bP;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = 7 - RanksBrd[sq];
        mg_score -= mg_pawn_table[MIRROR64(sq64)];
        eg_score -= eg_pawn_table[MIRROR64(sq64)];
        if ((BlackPassedMask[sq64] & pos->pawns[WHITE]) == 0) {
            mg_score -= PawnPassedMg[rank];
            eg_score -= PawnPassedEg[rank];
        }
    }
    // Pieces
    pce = wN;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = RanksBrd[sq];
        mg_score += mg_knight_table[sq64];
        eg_score += eg_knight_table[sq64];
        mob = 0;
        for (int i = 0; i < 8; ++i) {
            int tsq = sq + knight_delta[i];
            if (SqOnBoard(tsq) && (pos->pieces[tsq] == EMPTY || PieceCol[pos->pieces[tsq]] == BLACK)) mob++;
        }
        mg_score += KnightMobMg[mob];
        eg_score += KnightMobEg[mob];
        if (rank >= RANK_4 && rank <= RANK_6 && (PawnAttacks(WHITE, sq) & pos->pawns[WHITE]) && !(PawnAttacks(BLACK, sq) & pos->pawns[BLACK])) {
            mg_score += KnightOutpostBonusMg;
            eg_score += KnightOutpostBonusEg;
        }
        dist = ChebDist(sq, bk_sq);
        mg_score += TropismMg[KNIGHT] * (7 - dist);
        eg_score += TropismEg[KNIGHT] * (7 - dist);
    }
    pce = bN;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = RanksBrd[sq];
        mg_score -= mg_knight_table[MIRROR64(sq64)];
        eg_score -= eg_knight_table[MIRROR64(sq64)];
        mob = 0;
        for (int i = 0; i < 8; ++i) {
            int tsq = sq + knight_delta[i];
            if (SqOnBoard(tsq) && (pos->pieces[tsq] == EMPTY || PieceCol[pos->pieces[tsq]] == WHITE)) mob++;
        }
        mg_score -= KnightMobMg[mob];
        eg_score -= KnightMobEg[mob];
        if (rank <= RANK_5 && rank >= RANK_3 && (PawnAttacks(BLACK, sq) & pos->pawns[BLACK]) && !(PawnAttacks(WHITE, sq) & pos->pawns[WHITE])) {
            mg_score -= KnightOutpostBonusMg;
            eg_score -= KnightOutpostBonusEg;
        }
        dist = ChebDist(sq, wk_sq);
        mg_score -= TropismMg[KNIGHT] * (7 - dist);
        eg_score -= TropismEg[KNIGHT] * (7 - dist);
    }
    pce = wB;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        mg_score += mg_bishop_table[sq64];
        eg_score += eg_bishop_table[sq64];
        mob = 0;
        for (int i = 0; i < 4; ++i) {
            int tsq = sq + bishop_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == BLACK) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += bishop_delta[i];
            }
        }
        mg_score += BishopMobMg[mob];
        eg_score += BishopMobEg[mob];
        dist = ChebDist(sq, bk_sq);
        mg_score += TropismMg[BISHOP] * (7 - dist);
        eg_score += TropismEg[BISHOP] * (7 - dist);
    }
    pce = bB;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        mg_score -= mg_bishop_table[MIRROR64(sq64)];
        eg_score -= eg_bishop_table[MIRROR64(sq64)];
        mob = 0;
        for (int i = 0; i < 4; ++i) {
            int tsq = sq + bishop_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == WHITE) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += bishop_delta[i];
            }
        }
        mg_score -= BishopMobMg[mob];
        eg_score -= BishopMobEg[mob];
        dist = ChebDist(sq, wk_sq);
        mg_score -= TropismMg[BISHOP] * (7 - dist);
        eg_score -= TropismEg[BISHOP] * (7 - dist);
    }
    pce = wR;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = RanksBrd[sq];
        mg_score += mg_rook_table[sq64];
        eg_score += eg_rook_table[sq64];
        mob = 0;
        for (int i = 0; i < 4; ++i) {
            int tsq = sq + rook_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == BLACK) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += rook_delta[i];
            }
        }
        mg_score += RookMobMg[mob];
        eg_score += RookMobEg[mob];
        file = FilesBrd[sq];
        if (!(pos->pawns[BOTH] & FileBBMask[file])) {
            mg_score += RookOpenFileMg;
            eg_score += RookOpenFileEg;
        } else if (!(pos->pawns[WHITE] & FileBBMask[file])) {
            mg_score += RookSemiOpenFileMg;
            eg_score += RookSemiOpenFileEg;
        }
        if (rank == RANK_7) {
            mg_score += RookSeventhMg;
            eg_score += RookSeventhEg;
        }
        dist = ChebDist(sq, bk_sq);
        mg_score += TropismMg[ROOK] * (7 - dist);
        eg_score += TropismEg[ROOK] * (7 - dist);
    }
    pce = bR;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        rank = RanksBrd[sq];
        mg_score -= mg_rook_table[MIRROR64(sq64)];
        eg_score -= eg_rook_table[MIRROR64(sq64)];
        mob = 0;
        for (int i = 0; i < 4; ++i) {
            int tsq = sq + rook_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == WHITE) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += rook_delta[i];
            }
        }
        mg_score -= RookMobMg[mob];
        eg_score -= RookMobEg[mob];
        file = FilesBrd[sq];
        if (!(pos->pawns[BOTH] & FileBBMask[file])) {
            mg_score -= RookOpenFileMg;
            eg_score -= RookOpenFileEg;
        } else if (!(pos->pawns[BLACK] & FileBBMask[file])) {
            mg_score -= RookSemiOpenFileMg;
            eg_score -= RookSemiOpenFileEg;
        }
        if (rank == RANK_2) {
            mg_score -= RookSeventhMg;
            eg_score -= RookSeventhEg;
        }
        dist = ChebDist(sq, wk_sq);
        mg_score -= TropismMg[ROOK] * (7 - dist);
        eg_score -= TropismEg[ROOK] * (7 - dist);
    }
    pce = wQ;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        mg_score += mg_queen_table[sq64];
        eg_score += eg_queen_table[sq64];
        mob = 0;
        for (int i = 0; i < 8; ++i) {
            int tsq = sq + queen_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == BLACK) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += queen_delta[i];
            }
        }
        mg_score += QueenMobMg[mob];
        eg_score += QueenMobEg[mob];
        file = FilesBrd[sq];
        if (!(pos->pawns[BOTH] & FileBBMask[file])) {
            mg_score += QueenOpenFileMg;
            eg_score += QueenOpenFileEg;
        }
        dist = ChebDist(sq, bk_sq);
        mg_score += TropismMg[QUEEN] * (7 - dist);
        eg_score += TropismEg[QUEEN] * (7 - dist);
    }
    pce = bQ;
    for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        sq64 = SQ64(sq);
        mg_score -= mg_queen_table[MIRROR64(sq64)];
        eg_score -= eg_queen_table[MIRROR64(sq64)];
        mob = 0;
        for (int i = 0; i < 8; ++i) {
            int tsq = sq + queen_delta[i];
            while (SqOnBoard(tsq)) {
                int target_pce = pos->pieces[tsq];
                if (target_pce != EMPTY) {
                    if (PieceCol[target_pce] == WHITE) {
                        mob++;
                    }
                    break;
                }
                mob++;
                tsq += queen_delta[i];
            }
        }
        mg_score -= QueenMobMg[mob];
        eg_score -= QueenMobEg[mob];
        file = FilesBrd[sq];
        if (!(pos->pawns[BOTH] & FileBBMask[file])) {
            mg_score -= QueenOpenFileMg;
            eg_score -= QueenOpenFileEg;
        }
        dist = ChebDist(sq, wk_sq);
        mg_score -= TropismMg[QUEEN] * (7 - dist);
        eg_score -= TropismEg[QUEEN] * (7 - dist);
    }
    // King
    sq = wk_sq;
    sq64 = SQ64(sq);
    mg_score += mg_king_table[sq64];
    eg_score += eg_king_table[sq64];
    sq = bk_sq;
    sq64 = SQ64(sq);
    mg_score -= mg_king_table[MIRROR64(sq64)];
    eg_score -= eg_king_table[MIRROR64(sq64)];
    // Bishop pair
    if (pos->pceNum[wB] >= 2) {
        mg_score += BishopPairMg;
        eg_score += BishopPairEg;
    }
    if (pos->pceNum[bB] >= 2) {
        mg_score -= BishopPairMg;
        eg_score -= BishopPairEg;
    }
    // King safety
    U64 w_king_zone = 0ULL;
    for (int i = 0; i < 8; ++i) {
        int tsq = wk_sq + king_delta[i];
        if (SqOnBoard(tsq)) SETBIT(w_king_zone, SQ64(tsq));
    }
    SETBIT(w_king_zone, SQ64(wk_sq));
    U64 b_king_zone = 0ULL;
    for (int i = 0; i < 8; ++i) {
        int tsq = bk_sq + king_delta[i];
        if (SqOnBoard(tsq)) SETBIT(b_king_zone, SQ64(tsq));
    }
    SETBIT(b_king_zone, SQ64(bk_sq));
    int w_danger = 0;
    int b_danger = 0;
    // Black attacks on white king zone
    for (int black_pce = bP; black_pce <= bK; black_pce++) {
        for (pceNum = 0; pceNum < pos->pceNum[black_pce]; ++pceNum) {
            sq = pos->pList[black_pce][pceNum];
            U64 attacks = PieceAttacks(black_pce, sq, pos);
            if (attacks & w_king_zone) w_danger += AttackWeight[PieceToType[black_pce]];
        }
    }
    // White attacks on black king zone
    for (int white_pce = wP; white_pce <= wK; white_pce++) {
        for (pceNum = 0; pceNum < pos->pceNum[white_pce]; ++pceNum) {
            sq = pos->pList[white_pce][pceNum];
            U64 attacks = PieceAttacks(white_pce, sq, pos);
            if (attacks & b_king_zone) b_danger += AttackWeight[PieceToType[white_pce]];
        }
    }
    mg_score -= w_danger * KingDangerScale;
    mg_score += b_danger * KingDangerScale;
    // Tapered
    int score = ((mg_score * game_phase) + (eg_score * (256 - game_phase))) / 256;
    return (pos->side == WHITE) ? score : -score;
}
*/
// evaluate.c

#include "stdio.h"
#include "defs.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

const int KingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};
// sjeng 11.2
//8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41 
int MaterialDraw(const S_BOARD *pos) {

	ASSERT(CheckBoard(pos));
	
    if (!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]) {
	  if (!pos->pceNum[bB] && !pos->pceNum[wB]) {
	      if (pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3) {  return TRUE; }
	  } else if (!pos->pceNum[wN] && !pos->pceNum[bN]) {
	     if (abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2) { return TRUE; }
	  } else if ((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])) {
	    if ((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] == 1 && !pos->pceNum[bN]))  { return TRUE; }
	  }
	} else if (!pos->pceNum[wQ] && !pos->pceNum[bQ]) {
        if (pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1) {
            if ((pos->pceNum[wN] + pos->pceNum[wB]) < 2 && (pos->pceNum[bN] + pos->pceNum[bB]) < 2)	{ return TRUE; }
        } else if (pos->pceNum[wR] == 1 && !pos->pceNum[bR]) {
            if ((pos->pceNum[wN] + pos->pceNum[wB] == 0) && (((pos->pceNum[bN] + pos->pceNum[bB]) == 1) || ((pos->pceNum[bN] + pos->pceNum[bB]) == 2))) { return TRUE; }
        } else if (pos->pceNum[bR] == 1 && !pos->pceNum[wR]) {
            if ((pos->pceNum[bN] + pos->pceNum[bB] == 0) && (((pos->pceNum[wN] + pos->pceNum[wB]) == 1) || ((pos->pceNum[wN] + pos->pceNum[wB]) == 2))) { return TRUE; }
        }
    }
  return FALSE;
}

#define ENDGAME_MAT (1 * PieceVal[wR] + 2 * PieceVal[wN] + 2 * PieceVal[wP] + PieceVal[wK])

int EvalPosition(const S_BOARD *pos) {

	ASSERT(CheckBoard(pos));

	int pce;
	int pceNum;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];
	
	if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE) {
		return 0;
	}
	
	pce = wP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		score += PawnTable[SQ64(sq)];	
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			//printf("wP Iso:%s\n",PrSq(sq));
			score += PawnIsolated;
		}
		
		if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			//printf("wP Passed:%s\n",PrSq(sq));
			score += PawnPassed[RanksBrd[sq]];
		}
		
	}	

	pce = bP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
		score -= PawnTable[MIRROR64(SQ64(sq))];	
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			//printf("bP Iso:%s\n",PrSq(sq));
			score -= PawnIsolated;
		}
		
		if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			//printf("bP Passed:%s\n",PrSq(sq));
			score -= PawnPassed[7 - RanksBrd[sq]];
		}
	}	
	
	pce = wN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		score += KnightTable[SQ64(sq)];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
		score -= KnightTable[MIRROR64(SQ64(sq))];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		score += BishopTable[SQ64(sq)];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
		score -= BishopTable[MIRROR64(SQ64(sq))];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		score += RookTable[SQ64(sq)];
		
		ASSERT(FileRankValid(FilesBrd[sq]));
		
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += RookOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += RookSemiOpenFile;
		}
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
		score -= RookTable[MIRROR64(SQ64(sq))];
		ASSERT(FileRankValid(FilesBrd[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score -= RookOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
			score -= RookSemiOpenFile;
		}
	}	
	
	pce = wQ;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		ASSERT(FileRankValid(FilesBrd[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += QueenOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += QueenSemiOpenFile;
		}
	}	

	pce = bQ;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		ASSERT(FileRankValid(FilesBrd[sq]));
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score -= QueenOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
			score -= QueenSemiOpenFile;
		}
	}	
	//8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62 
	pce = wK;
	sq = pos->pList[pce][0];
	ASSERT(SqOnBoard(sq));
	ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
	
	if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
	}
	
	pce = bK;
	sq = pos->pList[pce][0];
	ASSERT(SqOnBoard(sq));
	ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
	
	if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
		score -= KingE[MIRROR64(SQ64(sq))];
	} else {
		score -= KingO[MIRROR64(SQ64(sq))];
	}
	
	if(pos->pceNum[wB] >= 2) score += BishopPair;
	if(pos->pceNum[bB] >= 2) score -= BishopPair;
	
	if(pos->side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}
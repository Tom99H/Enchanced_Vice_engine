// movegen.c
#include "stdio.h"
#include "defs.h"

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)

// Magic constants and tables
const U64 rook_magics[64] = {
0x8a80104000800020ULL, 0x140002000100040ULL, 0x2801880a00204100ULL, 0x100081e800a00120ULL, 0x8010002000808004ULL, 0x1004001002401002ULL, 0x4c002150008a0008ULL, 0x8420800200010500ULL,
0x28802000100ULL, 0x400080100020ULL, 0x100042001000ULL, 0x200021000400ULL, 0x3002001000ULL, 0x8004000802ULL, 0x404200080000ULL, 0x840102000080ULL,
0x4010200010004000ULL, 0x200014002000ULL, 0x8010002000808000ULL, 0x800100800400ULL, 0x200002002000ULL, 0x100002000800ULL, 0x8480008002000100ULL, 0x2000840000802000ULL,
0x30020008004000ULL, 0x10040008000200ULL, 0x120002000400ULL, 0x100042001000ULL, 0x400082000400ULL, 0x1000300000400ULL, 0x46500080400ULL, 0x210001000200ULL,
0x8010200010201000ULL, 0x400080200080ULL, 0x800100200000ULL, 0x800100020000ULL, 0x8002000400ULL, 0x8002000400ULL, 0x8001000200ULL, 0x8000480000001ULL,
0x1000a0004000200ULL, 0x4000802000801000ULL, 0x100042001000ULL, 0x2000081000ULL, 0x400002000800ULL, 0x80050000ULL, 0x8080008002000100ULL, 0x200040400020100ULL,
0x10001000804000ULL, 0x200010000400ULL, 0x100008008100ULL, 0x400004000200ULL, 0x8410004000200ULL, 0x100004000200ULL, 0x100040080001000ULL, 0x1020000a8201ULL,
0x4002800040005ULL, 0x30010000200ULL, 0x100042001000ULL, 0x400082000400ULL, 0x1000300000400ULL, 0x46500080400ULL, 0x210001000200ULL, 0x102000010008042ULL
};

const U64 bishop_magics[64] = {
0x400408244040ULL, 0x2004208a004208ULL, 0x401100d0084000ULL, 0x212400640400ULL, 0x212400640400ULL, 0x2c0050410400ULL, 0x6a8846202008ULL, 0x405040444040ULL,
0x2104004200ULL, 0x40190041008020ULL, 0x288060402ULL, 0x2104000802ULL, 0x120802000ULL, 0x10004201090ULL, 0x78802841007800ULL, 0x4100404001402ULL,
0x120014400320ULL, 0x100042401000ULL, 0x4100410400400ULL, 0x248001000ULL, 0x2804200ULL, 0x690002000ULL, 0x28004008010ULL, 0x2800020082ULL,
0x60000204008010ULL, 0x400400ULL, 0x200010916400ULL, 0x801030400ULL, 0x800000a8ULL, 0x40000060ULL, 0x400200ULL, 0x100042000ULL,
0x10002040080ULL, 0x4000821000200ULL, 0x800820800ULL, 0x801080c000ULL, 0x800080080080ULL, 0x804000ULL, 0x8201100200ULL, 0x1008080100ULL,
0x40002041090ULL, 0x2802084100200ULL, 0x801000600ULL, 0x800164000ULL, 0x100300a0d0ULL, 0x110400a000ULL, 0x4020820a420ULL, 0x885000804080ULL,
0x100042000ULL, 0x40008038ULL, 0x2000810a0ULL, 0x140008100ULL, 0x4002801350ULL, 0x1c004001005ULL, 0x2214100a0ULL, 0x2886400ULL,
0x1000420080ULL, 0x400618ULL, 0x1000420080ULL, 0x400618ULL, 0x1000420080ULL, 0x400618ULL, 0x1000420080ULL, 0x400618ULL
};

const U64 rook_masks[64] = {
0x1010101010101feULL, 0x2020202020202fdULL, 0x4040404040404fbULL, 0x8080808080808f7ULL, 0x10101010101010efULL, 0x20202020202020dfULL, 0x40404040404040bfULL, 0x808080808080807fULL,
0x10101010101fe01ULL, 0x20202020202fd02ULL, 0x40404040404fb04ULL, 0x80808080808f708ULL, 0x101010101010ef10ULL, 0x202020202020df20ULL, 0x404040404040bf40ULL, 0x8080808080807f80ULL,
0x101010101fe0101ULL, 0x202020202fd0202ULL, 0x404040404fb0404ULL, 0x808080808f70808ULL, 0x1010101010ef1010ULL, 0x2020202020df2020ULL, 0x4040404040bf4040ULL, 0x80808080807f8080ULL,
0x1010101fe010101ULL, 0x2020202fd020202ULL, 0x4040404fb040404ULL, 0x8080808f7080808ULL, 0x10101010ef101010ULL, 0x20202020df202020ULL, 0x40404040bf404040ULL, 0x808080807f808080ULL,
0x10101fe01010101ULL, 0x20202fd02020202ULL, 0x40404fb04040404ULL, 0x80808f708080808ULL, 0x101010ef10101010ULL, 0x202020df20202020ULL, 0x404040bf40404040ULL, 0x8080807f80808080ULL,
0x101fe0101010101ULL, 0x202fd0202020202ULL, 0x404fb0404040404ULL, 0x808f70808080808ULL, 0x1010ef1010101010ULL, 0x2020df2020202020ULL, 0x4040bf4040404040ULL, 0x80807f8080808080ULL,
0x1fe010101010101ULL, 0x2fd020202020202ULL, 0x4fb040404040404ULL, 0x8f7080808080808ULL, 0x10ef101010101010ULL, 0x20df2020202020ULL, 0x40bf404040404040ULL, 0x807f808080808080ULL,
0xfe01010101010101ULL, 0xfd02020202020202ULL, 0xfb04040404040404ULL, 0xf708080808080808ULL, 0xef10101010101010ULL, 0xdf20202020202020ULL, 0xbf40404040404040ULL, 0x7f80808080808080ULL 
};

const U64 bishop_masks[64] = {
0x8040201008040200ULL, 0x80402010080500ULL, 0x804020110a00ULL, 0x8041221400ULL, 0x182442800ULL, 0x10204885000ULL, 0x102040810a000ULL, 0x102040810204000ULL,
0x4020100804020002ULL, 0x8040201008050005ULL, 0x804020110a000aULL, 0x804122140014ULL, 0x18244280028ULL, 0x1020488500050ULL, 0x102040810a000a0ULL, 0x204081020400040ULL,
0x2010080402000204ULL, 0x4020100805000508ULL, 0x804020110a000a11ULL, 0x80412214001422ULL, 0x1824428002844ULL, 0x102048850005088ULL, 0x2040810a000a010ULL, 0x408102040004020ULL,
0x1008040200020408ULL, 0x2010080500050810ULL, 0x4020110a000a1120ULL, 0x8041221400142241ULL, 0x182442800284482ULL, 0x204885000508804ULL, 0x40810a000a01008ULL, 0x810204000402010ULL,
0x804020002040810ULL, 0x1008050005081020ULL, 0x20110a000a112040ULL, 0x4122140014224180ULL, 0x8244280028448201ULL, 0x488500050880402ULL, 0x810a000a0100804ULL, 0x1020400040201008ULL,
0x402000204081020ULL, 0x805000508102040ULL, 0x110a000a11204080ULL, 0x2214001422418000ULL, 0x4428002844820100ULL, 0x8850005088040201ULL, 0x10a000a010080402ULL, 0x2040004020100804ULL,
0x200020408102040ULL, 0x500050810204080ULL, 0xa000a1120408000ULL, 0x1400142241800000ULL, 0x2800284482010000ULL, 0x5000508804020100ULL, 0xa000a01008040201ULL, 0x4000402010080402ULL,
0x2040810204080ULL, 0x5081020408000ULL, 0xa112040800000ULL, 0x14224180000000ULL, 0x28448201000000ULL, 0x50880402010000ULL, 0xa0100804020100ULL, 0x40201008040201ULL 
};

const int rook_shifts[64] = {
52, 53, 53, 53, 53, 53, 53, 52, 
53, 54, 54, 54, 54, 54, 54, 53, 
53, 54, 54, 54, 54, 54, 54, 53, 
53, 54, 54, 54, 54, 54, 54, 53, 
53, 54, 54, 54, 54, 54, 54, 53, 
53, 54, 54, 54, 54, 54, 54, 53, 
53, 54, 54, 54, 54, 54, 54, 53, 
52, 53, 53, 53, 53, 53, 53, 52 
};

const int bishop_shifts[64] = {
58, 59, 59, 59, 59, 59, 59, 58, 
59, 59, 59, 59, 59, 59, 59, 59, 
59, 59, 57, 57, 57, 57, 59, 59, 
59, 59, 55, 55, 55, 55, 59, 59, 
59, 59, 55, 55, 55, 55, 59, 59, 
59, 59, 57, 57, 57, 57, 59, 59, 
59, 59, 59, 59, 59, 59, 59, 59, 
58, 59, 59, 59, 59, 59, 59, 58 
};

U64 rook_attacks[64][4096];
U64 bishop_attacks[64][512];

// Function to compute real rook attacks for init
U64 compute_rook_attacks(int sq, U64 occ) {
    U64 attacks = 0ULL;
    int r = sq / 8, f = sq % 8;
    int dr[] = {1, -1, 0, 0};
    int df[] = {0, 0, 1, -1};
    for (int d = 0; d < 4; d++) {
        int rr = r + dr[d], ff = f + df[d];
        while (rr >= 0 && rr < 8 && ff >= 0 && ff < 8) {
            U64 pos = (1ULL << (rr * 8 + ff));
            attacks |= pos;
            if (occ & pos) break;
            rr += dr[d];
            ff += df[d];
        }
    }
    return attacks;
}

// Similar for bishop
U64 compute_bishop_attacks(int sq, U64 occ) {
    U64 attacks = 0ULL;
    int r = sq / 8, f = sq % 8;
    int dr[] = {1, 1, -1, -1};
    int df[] = {1, -1, 1, -1};
    for (int d = 0; d < 4; d++) {
        int rr = r + dr[d], ff = f + df[d];
        while (rr >= 0 && rr < 8 && ff >= 0 && ff < 8) {
            U64 pos = (1ULL << (rr * 8 + ff));
            attacks |= pos;
            if (occ & pos) break;
            rr += dr[d];
            ff += df[d];
        }
    }
    return attacks;
}

// Init function for sliders (call in AllInit())
void InitSliders() {
    // Rook attacks
    for (int sq = 0; sq < 64; sq++) {
        U64 mask = rook_masks[sq];
        int bits = 64 - rook_shifts[sq];
        U64 num_occ = 1ULL << bits;
        for (U64 i = 0; i < num_occ; i++) {
            U64 occ = 0ULL;
            U64 temp = mask;
            int bit = 0;
            do {
                if (temp) {
                    int ls1b = POP(&temp);
                    if (i & (1ULL << bit)) {
                        occ |= (1ULL << ls1b);
                    }
                    bit++;
                }
            } while (temp);
            U64 key = (occ * rook_magics[sq]) >> rook_shifts[sq];
            rook_attacks[sq][key] = compute_rook_attacks(sq, occ);
        }
    }

    // Bishop attacks
    for (int sq = 0; sq < 64; sq++) {
        U64 mask = bishop_masks[sq];
        int bits = 64 - bishop_shifts[sq];
        U64 num_occ = 1ULL << bits;
        for (U64 i = 0; i < num_occ; i++) {
            U64 occ = 0ULL;
            U64 temp = mask;
            int bit = 0;
            do {
                if (temp) {
                    int ls1b = POP(&temp);
                    if (i & (1ULL << bit)) {
                        occ |= (1ULL << ls1b);
                    }
                    bit++;
                }
            } while (temp);
            U64 key = (occ * bishop_magics[sq]) >> bishop_shifts[sq];
            bishop_attacks[sq][key] = compute_bishop_attacks(sq, occ);
        }
    }
}

U64 get_rook_attacks(int sq, U64 occ) {
    occ &= rook_masks[sq];
    occ *= rook_magics[sq];
    occ >>= rook_shifts[sq];
    return rook_attacks[sq][occ];
}

U64 get_bishop_attacks(int sq, U64 occ) {
    occ &= bishop_masks[sq];
    occ *= bishop_magics[sq];
    occ >>= bishop_shifts[sq];
    return bishop_attacks[sq][occ];
}

const int LoopSlidePce[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};
const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};
const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };
const int PceDir[13][8] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,  -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,  1, 10, 0, 0, 0, 0 },
    { -1, -10,  1, 10, -9, -11, 11, 9 },
    { -1, -10,  1, 10, -9, -11, 11, 9 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,  -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,  1, 10, 0, 0, 0, 0 },
    { -1, -10,  1, 10, -9, -11, 11, 9 },
    { -1, -10,  1, 10, -9, -11, 11, 9 }
};
const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};
/*
PV Move
Cap -> MvvLVA
Killers
HistoryScore
*/
const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];
void InitMvvLva() {
    int Attacker;
    int Victim;
    for(Attacker = wP; Attacker <= bK; ++Attacker) {
        for(Victim = wP; Victim <= bK; ++Victim) {
            MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - ( VictimScore[Attacker] / 100);
        }
    }
}
int MoveExists(S_BOARD *pos, const int move) {
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        TakeMove(pos);
        if(list->moves[MoveNum].move == move) {
            return TRUE;
        }
    }
    return FALSE;
}
static void AddQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    ASSERT(pos->ply >=0 && pos->ply < MAXDEPTH);
    list->moves[list->count].move = move;
    if(pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }
    list->count++;
}
static void AddCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(PieceValid(CAPTURED(move)));
    ASSERT(CheckBoard(pos));
    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
    list->count++;
}
static void AddEnPassantMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    ASSERT((RanksBrd[TOSQ(move)]==RANK_6 && pos->side == WHITE) || (RanksBrd[TOSQ(move)]==RANK_3 && pos->side == BLACK));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}
static void AddWhitePawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {
    ASSERT(PieceValidEmpty(cap));
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if(RanksBrd[from] == RANK_7) {
        AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
    } else {
        AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}


// In GenerateAllMoves and GenerateAllCaps, ensure occ is declared as U64 occ = pos->occupied[BOTH]; at the top of each function if not already present.
static void AddWhitePawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if(RanksBrd[from] == RANK_7) {
        AddQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wN,0), list);
    } else {
        AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}
static void AddBlackPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {
    ASSERT(PieceValidEmpty(cap));
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if(RanksBrd[from] == RANK_2) {
        AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
    } else {
        AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}
static void AddBlackPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if(RanksBrd[from] == RANK_2) {
        AddQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bN,0), list);
    } else {
        AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}
void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {
    ASSERT(CheckBoard(pos));
    list->count = 0;
    int pce = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;
    U64 occ = pos->occupied[BOTH];
    if(side == WHITE) {
        for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
            sq = pos->pList[wP][pceNum];
            ASSERT(SqOnBoard(sq));
            if(pos->pieces[sq + 10] == EMPTY) {
                AddWhitePawnMove(pos, sq, sq+10, list);
                if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
            }
            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq + 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }
        if(pos->castlePerm & WKCA) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos) ) {
                    AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        if(pos->castlePerm & WQCA) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos) ) {
                    AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    } else {
        for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
            sq = pos->pList[bP][pceNum];
            ASSERT(SqOnBoard(sq));
            if(pos->pieces[sq - 10] == EMPTY) {
                AddBlackPawnMove(pos, sq, sq-10, list);
                if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
            }
            if(pos->enPas != NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq - 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }
        // castling
        if(pos->castlePerm &  BKCA) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos) ) {
                    AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        if(pos->castlePerm &  BQCA) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos) ) {
                    AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }
    /* Loop for slide pieces */
    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePce[pceIndex++];
    while( pce != 0) {
        ASSERT(PieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));
            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;
                while(!SQOFFBOARD(t_sq)) {
                    // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                    if(pos->pieces[t_sq] != EMPTY) {
                        if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }
        pce = LoopSlidePce[pceIndex++];
    }
    /* Loop for non slide */
    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePce[pceIndex++];
    while( pce != 0) {
        ASSERT(PieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));
            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;
                if(SQOFFBOARD(t_sq)) {
                    continue;
                }
                // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                if(pos->pieces[t_sq] != EMPTY) {
                    if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
                AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
            }
        }
        pce = LoopNonSlidePce[pceIndex++];
    }
    ASSERT(MoveListOk(list,pos));
}
void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {
    ASSERT(CheckBoard(pos));
    list->count = 0;
    int pce = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;
    U64 occ = pos->occupied[BOTH];
    if(side == WHITE) {
        for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
            sq = pos->pList[wP][pceNum];
            ASSERT(SqOnBoard(sq));
            if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
            }
            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq + 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }
    } else {
        for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
            sq = pos->pList[bP][pceNum];
            ASSERT(SqOnBoard(sq));
            if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
            }
            if(pos->enPas != NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq - 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }
    }
    /* Loop for slide pieces */
    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePce[pceIndex++];
    while( pce != 0) {
        ASSERT(PieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));
            U64 attacks = 0ULL;
            if (PieceBishopQueen[pce]) {
                attacks = get_bishop_attacks(sq, occ);
            }
            if (PieceRookQueen[pce]) {
                attacks |= get_rook_attacks(sq, occ);
            }
            while (attacks) {
                t_sq = POP(&attacks);
                if(pos->pieces[t_sq] != EMPTY) {
                    if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                }
            }
        }
        pce = LoopSlidePce[pceIndex++];
    }
    /* Loop for non slide */
    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePce[pceIndex++];
    while( pce != 0) {
        ASSERT(PieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));
            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;
                if(SQOFFBOARD(t_sq)) {
                    continue;
                }
                // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                if(pos->pieces[t_sq] != EMPTY) {
                    if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
            }
        }
        pce = LoopNonSlidePce[pceIndex++];
    }
    ASSERT(MoveListOk(list,pos));
}
/*
// Init function for sliders (call in AllInit())
void InitSliders() {
    for (int sq = 0; sq < 64; sq++) {
        U64 mask = rook_masks[sq];
        int bits = 64 - rook_shifts[sq];
        U64 num_occ = 1ULL << bits;
        for (U64 i = 0; i < num_occ; i++) {
            U64 occ = 0ULL;
            U64 temp = mask;
            int bit = 0;
            while (temp) {
                int ls1b = POP(&temp);
                if (i & (1ULL << bit)) {
                    occ |= (1ULL << ls1b);
                }
                bit++;
            }
            U64 key = (occ * rook_magics[sq]) >> rook_shifts[sq];
            rook_attacks[sq][key] = compute_rook_attacks(sq, occ);
        }
    }

    for (int sq = 0; sq < 64; sq++) {
        U64 mask = bishop_masks[sq];
        int bits = 64 - bishop_shifts[sq];
        U64 num_occ = 1ULL << bits;
        for (U64 i = 0; i < num_occ; i++) {
            U64 occ = 0ULL;
            U64 temp = mask;
            int bit = 0;
            while (temp) {
                int ls1b = POP(&temp);
                if (i & (1ULL << bit)) {
                    occ |= (1ULL << ls1b);
                }
                bit++;
            }
            U64 key = (occ * bishop_magics[sq]) >> bishop_shifts[sq];
            bishop_attacks[sq][key] = compute_bishop_attacks(sq, occ);
        }
    }
}
*/
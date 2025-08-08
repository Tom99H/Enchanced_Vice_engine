// search.c

#include "stdio.h"
#include "defs.h"

int rootDepth;

/* --------------------------------------------------------------- */
/* Helpers                                                         */
/* --------------------------------------------------------------- */

static void CheckUp(S_SEARCHINFO *info) {
    // .. check if time up, or interrupt from GUI
    if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
        info->stopped = TRUE;
    }
    ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {

    S_MOVE temp;
    int index = 0;
    int bestNum = moveNum;

    ASSERT(moveNum >= 0 && moveNum < list->count);

    int bestScore = list->moves[moveNum].score; /* nezačínej od 0 – skóre může být záporné */

    for (index = moveNum + 1; index < list->count; ++index) {
        if (list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }

    ASSERT(bestNum >= 0 && bestNum < list->count);
    ASSERT(bestNum >= moveNum);

    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}

static int IsRepetition(const S_BOARD *pos) {

    int index = 0;
    int start = pos->hisPly - pos->fiftyMove;
    if (start < 0) start = 0; /* ochrana proti zápornému indexu */

    for(index = start; index < pos->hisPly-1; ++index) {
        ASSERT(index >= 0 && index < MAXGAMEMOVES);
        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }
    return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {

    int index = 0;
    int index2 = 0;

    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < BRD_SQ_NUM; ++index2) {
            pos->searchHistory[index][index2] = 0;
        }
    }

    for(index = 0; index < 2; ++index) {
        for(index2 = 0; index2 < MAXDEPTH; ++index2) {
            pos->searchKillers[index][index2] = 0;
        }
    }

    pos->HashTable->overWrite=0;
    pos->HashTable->hit=0;
    pos->HashTable->cut=0;
    pos->ply = 0;

    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

/* --------------------------------------------------------------- */
/* Quiescence                                                      */
/* --------------------------------------------------------------- */

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {

    ASSERT(CheckBoard(pos));
    ASSERT(beta>alpha);
    if(( info->nodes & 2047 ) == 0) {
        CheckUp(info);
    }

    info->nodes++;

    if(IsRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if(pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    int Score = EvalPosition(pos);
    const int OldAlpha = alpha;  /* pro ASSERT a konzistenci */

    ASSERT(Score>-INFINITE && Score<INFINITE);

    if(Score >= beta) {
        return beta;
    }

    if(Score > alpha) {
        alpha = Score;
    }

    /* Delta pruning: pokud ani optimistický zisk materiálu nedotáhne na alpha, skonči */
    {
        const int delta = 900; /* ~Q */
        if (Score + delta <= alpha) {
            return alpha;
        }
    }

    S_MOVELIST list[1];
    GenerateAllCaps(pos,list);

    int MoveNum = 0;
    int Legal = 0;
    Score = -INFINITE;

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        PickNextMove(MoveNum, list);

        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }

        Legal++;
        Score = -Quiescence( -beta, -alpha, pos, info);
        TakeMove(pos);

        if(info->stopped == TRUE) {
            return 0;
        }

        if(Score > alpha) {
            if(Score >= beta) {
                if(Legal==1) {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
            alpha = Score;
        }
    }

    ASSERT(alpha >= OldAlpha);

    return alpha;
}

/* --------------------------------------------------------------- */
/* AlphaBeta                                                       */
/* --------------------------------------------------------------- */

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {

    ASSERT(CheckBoard(pos));
    ASSERT(beta>alpha);
    ASSERT(depth>=0);

    if(depth <= 0) {
        return Quiescence(alpha, beta, pos, info);
        // return EvalPosition(pos);
    }

    if(( info->nodes & 2047 ) == 0) {
        CheckUp(info);
    }

    info->nodes++;

    if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {
        return 0;
    }

    if(pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    /* Mate-distance pruning: uřízni mimo dosažitelný interval mate skóre */
    if (alpha < -INFINITE + pos->ply) alpha = -INFINITE + pos->ply;
    if (beta  >  INFINITE - pos->ply) beta  =  INFINITE - pos->ply;
    if (alpha >= beta) return alpha;

    int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);

    if(InCheck == TRUE) {
        depth++;
    }

    int Score = -INFINITE;
    int PvMove = NOMOVE;

    if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE ) {
        pos->HashTable->cut++;
        return Score;
    }

    int staticEval = -INFINITE;
    if (depth <= 3) {
        staticEval = EvalPosition(pos);
    }

    // Razoring (aggressive)
    if (depth <= 3 && !InCheck && staticEval + 300 + 100 * depth <= alpha && PvMove == NOMOVE) {
        return Quiescence(alpha, beta, pos, info);
    }

    if( DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 3) {
        /* adaptivní R: R3 pro mělké, R4 pro hlubší */
        int R = (depth >= 6 ? 4 : 3);
        MakeNullMove(pos);
        Score = -AlphaBeta( -beta, -beta + 1, depth - 1 - R, pos, info, FALSE);
        TakeNullMove(pos);
        if(info->stopped == TRUE) {
            return 0;
        }

        if (Score >= beta && abs(Score) < ISMATE) {
            info->nullCut++;
            return beta;
        }
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;

    int BestScore = -INFINITE;

    Score = -INFINITE;

    if( PvMove != NOMOVE) {
        for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
            if( list->moves[MoveNum].move == PvMove) {
                list->moves[MoveNum].score = 2000000;
                //printf("Pv move found \n");
                break;
            }
        }
    }

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        PickNextMove(MoveNum, list);

        int move = list->moves[MoveNum].move;

        // Futility pruning (mírnější marginy)
        if (depth <= 3 && !InCheck && staticEval + 120 + 120 * depth <= alpha &&
            !(move & MFLAGCAP) && !(move & MFLAGPROM)) {
            continue;
        }

        if ( !MakeMove(pos, move))  {
            continue;
        }

        Legal++;

        /* zkontroluj, zda tah dává šach – tiché šachy neredukujeme */
        int givesCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

        // PVS + LMR (neredukuj tiché šachy)
        int doLMR = (depth > 2 && Legal > 3 && !InCheck && !givesCheck &&
                     !(move & MFLAGCAP) && !(move & MFLAGPROM));
        int r = 0;
        if (doLMR) {
            r = 1;
            if (depth > 5) r++;
            if (Legal > 10) r++;
            if (r > depth - 2) r = depth - 2;
        }
        int newDepth = depth - 1 - r;

        int a = alpha;
        int b = beta;
        if (Legal > 1) {
            b = alpha + 1;
        }

        Score = -AlphaBeta(-b, -a, newDepth, pos, info, TRUE);

        if (doLMR && Score > alpha) {
            newDepth = depth - 1;
            Score = -AlphaBeta(-b, -a, newDepth, pos, info, TRUE);
        }

        if (Legal > 1 && Score > alpha) {
            Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
        }

        TakeMove(pos);

        if(info->stopped == TRUE) {
            return 0;
        }
        if(Score > BestScore) {
            BestScore = Score;
            BestMove = move;
            if(Score > alpha) {
                if(Score >= beta) {
                    if(Legal==1) {
                        info->fhf++;
                    }
                    info->fh++;

                    if(!(move & MFLAGCAP)) {
                        pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                        pos->searchKillers[0][pos->ply] = move;
                    }

                    StoreHashEntry(pos, BestMove, beta, HFBETA, depth);

                    return beta;
                }
                alpha = Score;

                if(!(move & MFLAGCAP)) {
                    pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
                }
            }
        }
    }

    if(Legal == 0) {
        if(InCheck) {
            return -INFINITE + pos->ply;
        } else {
            return 0;
        }
    }

    ASSERT(alpha>=OldAlpha);

    if(alpha != OldAlpha) {
        StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
    } else {
        StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
    }

    return alpha;
}

/* --------------------------------------------------------------- */
/* Root search / ID + aspiration                                   */
/* --------------------------------------------------------------- */

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {

    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;

    /* Aspiration state */
    int lastScore = 0;
    const int ASP_DELTA_START = 50;   /* ~0.50 pěšce */
    const int ASP_DELTA_MAX   = 300;  /* horní limit rozšíření okna */

    ClearForSearch(pos, info);

    if(EngineOptions->UseBook == TRUE) {
        bestMove = GetBookMove(pos);
    }

    // iterative deepening
    if(bestMove == NOMOVE) {
        for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {

            rootDepth = currentDepth;

            int alpha = -INFINITE;
            int beta  =  INFINITE;
            int delta = ASP_DELTA_START;

            /* od hloubky 2 používáme aspirační okno kolem posledního skóre */
            if (currentDepth > 1) {
                alpha = lastScore - delta;
                beta  = lastScore + delta;
            }

            bestScore = AlphaBeta(alpha, beta, currentDepth, pos, info, TRUE);

            /* při fail-low / fail-high postupně rozšiřujeme okno */
            while (info->stopped == FALSE &&
                   currentDepth > 1 &&
                   (bestScore <= alpha || bestScore >= beta) &&
                   delta < ASP_DELTA_MAX) {

                delta <<= 1; /* zdvojnásob okno */
                if (delta > ASP_DELTA_MAX) delta = ASP_DELTA_MAX;

                alpha = lastScore - delta;
                beta  = lastScore + delta;

                bestScore = AlphaBeta(alpha, beta, currentDepth, pos, info, TRUE);
            }

            if(info->stopped == TRUE) {
                break;
            }

            pvMoves = GetPvLine(currentDepth, pos);
            bestMove = pos->PvArray[0];

            if(info->GAME_MODE == UCIMODE) {
                printf("info score cp %d depth %d nodes %ld time %d ",
                    bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
            } else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
                printf("%d %d %d %ld ",
                    currentDepth,bestScore,(GetTimeMs()-info->starttime)/10,info->nodes);
            } else if(info->POST_THINKING == TRUE) {
                printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
                    bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
            }
            if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
                pvMoves = GetPvLine(currentDepth, pos);
                if (info->GAME_MODE != XBOARDMODE) {
                    printf("pv");
                }
                for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
                    printf(" %s",PrMove(pos->PvArray[pvNum]));
                }
                printf("\n");
            }

            //printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d\nOrdering %.2f NullCut:%d\n",pos->HashTable->hit,pos->HashTable->overWrite,pos->HashTable->newWrite,pos->HashTable->cut,
            //(info->fhf/info->fh)*100,info->nullCut);

            /* zapamatuj finální skóre této iterace jako střed okna pro další */
            lastScore = bestScore;
        }
    }

    if(info->GAME_MODE == UCIMODE) {
        printf("bestmove %s\n",PrMove(bestMove));
    } else if(info->GAME_MODE == XBOARDMODE) {
        printf("move %s\n",PrMove(bestMove));
        MakeMove(pos, bestMove);
    } else {
        printf("\n\n***!! Vice makes move %s !!***\n\n",PrMove(bestMove));
        MakeMove(pos, bestMove);
        PrintBoard(pos);
    }
}


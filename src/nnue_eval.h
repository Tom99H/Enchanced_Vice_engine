#ifndef NNUE_EVAL_H
#define NNUE_EVAL_H

#include "nnue/nnue.h"  // From the library
#include "defs.h"  // Vice board defs
extern "C" {
/*void init_nnue(const char* filename);
int evaluate_nnue(const S_BOARD *pos);  // Wrapper for Vice board*/
void init_nnue(char *filename);
int evaluate_nnue(int player, int *pieces, int *squares);
//int evaluate_nnue_incremental(int player, int *pieces, int *squares, void **nnue);
int evaluate_fen_nnue(char *fen);
}

#endif
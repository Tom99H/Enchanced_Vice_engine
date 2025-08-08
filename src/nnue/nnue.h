#ifndef NNUE_H
#define NNUE_H

#include <stdint.h>

#ifndef __cplusplus
#ifndef _MSC_VER
#include <stdalign.h>
#endif
#endif

/**
* Calling convention
*/
#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif
/*
#if defined (_WIN32)
#   define _CDECL __cdecl
#ifdef DLL_EXPORT
#   define DLLExport EXTERNC __declspec(dllexport)
#else
#   define DLLExport EXTERNC __declspec(dllimport)
#endif
#else
#   define _CDECL
#   define DLLExport EXTERNC
#endif
*/
/**
* Internal piece representation
*     wking=1, wqueen=2, wrook=3, wbishop= 4, wknight= 5, wpawn= 6,
*     bking=7, bqueen=8, brook=9, bbishop=10, bknight=11, bpawn=12
*
* Make sure the piecesyou pass to the library from your engine
* use this format.
*/
enum colors {
    white,black
};
enum pieces {
    blank=0,wking,wqueen,wrook,wbishop,wknight,wpawn,
            bking,bqueen,brook,bbishop,bknight,bpawn
};

/**
* nnue data structure
*/

typedef struct DirtyPiece {
  int dirtyNum;
  int pc[3];
  int from[3];
  int to[3];
} DirtyPiece;

typedef struct Accumulator {
  alignas(64) int16_t accumulation[2][256];
  int computedAccumulation;
} Accumulator;

typedef struct NNUEdata {
  Accumulator accumulator;
  DirtyPiece dirtyPiece;
} NNUEdata;

/**
* position data structure passed to core subroutines
*  See @nnue_evaluate for a description of parameters
*/
typedef struct Position {
  int player;
  int* pieces;
  int* squares;
  NNUEdata* nnue[3];
} Position;

int nnue_evaluate_pos(Position* pos);

/************************************************************************
*         EXTERNAL INTERFACES
*
* Load a NNUE file using
*
*   nnue_init(file_path)
*
* and then probe score using one of three functions, whichever
* is convenient. From easy to hard
*   
*   a) nnue_evaluate_fen         - accepts a fen string for evaluation
*   b) nnue_evaluate             - suitable for use in engines
*   c) nnue_evaluate_incremental - for ultimate performance but will need
*                                  some work on the engines side.
*
**************************************************************************/

/**
* Load NNUE file
*/
void nnue_init(const char *evalFile);
int nnue_evaluate(int player, int *pieces, int *squares);
int nnue_evaluate_incremental(int player, int *pieces, int *squares, NNUEdata **nnue);
int nnue_evaluate_fen(const char *fen);

/**
* Evaluation subroutine suitable for chess engines.
* -------------------------------------------------
* Piece codes are
*     wking=1, wqueen=2, wrook=3, wbishop= 4, wknight= 5, wpawn= 6,
*     bking=7, bqueen=8, brook=9, bbishop=10, bknight=11, bpawn=12,
* Squares are
*     A1=0, B1=1 ... H8=63
* Input format:
*     piece[0] is white king, square[0] is its location
*     piece[1] is black king, square[1] is its location
*     ..
*     piece[x], square[x] can be in any order
*     ..
*     piece[n+1] is set to 0 to represent end of array
* Returns
*   Score relative to side to move in approximate centi-pawns
*/


#endif

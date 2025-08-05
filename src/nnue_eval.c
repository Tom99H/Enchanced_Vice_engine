#include "nnue_eval.h"

void init_nnue(const char* filename) {
    printf("Attempting to load NNUE from: %s\n", filename);
    nnue_init(filename); // Library call
}

int evaluate_nnue(const S_BOARD *pos) {
    // Convert Vice board to library format
    int pieces[33], squares[33], player = pos->side;
    int idx = 0;
    for (int sq = 0; sq < 64; ++sq) {
        int pce = pos->pieces[SQ120(sq)];
        if (pce != EMPTY) {
            pieces[idx] = pce;  // Adjust enums if needed
            squares[idx] = sq;
            idx++;
        }
    }
    pieces[idx] = 0; squares[idx] = 0;  // Null terminate
    return nnue_evaluate(player, pieces, squares); // Library call, scaled
}
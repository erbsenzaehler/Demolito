#pragma once
#include <stdatomic.h>
#include "smp.h"
#include "zobrist.h"

extern atomic_uint_fast64_t Signal;
enum {STOP = (uint_fast64_t)(-1)};

typedef struct {
    int depth, movestogo;
    int64_t movetime, time, inc, nodes;
} Limits;

extern Position rootPos;
extern Stack rootStack;
extern Limits lim;
extern int Contempt;

int aspirate(Worker *worker, int depth, move_t pv[], int score);
int draw_score(int);
int iterate(Worker *worker);
void search_init(void);
int64_t search_go(void);

#pragma once
#include "types.h"
#include "smp.h"

void eval_init(void);
int evaluate(Worker *worker, const Position *pos);

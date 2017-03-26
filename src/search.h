#pragma once
#include <atomic>
#include <vector>
#include "position.h"
#include "zobrist.h"

namespace search {

extern thread_local int ThreadId;
extern std::vector<GameStack> gameStack;
extern std::vector<uint64_t> nodeCount;

extern int Threads;
extern int Contempt;

extern std::atomic<uint64_t> signal;
#define STOP    uint64_t(-1)

uint64_t nodes();

struct Limits {
    Limits(): depth(MAX_DEPTH), movetime(0), movestogo(0), time(0), inc(0), nodes(0) {}
    int depth, movetime, movestogo, time, inc;
    uint64_t nodes;
};

void init();

void bestmove(const Limits& lim, const GameStack& gameStack);

}

extern Position rootPos;

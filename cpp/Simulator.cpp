
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <Simulator.h>

Simulator::Simulator()
{
    std::srand(std::time(0));
}

Simulator::~Simulator() {}

uint Simulator::play()
{
    GameMatrix gm;
    uint count = 0;
    while (gm.can_move())
    {
        assert(gm.do_move(gm.get_best_move()));
        gm.fill_random_cell();
        ++count;
    }
    return count;
}

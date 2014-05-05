
#include <iostream>
#include <Simulator.h>

Simulator::Simulator() {}

Simulator::~Simulator() {}

uint Simulator::play()
{
    GameMatrix gm;
    uint count = 0;
    while (gm.can_move())
    {
        gm.do_move(gm.get_best_move());
        gm.fill_random_cell();
        ++count;
    }
    return count;
}
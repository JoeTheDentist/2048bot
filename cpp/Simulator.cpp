
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Simulator.h>

Simulator::Simulator()
{
    #ifndef NO_RANDOM
    std::srand(std::time(0));
    #else
    std::srand(0);
    #endif
}

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

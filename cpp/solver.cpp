
#include <iostream>
#include <GameMatrix.h>

int main()
{
    uint m[4][4] = {{2,2,2,2},{2,2,2,2},{4,4,4,4},{8,8,8,8}};
    GameMatrix gm(m);
    std::cout << "initial state: " << std::endl;
    gm.dump();
    GameMatrix gm_up = gm.simulate_move(UP);
    std::cout << "after move UP: " << std::endl;
    gm_up.dump();
    return 0;
}


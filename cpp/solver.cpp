
#include <GameMatrix.h>

int main()
{
    uint m[4][4] = {{2,2,2,2},{2,2,2,2},{4,4,4,4},{8,8,8,8}};
    GameMatrix gm(m);
    GameMatrix gm_up = gm.simulate_move(UP);
    gm_up.dump();
	return 0;
}


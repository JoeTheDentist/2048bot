
#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <vector>
#include <GameMatrix.h>

class Simulator
{
public:
    /**
     * @brief constructor
     */
    Simulator();
    
    /**
     * @brief destructor
     */
    ~Simulator();
    
    /**
     * @brief simulate a round
     */
    void game_loop();

    /**
     * @brief play once
     */
    void play_once();

private:
    // should be an interface to simulate multiple 
    GameMatrix _gm;
    std::vector<position> _free_cells;
};

#endif

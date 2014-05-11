
#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

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
     * @brief simulate a full game
     * @return number of move for the game
     * @todo allow different implementations of game matrix
     */
    uint play();
};

#endif

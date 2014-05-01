
#include <iostream>
#include <GameMatrix.h>

GameMatrix::GameMatrix(const uint (&m)[4][4]) : _size(4) {
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            _matrix[i][j] = m[i][j];
        }
    }
}

GameMatrix::~GameMatrix() {}

GameMatrix::GameMatrix(const GameMatrix &copy) : _size(copy._size) {
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            _matrix[i][j] = copy._matrix[i][j];
        }
    }
}

GameMatrix& GameMatrix::operator=(const GameMatrix &copy)
{
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            _matrix[i][j] = copy._matrix[i][j];
        }
    }
    return *this;
}

// Could be optimized storing a hash when updating the matrix
bool GameMatrix::operator==(const GameMatrix &gm) const
{
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            if (_matrix[i][j] != gm._matrix[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

GameMatrix GameMatrix::simulate_move(move m) const
{
    GameMatrix gm(*this);
    for (uint i=0; i<_size; ++i)
    {
        uint pos = 0;
        uint value = 0;
        for (uint j=0; j<_size; ++j)
        {
            uint new_value = _get_at(i, j, m);
            if (new_value == 0)
            {
                // 0, nothing to do
            }
            else if (value == new_value)
            {
                // same cells, can merge
                gm._set_at(i, pos, m, 2 * value);
                ++pos;
                value = 0;
            }
            else
            {
                // cannot merge
                if (value == 0)
                {
                    gm._set_at(i, pos, m, new_value);
                }
                else
                {
                    ++pos;
                }
                value = new_value;
            }
        }
        // Putting 0s in the rest
        uint start = pos+1;
        if (value == 0)
        {
            start = pos;
        }
        for (uint j=start; j<_size; ++j)
        {
            gm._set_at(i, j, m, 0);
        }
    }
    return gm;
}

// Could be optimized storing the weight at matrix update
uint GameMatrix::get_weight() const
{
    uint weight = 0;
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            if (_matrix[i][j] != 0)
            {
                ++weight;
            }
        }
    }
    return weight;
}

void GameMatrix::dump() const
{
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            std::cout << _matrix[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

position GameMatrix::_get_pos(uint i, uint j, move m) const
{
    /*
     * i is the increment of the outer loop
     * j is the increment of the inner loop
     *
     * if move LEFT then:
     * go from left to right (j)
     * and from top to bottom (i)
     */
    position pos(i, j);

    if (m == RIGHT)
    {
        /*
         * go from right to left (j)
         * and from top to bottom (i)
         */
        pos.i = i;
        pos.j = _size - j - 1;
    }

    else if (m == DOWN)
    {
        /*
         * go from bottom to top (j)
         * and from left to right (i)
         */
        pos.i = _size - j - 1;
        pos.j = i;
    }

    else if (m == UP)
    {
        /*
         * go from top to bottom (j)
         * and from left to right (i)
         */
        pos.j = i;
        pos.i = j;
    }

    return pos;
}

uint GameMatrix::_get_at(uint i, uint j, move m) const
{
    position p = _get_pos(i,j,m);
    return _matrix[p.i][p.j];
}

void GameMatrix::_set_at(uint i, uint j, move m, uint value)
{
    position p = _get_pos(i,j,m);
    _matrix[p.i][p.j] = value;
}

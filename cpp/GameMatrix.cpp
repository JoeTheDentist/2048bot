
#include <iostream>
#include <cstdlib>
#include <GameMatrix.h>

GameMatrix::GameMatrix() : _free_cells(M_SIZE) {
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            _matrix[i][j] = 0;
        }
    }
    fill_random_cell();
    fill_random_cell();
}

GameMatrix::GameMatrix(const uint (&m)[SIZE][SIZE]) : _free_cells(M_SIZE) {
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            _set(i, j, m[i][j]);
        }
    }
}

GameMatrix::~GameMatrix() {}

GameMatrix::GameMatrix(const GameMatrix &copy) : _free_cells(copy._free_cells) {
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            _matrix[i][j] = copy._matrix[i][j];
        }
    }
}

GameMatrix& GameMatrix::operator=(const GameMatrix &copy)
{
    _free_cells = copy._free_cells;
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            _matrix[i][j] = copy._matrix[i][j];
        }
    }
    return *this;
}

// Could be optimized storing a hash when updating the matrix
bool GameMatrix::operator==(const GameMatrix &gm) const
{
    if (_free_cells != gm._free_cells)
    {
        return false;
    }
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            if (_matrix[i][j] != gm._matrix[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

bool GameMatrix::operator!=(const GameMatrix &gm) const
{
    return !(*this == gm);
}

GameMatrix GameMatrix::simulate_move(move m) const
{
    GameMatrix gm(*this);
    gm.do_move(m);
    return gm;
}

bool GameMatrix::do_move(move m)
{
    bool valid = false;
    for (uint i=0; i<SIZE; ++i)
    {
        uint pos = 0;
        uint value = 0;
        for (uint j=0; j<SIZE; ++j)
        {
            uint new_value = _get_at(i, j, m);
            if (new_value == 0)
            {
                // 0, nothing to do
            }
            else if (value == new_value)
            {
                // same cells, can merge
                _set_at(i, pos, m, 2 * value);
                ++pos;
                value = 0;
                ++_free_cells;
                valid = true;
            }
            else
            {
                // cannot merge
                if (value != 0)
                {
                    ++pos;
                }
                _set_at(i, pos, m, new_value);
                value = new_value;
                if (!valid && j != pos) {
                    valid = true;
                }
            }
        }
        // Putting 0s in the rest
        uint start = pos+1;
        if (value == 0)
        {
            start = pos;
        }
        for (uint j=start; j<SIZE; ++j)
        {
            _set_at(i, j, m, 0);
        }
    }
    return valid;
}

double GameMatrix::get_weight() const
{
    return free_cells_count();
}

uint GameMatrix::free_cells_count() const
{
    return _free_cells;
}

void GameMatrix::fill_random_cell()
{
    if (unlikely(_free_cells == 0))
    {
        dump();
    }

    uint pos_to_add = rand() % _free_cells;
    uint count = 0;
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            if (_matrix[i][j] == 0)
            {
                if (count == pos_to_add)
                {
                    // to check, I wonder if it is possible to have a 4 sometimes
                    _set(i, j, 2);
                }
                ++count;
            }
        }
    }
}

bool GameMatrix::can_move() const
{
    if (_free_cells != 0)
    {
        return true;
    }
    for (uint i=0; i<4; ++i)
    {
        GameMatrix gm = simulate_move(static_cast<move>(i));
        if (gm != *this)
        {
            return true;
        }
    }
    return false;
}

move GameMatrix::get_best_move() const
{
    return _get_best_move(3).m;
}

void GameMatrix::dump() const
{
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
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
        pos.j = SIZE - j - 1;
    }

    else if (m == DOWN)
    {
        /*
         * go from bottom to top (j)
         * and from left to right (i)
         */
        pos.i = SIZE - j - 1;
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

void GameMatrix::_set(uint i, uint j, uint value)
{
    _matrix[i][j] = value;
    if (value != 0)
    {
        --_free_cells;
    }
}

move_action GameMatrix::_get_best_move(uint depth) const
{
    move best_move = static_cast<move>(-1);
    double best_weight = 0;
    for (uint i=0; i<4; ++i)
    {
        move m = static_cast<move>(i);
        GameMatrix current_matrix = simulate_move(m);
        double current_weight = 0;
        if (*this == current_matrix)
        {
            continue;
        }
        if (depth == 0)
        {
            current_weight = current_matrix.get_weight();
        }
        else
        {
            current_weight = 0;
            uint free_cell_count = 0;
            for (uint i=0; i<SIZE; ++i)
            {
                for (uint j=0; j<SIZE; ++j)
                {
                    if (current_matrix._matrix[i][j] == 0)
                    {
                        ++free_cell_count;
                        GameMatrix curr_mat_rand(current_matrix);
                        curr_mat_rand._set(i, j, 2);
                        current_weight += curr_mat_rand._get_best_move(depth - 1).weight;
                    }
                }
            }
            current_weight /= free_cell_count;
            current_weight = depth * current_weight + current_matrix.get_weight();
            current_weight /= (depth + 1);
        }
        if (best_weight < current_weight)
        {
            best_move = m;
            best_weight = current_weight;
        }
    }
    return move_action(best_weight, best_move);
}

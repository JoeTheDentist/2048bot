
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GameMatrix.h>

GameMatrix::GameMatrix() : _size(4), _tmp_vector(16) {
    std::srand(std::time(0));
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            _matrix[i][j] = 0;
        }
    }
    fill_random_cell();
    fill_random_cell();
}

GameMatrix::GameMatrix(const uint (&m)[4][4]) : _size(4), _tmp_vector(16) {
    std::srand(std::time(0));
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
    gm.do_move(m);
    return gm;
}

void GameMatrix::do_move(move m)
{
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
                _set_at(i, pos, m, 2 * value);
                ++pos;
                value = 0;
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
            _set_at(i, j, m, 0);
        }
    }
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

void GameMatrix::get_free_cells(std::vector<position> &v) const
{
    // the stl documentation does not provide guarantee that the vector is not
    // resized. It says it does not guarantee rezise so I guess it is possible
    // to have a resize. If it is the case, find a way to prevent it.
    // We don't want the vector to be resized.
    // Also check whether it is O(n) or O(1), according to documentation, it is
    // O(1) only for scalars and PODs (should be the case here).
    // The operation have to be O(1).
    v.clear();
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            if (_matrix[i][j] == 0)
            {
                v.push_back(position(i,j));
            }
        }
    }
}

void GameMatrix::fill_random_cell()
{
    get_free_cells(_tmp_vector);
    position rand_pos = _tmp_vector[rand() % _tmp_vector.size()];
    // to check, I wonder if it is possible to have a 4 sometimes
    _matrix[rand_pos.i][rand_pos.j] = 2;
}

bool GameMatrix::can_move() const
{
    uint weight = get_weight();
    if (weight != _size * _size)
    {
        return true;
    }
    for (uint i=0; i<4; ++i)
    {
        GameMatrix gm = simulate_move(static_cast<move>(i));
        if (gm == *this)
        {
            return true;
        }
    }
    return false;
}

move GameMatrix::get_best_move() const
{
    return _get_best_move(2).m;
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

move_action GameMatrix::_get_best_move(uint depth) const
{
    move best_move = static_cast<move>(std::rand() % 4);
    GameMatrix best_matrix = simulate_move(best_move);
    uint best_weight = 0;
    if (*this == best_matrix)
    {
        best_weight = 17;
    }
    else
    {
        best_weight = best_matrix.get_weight();
    }
    for (uint i=0; i<4; ++i)
    {
        move m = static_cast<move>(i);
        GameMatrix current_matrix = simulate_move(m);
        uint current_weight = 17;
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
            current_weight = current_matrix._get_best_move(depth - 1).weight;
        }
        if (best_weight > current_weight)
        {
            best_matrix = current_matrix;
            best_move = m;
            best_weight = current_weight;
        }
    }
    return move_action(best_matrix, best_weight, best_move);
}

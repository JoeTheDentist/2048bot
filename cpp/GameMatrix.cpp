
#include <iostream>
#include <cstdlib>
#include <GameMatrix.h>

typedef position p;

// pre-processed conversion table
// could be computed at compile time by templates
const position GameMatrix::_get_pos_table[4][SIZE][SIZE] =
{{//UP
  {p(0,0), p(1,0), p(2,0), p(3,0)},
  {p(0,1), p(1,1), p(2,1), p(3,1)},
  {p(0,2), p(1,2), p(2,2), p(3,2)},
  {p(0,3), p(1,3), p(2,3), p(3,3)}},
 {// RIGHT
  {p(0,3), p(0,2), p(0,1), p(0,0)},
  {p(1,3), p(1,2), p(1,1), p(1,0)},
  {p(2,3), p(2,2), p(2,1), p(2,0)},
  {p(3,3), p(3,2), p(3,1), p(3,0)}},
 {// DOWN
  {p(3,0), p(2,0), p(1,0), p(0,0)},
  {p(3,1), p(2,1), p(1,1), p(0,1)},
  {p(3,2), p(2,2), p(1,2), p(0,2)},
  {p(3,3), p(2,3), p(1,3), p(0,3)}},
 { // LEFT
  {p(0,0), p(0,1), p(0,2), p(0,3)},
  {p(1,0), p(1,1), p(1,2), p(1,3)},
  {p(2,0), p(2,1), p(2,2), p(2,3)},
  {p(3,0), p(3,1), p(3,2), p(3,3)}}};

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

// auto_ptr to avoid useless copy? Maybe already optimized (return value optimization)
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
            current_weight = 2 * depth * current_weight + current_matrix.get_weight();
            current_weight /= (2 * depth + 1);
        }
        if (best_weight < current_weight)
        {
            best_move = m;
            best_weight = current_weight;
        }
    }
    return move_action(best_weight, best_move);
}

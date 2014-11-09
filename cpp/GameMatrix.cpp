
#include <iostream>
#include <cstdlib>
#include <cassert>
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

bool GameMatrix::move_line(move m, int i)
{
    bool valid = false;
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
            valid = true;
            ++_free_cells;
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
            if (j != pos) {
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

    return valid;
}

uint GameMatrix::do_move(move m)
{
    uint moving_lines = 0;
    for (uint i=0; i<SIZE; ++i)
    {
        if (move_line(m, i)) {
            moving_lines++;
        }
    }
    return moving_lines;
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
    double best_weight = 0;
    move best_move = DOWN;

    for (uint i=0; i<4; ++i)
    {
        move m = static_cast<move>(i);

        // current_matrix is a simulation of a move without
        // addition of a new tile.
        GameMatrix current_matrix(*this);

        if (current_matrix.do_move(m) == 0) {
            // invalid move
            continue;
        }

        double current_weight = current_matrix._get_best_move(DEPTH);
        if (best_weight < current_weight)
        {
            best_move = m;
            best_weight = current_weight;
        }
    }

    return best_move;
}

void GameMatrix::dump() const
{
    std::cout << *this;
}

std::ostream & operator<<(std::ostream & left_op, const GameMatrix& gm)
{
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            left_op << gm._matrix[i][j] << "\t";
        }
        left_op << std::endl;
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

double GameMatrix::_get_best_move(uint depth) const
{
    if (depth == 0) {
        return get_weight();
    }

    double weight = 0; 
    double weight_matrix [SIZE][SIZE];
    uint count = 0;
    bool one_valid_move = false;

    // init weight_matrix
    for (uint i=0; i<SIZE; i++) {
        for (uint j=0; j<SIZE; j++) {
            if (_matrix[i][j] == 0) {
                weight_matrix[i][j] = MINIMAL_WEIGHT;
                count++;
            }
            else {
                weight_matrix[i][j] = -1;
            }
        }
    }

    assert(count > 0);

    // for each move
    for (uint i=0; i<4; ++i)
    {
        move m = static_cast<move>(i);
        double current_weight=0;
        uint moving_lines;
        bool valid = false;

        // current_matrix is a simulation of a move without
        // addition of a new tile. 
        GameMatrix current_matrix(*this);

        moving_lines = current_matrix.do_move(m);
        if (moving_lines == 0) {
            // invalid move
            continue;
        }

        // look for possible spots where to add a new tile
        for (uint i=0; i<SIZE; i++) {
            current_weight = 0;
            for (uint j=0; j<SIZE; j++) {
                if(_get_at(i, j, m) == 0) {
                    // if previous square is empty then adding a new
                    // tile will produce the same matrix as before.
                    if (j>0 && _get_at(i, j-1, m) == 0 && current_weight > 0) {
                        const position * p = _get_pos(i, j, m);
                        if (current_weight > weight_matrix[p->i][p->j]) {
                            weight_matrix[p->i][p->j] = current_weight;
                        }
                        continue;
                    }

                    // re-compute corresponding column/row
                    GameMatrix refined_matrix(current_matrix);

                    // restore column/row from initial state
                    for (int k=0; k<SIZE; k++) {
                        refined_matrix._set_at(i, k, m, _get_at(i, k, m));
                    }

                    // add a new tile
                    refined_matrix._set_at(i, j, m, 2);

                    // move line
                    bool has_moved = refined_matrix.move_line(m, i);

                    // if adding a new tile prevents the only moving line to move
                    // then it is an invalid move.
                    // otherwise the move is validated and weights are updated.
                    if (has_moved || moving_lines > 1) {
                        valid = true;
                        current_weight = refined_matrix._get_best_move(depth - 1);
                        const position * p = _get_pos(i, j, m);
                        if (current_weight > weight_matrix[p->i][p->j]) {
                            weight_matrix[p->i][p->j] = current_weight;
                        }
                    }
                }
            }
        }
        if (valid) {
            one_valid_move = true;
        }
    }

    if (one_valid_move) {
        for (uint i=0; i<SIZE; i++) {
            for (uint j=0; j<SIZE; j++) {
                if (weight_matrix[i][j] > -1) {
                    weight += weight_matrix[i][j];
                }
            }
        }
        return (double) weight / (double) count;
    }

    else {
        return MINIMAL_WEIGHT;
    }
}

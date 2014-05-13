
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <GameMatrix.h>

std::vector<position> GameMatrix::_tmp_vector(16);

GameMatrix::GameMatrix() {
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

GameMatrix::GameMatrix(const uint (&m)[4][4]) {
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            _matrix[i][j] = m[i][j];
        }
    }
}

GameMatrix::~GameMatrix() {}

GameMatrix::GameMatrix(const GameMatrix &copy) {
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

uint GameMatrix::get_weight() const
{
    return free_cells_count();
}

// Could be optimized storing the count at matrix update
uint GameMatrix::free_cells_count() const
{
    uint count = 0;
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
        {
            if (_matrix[i][j] != 0)
            {
                ++count;
            }
        }
    }
    return count;
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
    for (int i=0; i<SIZE; ++i)
    {
        for (int j=0; j<SIZE; ++j)
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
    get_free_cells(GameMatrix::_tmp_vector);
    uint size = GameMatrix::_tmp_vector.size();
    if (size == 0)
    {
        dump();
    }
    uint rand_idx = rand() % size;
    position rand_pos = GameMatrix::_tmp_vector[rand_idx];
    // to check, I wonder if it is possible to have a 4 sometimes
    _matrix[rand_pos.i][rand_pos.j] = 2;
}

bool GameMatrix::can_move() const
{
    uint free_cells = free_cells_count();
    if (free_cells != M_SIZE)
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
    double best_weight = DISSUASIVE_WEIGHT + 1;
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

        double current_weight = current_matrix._get_best_move(4);
        if (best_weight > current_weight)
        {
            best_move = m;
            best_weight = current_weight;
        }
    }

    // if no move looks good, then just play a valid move
    if (best_weight >= DISSUASIVE_WEIGHT) {
        for (uint i=0; i<4; i++) {
            move m = static_cast<move>(i);

            GameMatrix current_matrix(*this);

            if (current_matrix.do_move(m) != 0) {
                best_move = m;
                break;
            }
        }
    }

    return best_move;
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
                weight_matrix[i][j] = DISSUASIVE_WEIGHT;
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
                        position p = _get_pos(i, j, m);
                        if (current_weight < weight_matrix[p.i][p.j]) {
                            weight_matrix[p.i][p.j] = current_weight;
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
                        position p = _get_pos(i, j, m);
                        if (current_weight < weight_matrix[p.i][p.j]) {
                            weight_matrix[p.i][p.j] = current_weight;
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
        return DISSUASIVE_WEIGHT;
    }
}

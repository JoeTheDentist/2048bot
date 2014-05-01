
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

GameMatrix & GameMatrix::operator=(const GameMatrix &copy)
{
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
        {
            _matrix[i][j] = copy._matrix[i][j];
        }
    }
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

position GameMatrix::_get_pos(uint i, uint j, move m)
{
    position pos(i,j);
    if (m == RIGHT)
    {
        pos.j = _size - pos.j;
    }
    else if (m == DOWN)
    {
        uint tmp = pos.j;
        pos.j = _size - pos.i;
        pos.i = _size - tmp;
    }
    else if (m == UP)
    {
        uint tmp = pos.j;
        pos.j = pos.i;
        pos.i = tmp;
    }
    return pos;
}

uint GameMatrix::_get_at(uint i, uint j, move m)
{
    position p = _get_pos(i,j,m);
    return _matrix[p.i][p.j];
}

void GameMatrix::_set_at(uint i, uint j, move m, uint value)
{
    position p = _get_pos(i,j,m);
    _matrix[p.i][p.j] = value;
}

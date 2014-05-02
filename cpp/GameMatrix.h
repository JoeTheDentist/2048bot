
typedef unsigned int uint;

enum move
{
    UP=0, RIGHT, DOWN, LEFT
};

struct position
{
    position(uint i = 0, uint j = 0) : i(i), j(j) {}
    uint i;
    uint j;
    bool operator==(const position& pos)
    {
        return i==pos.i && j==pos.j;
    }
};

/**
 * @brief The GameMatrix class (square matrix, size x size)
 */
class GameMatrix
{
public:
    /**
     * @brief GameMatrix constructor
     * @param m input matrix
     */
    GameMatrix(const uint (&m)[4][4]);

    /**
     *  @brief GameMatrix destructor
     */
    ~GameMatrix();

    /**
     * @brief GameMatrix constructor
     * @param copy
     */
    GameMatrix(const GameMatrix &copy);

    /**
     * @brief Copy assignment
     * @param copy
     * @return copy
     */
    GameMatrix& operator=(const GameMatrix &copy);

    /**
     * @brief operator ==
     * @param gm
     * @return whether the two objects are equal
     */
    bool operator==(const GameMatrix &gm) const;

    /**
     * @brief Simulate the given move
     * @param m
     * @return new matrix with the move applied
     */
    GameMatrix simulate_move(move m) const;

    /**
     * @brief get_weight
     * @return weight
     */
    uint get_weight() const;

    /**
     * @brief dump in stdout
     */
    void dump() const;

    /**
     * @brief _get_pos, helper function to have only one way to treat positions
     * @param i
     * @param j
     * @param m
     * @return real position
     */
    position _get_pos(uint i, uint j, move m) const;

    /**
     * @brief _get_at, helper function to get value (relies _get_pos)
     * @param i
     * @param j
     * @param m
     * @return value in the real position
     */
    uint _get_at(uint i, uint j, move m) const;

    /**
     * @brief _set_at, helper function to set vale (relies on _get_pos)
     * @param i
     * @param j
     * @param m
     * @param value
     */
    void _set_at(uint i, uint j, move m, uint value);

public:
    uint _size;
    uint _matrix[4][4];
};

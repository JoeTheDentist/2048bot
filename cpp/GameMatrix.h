
#ifndef _GAMEMATRIX_H_
#define _GAMEMATRIX_H_

#include <vector>

#define SIZE 4
#define M_SIZE 16

typedef unsigned int uint;
struct move_action;

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

struct move_action
{
    move_action(double weight, move m) : weight(weight), m(m) {}
    double weight;
    move m;
};

/**
 * @brief The GameMatrix class (square matrix, size x size)
 */
class GameMatrix
{
public:
    /**
     * @brief default constructor, for real game. Fill randomly two cells with 2
     */
    GameMatrix();

    /**
     * @brief GameMatrix constructor
     * @param m input matrix
     */
    GameMatrix(const uint (&m)[SIZE][SIZE]);

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
     * @brief operator ==
     * @param gm
     * @return whether the two objects are different
     */
    bool operator!=(const GameMatrix &gm) const;

    /**
     * @brief Simulate the given move
     * @param move to simulate
     * @return new matrix with the move applied
     */
    GameMatrix simulate_move(move m) const;

    /**
     * @brief do the move
     * @param move to apply
     * @return true if move is valid (i.e. something has changed on the board)
     */
    bool do_move(move m);

    /**
     * @brief get_weight
     * @return weight
     */
    double get_weight() const;

    /**
     * @brief number of free cells
     * @return number of free cells
     */
    uint free_cells_count() const;
    
    /**
     * @brief get empty cells
     * @param vector to fill with the empty positions
     * @todo make sure no resize happen
     */
    void get_free_cells(std::vector<position> &v) const;

    /**
     * @brief fill randomly an empty cell accordingly to 2048 rules
     */
    void fill_random_cell();

    /**
     * @brief check whether there are still possible moves
     * @return true if there is, false in the other case
     * Relies on get_weight for future optimization.
     */
    bool can_move() const;

    /**
     * @brief compute the best move to play
     * @todo implement
     */
    move get_best_move() const;

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
     * @brief _set_at, helper function to set value (relies on _get_pos)
     * @param i
     * @param j
     * @param m
     * @param value
     */
    void _set_at(uint i, uint j, move m, uint value);

    /**
     * @brief set value at i j of the matrix, updates free cell count
     * @param i
     * @param j
     * @param value
     */
    void _set(uint i, uint j, uint value);

    /**
     * @brief recursive function for get_best_move
     * @param depth: depth of the recurtion
     * @return best move to play
     */
    move_action _get_best_move(uint depth = 0) const;

private:
    uint _matrix[SIZE][SIZE];
    uint _free_cells;
    static std::vector<position> _tmp_vector;
};

#endif

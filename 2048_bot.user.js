// ==UserScript==
// @name        2048 bot
// @namespace   2048_bot
// @description Bot for the game 2048
// @version     0.1.0
// @grant       none
// @include     http://gabrielecirulli.github.io/2048/
// ==/UserScript==

UP = 0;
RIGHT = 1;
DOWN = 2;
LEFT = 3;
MOVES = [UP, RIGHT, DOWN, LEFT];

// Generate empty matrix (square)
function generate_matrix(size)
{
    var matrix = [];
    for(var i=0; i<size; i++)
    {
        matrix[i] = [];
        for(var j=0; j<size; j++)
        {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

// Create a new matrix with the same values
function copy_matrix(matrix, size)
{
    var new_matrix = [];
    for(var i=0; i<size; i++)
    {
        new_matrix[i] = [];
        for(var j=0; j<size; j++)
        {
            new_matrix[i][j] = matrix[i][j];
        }
    }
    return new_matrix;
}

// Refresh matrix with raw cells
function refresh_matrix(matrix, raw_cells, size)
{
    for(var i=0; i<size; i++)
    {
        for(var j=0; j<size; j++)
        {
            if (raw_cells[j][i]) {
                matrix[i][j] = raw_cells[j][i].value;
            } else {
                matrix[i][j] = 0;
            }
        }
    }
}

// Refresh with game data
function refresh(matrix, game_manager, size)
{
    var raw_cells = eval('('+game_manager.storageManager.storage.gameState+')').grid.cells;
    refresh_matrix(matrix, raw_cells, size);
}

// Is the game over?
function over(game_manager)
{
    return eval('('+game_manager.storageManager.storage.gameState+')').over;
}

// Get the current best possible move
function get_best_move(matrix, size)
{
    get_best_move_aux(matrix, size);
    return get_best_move_aux(matrix, size)[1];
    //return Math.floor(Math.random()*4);
}

// Recursive function for getting the best move
function get_best_move_aux(matrix, size)
{
    var best_move = Math.floor(Math.random()*4);
    var best_matrix = simulate_move(matrix, size, move);
    var best_weight = get_weight(best_matrix, size);
    for (var i=0; i<4; i++)
    {
        var move = MOVES[i];
        var matrix_move = simulate_move(matrix, size, move);
        var weight = get_weight(matrix_move, size);
        if (weight < best_weight) {
            best_weight = weight;
            best_matrix = matrix_move;
            best_move = move;
        }
    }
    return [best_matrix, best_move, best_weight];
}

// Generate the new matrix with the given move
function simulate_move(matrix, size, move)
{
    var new_matrix = copy_matrix(matrix, size);
    for (var i = 0; i < size; i++)
    {
        var pos = 0;
        var value = 0;
        for (var j = 0; j < size; j++)
        {
            var new_value = get_at(new_matrix, i, j, move);
            if (new_value == 0)
            {
                // Nothing to do here
            }
            else if (value == new_value)
            {
                set_at(new_matrix, i, pos, move, 2*value);
                pos++;
                value = 0;
            }
            else
            {
                if (value == 0)
                {
                    set_at(new_matrix, i, pos, move, new_value);
                }
                else
                {
                    //Was not able to merge.
                    pos++;
                }
                value = new_value;
            }
        }
        for (var j = pos+1; j < size; j++)
        {
            set_at(new_matrix, i, j, move, 0);
        }
    }
    return new_matrix;
}

// Get the real position according to move
function get_pos(i, j, move)
{
    var pos = [i, j];
    if (move == RIGHT)
    {
        pos[1] = 3 - pos[1];
    }
    else if (move == DOWN)
    {
        var tmp = pos[1];
        pos[1] = 3 - pos[0];
        pos[0] = 3 - tmp;
    }
    else if (move == UP)
    {
        var tmp = pos[1];
        pos[1] = pos[0];
        pos[0] = tmp;
    }
    return pos;
}

// Set the value to the real pos
function set_at(matrix, i, j, move, value)
{
    var pos = get_pos(i, j, move);
    matrix[pos[0]][pos[1]] = value;
}

// Set the value to the real pos
function get_at(matrix, i, j, move)
{
    var pos = get_pos(i, j, move);
    return matrix[pos[0]][pos[1]];
}

// Get the weight of the matrix. Metric to be able to compare solutions.
function get_weight(matrix, size)
{
    // Just counting the non empty cells for now
    console.log(matrix);
    var weight = 0;
    var cell_map = {};
    for(var i=0; i<size; i++)
    {
        for(var j=0; j<size; j++)
        {
            if (matrix[i][j] != 0)
            {
                weight++;
            }
        }
    }
    console.log(weight);
    return weight;
}

// Main function
function main()
{
    console.log("###### Starting 2048 bot ######");
    var size = 4;
    gm = new GameManager(size, KeyboardInputManager, HTMLActuator, LocalStorageManager);
    grid = generate_matrix(size);
    while (!over(gm))
    {
        refresh(grid, gm, size);
        gm.move(get_best_move(grid, size));
    }
    console.log("###### Stopping 2048 bot ######");
}

main();

// ==UserScript==
// @name        2048_bot
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
MAXWEIGHT = 16

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

// Checks whether the two matrix are equal
function equals_matrix(m1, m2, size)
{
    for(var i=0; i<size; i++)
    {
        for(var j=0; j<size; j++)
        {
            if (m1[i][j] != m2[i][j])
            {
                return false;
            }
        }
    }
    return true;
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
    var best = get_best_move_aux(matrix, size, 2);
    console.log(best[0]);
    console.log(best[2]);
    return best[1];
}

// Recursive function for getting the best move
function get_best_move_aux(matrix, size, depth)
{
    var best_move = Math.floor(Math.random()*4);
    var best_matrix = simulate_move(matrix, size, move);
    var best_weight = 0;
    if (equals_matrix(best_matrix, matrix, size)) {
        best_weight = MAXWEIGHT + 1; // do not play that move
    }
    else {
        best_weight = get_weight(best_matrix, size);
    }
    for (var i=0; i<4; i++)
    {
        var move = MOVES[i];
        var matrix_move = simulate_move(matrix, size, move);
        var weight = MAXWEIGHT + 1;
        if (equals_matrix(matrix_move, matrix, size)) {
            continue; // do not play that move
        }
        if (depth == 0)
        {
            weight = get_weight(matrix_move, size);
        }
        else
        {
            weight = get_best_move_aux(matrix_move, size, depth - 1)[2];
        }
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
        var start = pos+1;
        if (value == 0)
        {
            start = pos;
        }
        for (var j = start; j < size; j++)
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
    return weight;
}

function loop()
{
    if (!over(gm))
    {
        refresh(grid, gm, size);
        gm.move(get_best_move(grid, size));
    }
}

// Play mode (greasemonkey)
function run()
{
    console.log("### Running 2048 bot ###");
    size = 4;
    gm = new GameManager(size, KeyboardInputManager, HTMLActuator, LocalStorageManager);
    grid = generate_matrix(size);
    setInterval(loop, 250);
}

//----------------------------------------------------------------------
//---TESTING------------------------------------------------------------

function function_name(fun) {
  var ret = fun.toString();
  ret = ret.substr('function '.length);
  ret = ret.substr(0, ret.indexOf('('));
  return ret;
}

function assert_matrix(in_m, out_m, exp_m, arg, size)
{
    if (!equals_matrix(out_m, exp_m, size))
    {
        console.log("*** Test failed for data", in_m, "and argument", arg, "expecting", exp_m, "returned", out_m);
        return 1;
    }
    return 0;
}

// Test simulate_move function. Returns number of failed tests.
function test_simulate_move()
{
    var count = 0;
    var ref = [[0, 0, 0, 0], [0, 2, 0, 0], [0, 0, 2, 0], [0, 0, 0, 0]];
    var one_move = [[[0, 2, 2, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]],
                [[0, 0, 0, 0], [0, 0, 0, 2], [0, 0, 0, 2], [0, 0, 0, 0]],
                [[0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 2, 2, 0]],
                [[0, 0, 0, 0], [2, 0, 0, 0], [2, 0, 0, 0], [0, 0, 0, 0]]];
    var multi_moves = [[0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [4, 0, 0, 0]];
    var multi_merge = [[2, 2, 2, 2], [2, 2, 2, 2], [2, 2, 2, 2], [2, 2, 2, 2]];
    var multi_merge_down = [[0, 0, 0, 0], [0, 0, 0, 0], [4, 4, 4, 4], [4, 4, 4, 4]];
    var multi_merge_up = [[8, 8, 8, 8], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]];
    var multi_merge_left = [[16, 16, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]];
    var multi_merge_right = [[0, 0, 0, 32], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]];
    for (var move in MOVES)
    {
        // Test one move
        var test_matrix = simulate_move(ref, 4, move);
        count += assert_matrix(ref, test_matrix, one_move[move], move, 4);
        // Test the same move (should not change anything)
        var second_test_matrix = simulate_move(test_matrix, 4, move);
        count += assert_matrix(ref, second_test_matrix, one_move[move], move, 4);
    }
    var test1_matrix = simulate_move(ref, 4, UP);
    var test2_matrix = simulate_move(test1_matrix, 4, LEFT);
    var test3_matrix = simulate_move(test2_matrix, 4, DOWN);
    count += assert_matrix(0, test3_matrix, multi_moves, 0, 4);
    var moves = [DOWN, UP, LEFT, RIGHT];
    var multi_merge_array = [multi_merge_down, multi_merge_up, multi_merge_left, multi_merge_right];
    var curr_mat = multi_merge;
    for (var i=0; i<4; i++)
    {
        var test_matrix = simulate_move(curr_mat, 4, moves[i]);
        count += assert_matrix(curr_mat, test_matrix, multi_merge_array[i], moves[i], 4);
        curr_mat = multi_merge_array[i];
    }
    return count;
}

// Test 
function test_get_best_move_depth_1()
{
    var count = 0;
    var ref1 = [[2, 2, 2, 2], [2, 2, 2, 2], [0, 0, 0, 0], [0, 0, 0, 0]];
    var weight1 = get_weight(get_best_move_aux(ref1, 4, 0)[0], 4);
    if (weight1 != 4)
    {
        console.log("*** Test failed for", ref1, "and depth", 0, "returned weight of", weight1, "instead of", 4);
        count++;
    }
    var weight2 = get_best_move_aux(ref1, 4, 1)[2];
    if (weight2 != 2)
    {
        console.log("*** Test failed for", ref1, "and depth", 1, "returned weight of", weight2, "instead of", 2);
        count++;
    }
    return count;
}

// Test mode (with node.js)
function test()
{
    console.log("### Testing 2048 bot functions");
    var functions = [test_simulate_move, test_get_best_move_depth_1];
    var failed_tests = 0;
    for (var i=0; i<functions.length; i++)
    {
        var curr_failed_tests = functions[i]();
        if (curr_failed_tests != 0)
        {
            console.log("===", curr_failed_tests, "test(s) failed for", function_name(functions[i]));
        }
        else
        {
            console.log("=== All tests for", function_name(functions[i]), "passed");
        }
        failed_tests += curr_failed_tests;
    }
    if (failed_tests != 0)
    {
        console.log("###", failed_tests, "test(s) failed in overall");
    }
    else
    {
        console.log("### All tests passed");
    }
    return failed_tests;
}

// Main function
function main()
{
    // If running with node, this is for tests
    if (typeof process != "undefined")
    {
        var failed_tests = test();
        //process.exit(code = failed_tests);
    }
    else
    {
        run();
    }
}

main();

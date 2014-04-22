// ==UserScript==
// @name        2048 bot
// @namespace   2048_bot
// @description Bot for the game 2048
// @version     0.1.0
// @grant       none
// @include     http://gabrielecirulli.github.io/2048/
// ==/UserScript==

// Notes:
// move codes 0: up, 1: right, 2: down, 3: left

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
    var raw_cells = eval('('+game_manager.storageManager.storage.gameState+')').grid.cells
    refresh_matrix(matrix, raw_cells, size);
}

// Is the game over?
function over(game_manager)
{
    return eval('('+game_manager.storageManager.storage.gameState+')').over;
}

// Get the current best possible move
function get_best_move(matrix)
{
    return Math.floor(Math.random()*4);
}

function main()
{
    console.log("###### Starting 2048 bot ######");
    var size = 4
    var gm = new GameManager(size, KeyboardInputManager, HTMLActuator, LocalStorageManager);
    var grid = generate_matrix(size);
    var i = 0;
    while (!over(gm))
    {
        refresh(grid, gm, size);
        gm.move(get_best_move(grid));
    }
    console.log("###### Stopping 2048 bot ######");
}

main();

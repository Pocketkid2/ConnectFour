#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include <stdbool.h>
#include <stdint.h>

#define NUM_PLAYERS 2
#define NUM_COLUMNS 7
#define NUM_ROWS 6
#define NUM_SLOTS (NUM_COLUMNS * NUM_ROWS)

// Definition for a bitboard
typedef struct
{
    // One bitboard for each player
    uint64_t bitboard[NUM_PLAYERS];

    // Next available row in each column
    uint8_t height[NUM_COLUMNS];

    // How many moves have taken place
    uint8_t counter;

    // A list of moves (columns) that have happened
    uint8_t moves[NUM_SLOTS];

} cf_board_t;

// Definition for game states
typedef enum
{
    // Neither player has won and more moves can be made
    UNFINISHED,

    // The first player (yellow) has won (achieved four in a row)
    YELLOW_WIN,

    // The second player (red) has won (achieved four in a row)
    RED_WIN,

    // Neither player has won but the board is full and no more moves can be made
    STALEMATE

} cf_status_t;

// Maps the enum value to a string for readability
char *map_state_to_str(cf_status_t status);

// Returns the current state of the board
cf_status_t get_board_status(cf_board_t *board);

// Initializes a new, empty board
void init_board(cf_board_t *board);

// Places a token in the column (assumes valid)
void make_move(cf_board_t *board, uint8_t column);

// Returns false if the column requested does not have an open spot
bool move_valid(cf_board_t *board, uint8_t column);

// Prints the board out
void print_board(cf_board_t *board);

// Uses a minimax algorithm to find the best move
uint8_t find_best_move(cf_board_t *board, bool yellow_plays);

#endif
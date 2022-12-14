#include "connectfour.h"

#include <stdio.h>
#include <time.h>

#include "utils.h"

#define PLAYER_YELLOW 0
#define PLAYER_RED 1
#define TOP_BITMASK 0b1000000100000010000001000000100000010000001000000L

#define BASE_SCORE 50
#define INVALID -1

#define DEBUG false

static uint8_t column_choice;
static uint64_t minimax_count;

// Maps the enum value to a string for readability
char *map_state_to_str(cf_status_t status)
{
    switch (status)
    {
    case UNFINISHED:
        return "Unfinished";
    case YELLOW_WIN:
        return "Yellow won";
    case RED_WIN:
        return "Red win";
    case STALEMATE:
        return "Stalemate";
    }
}

// Returns true if this player bitboard has four in a row
static bool has_win(uint64_t bitboard)
{
    return (bitboard & (bitboard >> 1) & (bitboard >> 2) & (bitboard >> 3)) ||
           (bitboard & (bitboard >> 6) & (bitboard >> 12) & (bitboard >> 18)) ||
           (bitboard & (bitboard >> 7) & (bitboard >> 14) & (bitboard >> 21)) ||
           (bitboard & (bitboard >> 8) & (bitboard >> 16) & (bitboard >> 24));
}

// Returns the current state of the board
cf_status_t get_board_status(cf_board_t *board)
{
    if (has_win(board->bitboard[PLAYER_YELLOW]))
    {
        if (DEBUG)
        {
            printf("returning yellow win from get_board_status()\n");
        }
        return YELLOW_WIN;
    }
    if (has_win(board->bitboard[PLAYER_RED]))
    {
        if (DEBUG)
        {
            printf("returning red win from get_board_status()\n");
        }
        return RED_WIN;
    }
    // If all the spaces along the top are occupied, the board is full
    if ((((TOP_BITMASK >> 1) & board->bitboard[PLAYER_YELLOW]) | ((TOP_BITMASK >> 1) & board->bitboard[PLAYER_RED])) == (TOP_BITMASK >> 1))
    {
        if (DEBUG)
        {
            printf("returning stalemate from get_board_status()\n");
        }
        return STALEMATE;
    }
    if (DEBUG)
    {
        printf("returning unfinished from get_board_status()\n");
    }
    return UNFINISHED;
}

// Initializes a new, empty board
void init_board(cf_board_t *board)
{
    // Set the bitboards to empty
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        board->bitboard[i] = 0L;
    }

    // Set the column heights to the bottom
    for (int i = 0; i < NUM_COLUMNS; i++)
    {
        board->height[i] = i * 7;
    }

    // Set the counter to zero
    board->counter = 0;

    // Set the moves array to zero
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        board->moves[i] = 0;
    }
}

// Places a token in the column (assumes valid)
void make_move(cf_board_t *board, uint8_t column)
{
    // Find the bit index we are placing in and increment the next open index for
    // that column at the same time
    uint64_t move = 1L << board->height[column]++;

    // Update the board for whichever color is playing by looking at whether the
    // count is odd or even
    board->bitboard[board->counter & 1] ^= move;

    // Store the move and increment the counter
    board->moves[board->counter++] = column;
}

// Counterpart to make_move, removes the last move that was made
static void undo_move(cf_board_t *board)
{
    // Figure out what column the last move was on by decrementing the counter
    uint8_t column = board->moves[--board->counter];

    // Figure out what bitmask would replicate that move
    uint64_t move = 1L << --board->height[column];

    // Use that replicated bitmask to toggle the move (remove it)
    board->bitboard[board->counter & 1] ^= move;
}

// Returns false if the column requested does not have an open spot
bool move_valid(cf_board_t *board, uint8_t column)
{
    // If the next available row for that column is above the edge of the screen
    // (at the level of the TOP_BITMASK) then it is not valid
    return (column >= 0) && (column < NUM_COLUMNS) && !(TOP_BITMASK & (1L << board->height[column]));
}

// Prints the board out
void print_board(cf_board_t *board)
{
    if (DEBUG)
    {
        printf("Bitboard A (hex): 0x%llX\n", board->bitboard[0]);
        printf("Bitboard B (hex): 0x%llX\n", board->bitboard[1]);
        printf("Bitboard A (bin): %s\n", bitboard_to_str(board->bitboard[0]));
        printf("Bitboard B (bin): %s\n", bitboard_to_str(board->bitboard[1]));
    }

    printf("-------------\n");
    for (int row = NUM_ROWS - 1; row >= 0; row--)
    {
        for (int col = 0; col < NUM_COLUMNS; col++)
        {
            if (board->bitboard[0] & (1L << (row + (col * NUM_COLUMNS))))
            {
                printf("X ");
            }
            else if (board->bitboard[1] & (1L << (row + (col * NUM_COLUMNS))))
            {
                printf("O ");
            }
            else
            {
                printf("- ");
            }
        }
        printf("\n");
    }
    printf("-------------\n");
}

static int8_t minimax(cf_board_t *board, bool yellow_plays, uint8_t depth, uint8_t max_depth)
{
    if (depth > max_depth)
    {
        return INVALID;
    }

    minimax_count++;

    cf_status_t status = get_board_status(board);
    switch (status)
    {
    case STALEMATE:
        return 0;
    case YELLOW_WIN:
        return yellow_plays ? BASE_SCORE * -1 : BASE_SCORE;
    case RED_WIN:
        return yellow_plays ? BASE_SCORE : BASE_SCORE * -1;
    case UNFINISHED:

    {
        int8_t scores[NUM_COLUMNS];
        uint8_t scores_index = 0;

        // Figure out what the score is for all the other boards
        for (uint8_t i = 0; i < NUM_COLUMNS; i++)
        {

            if (move_valid(board, i))
            {
                // Simulate move
                make_move(board, i);

                // Call minimax recursively
                scores[i] = minimax(board, !yellow_plays, depth + 1, max_depth);

                // Undo move
                undo_move(board);
            }
            else
            {
                scores[i] = INVALID;
            }
        }

        if (DEBUG)
        {
            printf("Final score analysis: (depth %d, yellow_plays = %s)\n", depth, yellow_plays ? "yes" : "no");
            printf("Column | Score\n");
            for (int i = 0; i < NUM_COLUMNS; i++)
            {
                printf("%d      | %d\n", i, scores[i]);
            }
        }

        int8_t best_score;

        // Pick the best move based on the scores
        if (yellow_plays)
        {
            int8_t best_score = BASE_SCORE * -1;
            for (uint8_t i = 0; i < NUM_COLUMNS; i++)
            {
                if (scores[i] == INVALID)
                {
                    continue;
                }

                if (scores[i] > best_score)
                {
                    best_score = scores[i];
                    column_choice = i;
                }
            }
        }
        else
        {
            int8_t best_score = BASE_SCORE;
            for (uint8_t i = 0; i < NUM_COLUMNS; i++)
            {
                if (scores[i] == INVALID)
                {
                    continue;
                }

                if (scores[i] < best_score)
                {
                    best_score = scores[i];
                    column_choice = i;
                }
            }
        }

        // Return that score
        return best_score;
    }
    }
}

// Uses a minimax algorithm to find the best move
uint8_t find_best_move(cf_board_t *board, bool yellow_plays, uint8_t num_moves)
{
    // time_t start = time(NULL);
    clock_t start = clock();
    minimax_count = 0;
    minimax(board, yellow_plays, 0, num_moves);
    // time_t end = time(NULL);
    clock_t end = clock();
    printf("Computer analyzed %llu moves\n", minimax_count);
    // printf("Analysis took %.2f seconds\n", difftime(end, start));
    printf("Analysys took %.2f\n", (double)(end - start) / (double)CLOCKS_PER_SEC);
    return column_choice;
}
#include <stdio.h>
#include <stdlib.h>

#include "connectfour.h"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage: %s <h/c> <h/c> <moves>", argv[0]);
        return 0;
    }

    bool player1cpu = false;
    bool player2cpu = false;
    uint8_t moves_ahead = 0;

    if (argv[1][0] == 'h')
    {
        player1cpu = false;
    }
    else if (argv[1][0] == 'c')
    {
        player1cpu = true;
    }
    else
    {
        printf("Unknown option '%c'\n", argv[1][0]);
        return 0;
    }

    if (argv[2][0] == 'h')
    {
        player2cpu = false;
    }
    else if (argv[2][0] == 'c')
    {
        player2cpu = true;
    }
    else
    {
        printf("Unknown option '%c'\n", argv[1][0]);
        return 0;
    }

    moves_ahead = atoi(argv[3]);

    if (moves_ahead == 0) {
        printf("Moves ahead must be greater than zero!\n");
        return 0;
    }

    printf("Welcome to Connect Four! CPU will look %d moves ahead\n", moves_ahead);

    cf_board_t board;

    init_board(&board);

    bool red_turn = false;

    int column;

    while (true)
    {
        while (get_board_status(&board) == UNFINISHED)
        {

            if ((!red_turn && !player1cpu) || (red_turn && !player2cpu))
            {

                printf("%s turn, pick a column: ", red_turn ? "Red" : "Yellow");

                scanf("%d", &column);

                column--;

                if (!move_valid(&board, column))
                {
                    printf("You can't put a token in that column! Try again.\n");
                    continue;
                }
            } else {
                column = find_best_move(&board, !red_turn, moves_ahead);
            }

            make_move(&board, column);

            print_board(&board);

            red_turn = !red_turn;
        }

        printf("Game over! %s!\n\n", map_state_to_str(get_board_status(&board)));

        init_board(&board);

        red_turn = false;
    }
}
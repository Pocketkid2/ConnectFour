#include <stdio.h>

#include "connectfour.h"

int main(int argc, char **argv)
{
    printf("Welcome to Connect Four!\n");

    cf_board_t board;

    init_board(&board);

    bool red_turn = false;

    int column;

    while (true)
    {
        while (get_board_status(&board) == UNFINISHED)
        {

            printf("%s turn, pick a column: ", red_turn ? "Red" : "Yellow");

            scanf("%d", &column);

            column--;

            if (!move_valid(&board, column))
            {
                printf("You can't put a token in that column! Try again.\n");
                continue;
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
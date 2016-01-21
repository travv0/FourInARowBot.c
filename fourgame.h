#ifndef __FOURGAME_H
#define __FOURGAME_H

#include <stdio.h>
#include "gametypes.h"

/* void split(char delimiter, char *input, */

int calc_best_column(Game game);

int get_longest_line(int x, int y, int bot_id, Field field);

int get_potential_longest_line(int x, int y, int bot_id,
		Field field);

int alpha_beta(Game game, Field field, int x, int y, int depth, int alpha,
		int beta);

bool piece_can_be_placed(Field field, int x, int y);

int Evaluate(int bot_id, int winner_id, int prev_x, int prev_y,
		int last_player_id, Field field, Game game);

bool board_is_full(void);

#endif

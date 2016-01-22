#ifndef __FOURGAME_H
#define __FOURGAME_H

#include <stdio.h>

#include "gametypes.h"
#include "fourgame.h"

int can_be_placed(int x, int y, int field[][MAX_FIELD_ROWS]) {
	return TRUE;
}

void get_attack_point_counts(struct Player *red, struct Player *black,
	int field[][MAX_FIELD_ROWS])
{
	int x;
	int y;

	int red_count;
	int black_count;

	for (x = 0; x < game.settings.field_columns; ++x) {
		for (y = 0; y < game.settings.field_rows; ++y) {
			if (field[x][y] == 0 && !can_be_placed(x, y, field)) {
				field[x][y] = red->id;
				red_count = 0; // get_longest_line(x, y, red.id, field);
				field[x][y] = black->id;
				black_count = 0; // get_longest_line(x, y, black.id, field);
				field[x][y] = 0;

				if (red_count >= 4 && black_count >= 4) {
					if (y % 2 == 0) {
						red->attacks.shared_even_count++;
						black->attacks.shared_even_count++;
					} else {
						red->attacks.shared_odd_count++;
						black->attacks.shared_odd_count++;
					}

					break;
				}

				if (red_count >= 4) {
					if (y % 2 == 0)
						red->attacks.shared_even_count++;
					else
						red->attacks.shared_odd_count++;

					break;
				}

				if (black_count >= 4) {
					if (y % 2 == 0)
						black->attacks.shared_even_count++;
					else
						black->attacks.shared_odd_count++;

					break;
				}
			};
		}
	}
}

int calc_best_column(struct Game game)
{
	struct Player me;
	struct Player them;

	me.id = game.settings.your_botid;
	them.id = game.settings.their_botid;

	get_attack_point_counts(&me, &them, game.field);

	return game.settings.field_columns / 2;
}

#endif

#ifndef __FOURGAME_H
#define __FOURGAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "gametypes.h"
#include "fourgame.h"

int min(int i, int j)
{
	return i < j;
}

int max(int i, int j)
{
	return i > j;
}

static int board_full(int rnd)
{
	return rnd > game.settings.field_columns * game.settings.field_rows;
}

static int can_be_placed(int x, int field[][MAX_FIELD_ROWS])
{
	return game.column_fill[x] >= game.settings.field_rows;
}

static int get_longest_line(int x, int y, int botid, int field[][MAX_FIELD_ROWS])
{
	srand((unsigned) time(NULL));

	return rand() % 4;
}

static int alpha_beta(int field[][MAX_FIELD_ROWS], int x, int y, int depth,
		int alpha, int beta, int maximizing_player)
{
	int done = FALSE;
	int their_longest = get_longest_line(x, y, game.settings.their_botid, field);
	int your_longest = get_longest_line(x, y, game.settings.your_botid, field);

	int last_player;

	int chldx;
	int chldy;

	if (depth <= 0 || their_longest >= WIN_LENGTH ||
			your_longest >= WIN_LENGTH) {
		if (maximizing_player)
			last_player = 2;
		else
			last_player = 1;

		if (their_longest >= WIN_LENGTH)
			return -10000; // evaluate function goes here
		else if (your_longest >= WIN_LENGTH)
			return 10000; // evaluate function goes here
		else
			return 0; // evaluate function goes here
	} else if (board_full(game.round + ALPHABETA_LEVEL - depth))
		return 0;

	if (maximizing_player) {
		int v = INT_MIN;

		for (chldx = 0; chldx < game.settings.field_columns; ++chldx) {
			for (chldy = 0; chldy < game.settings.field_rows; ++chldy) {
				if (can_be_placed(chldx, field)) {
					field[chldx][chldy] = game.settings.your_botid;

					v = max(v, alpha_beta(field, chldx, chldy, depth - 1, alpha, beta, FALSE));
					alpha = max(alpha, v);

					field[chldx][chldy] = 0;

					if (beta <= alpha) {
						done = TRUE;
						break;
					}
				}
			}

			if (done)
				break;
		}

		return v;
	} else {
		int v = INT_MAX;

		for (chldx = 0; chldx < game.settings.field_columns; ++chldx) {
			for (chldy = 0; chldy < game.settings.field_rows; ++chldy) {
				if (can_be_placed(chldx, field) && chldy == game.column_fill[chldx] + 1) {
					field[chldx][chldy] = game.settings.their_botid;
					game.column_fill[chldx]++;

					v = min(v, alpha_beta(field, chldx, chldy, depth - 1, alpha, beta, TRUE));
					beta = min(beta, v);

					field[chldx][chldy] = 0;
					game.column_fill[chldx]--;

					if (beta <= alpha) {
						done = TRUE;
						break;
					}
				}
			}

			if (done)
				break;
		}

		return v;
	}
}

/*
 * returns true if it finds an attack point
 */
static int increment_points(struct Player *red, struct Player *black,
		int field[][MAX_FIELD_ROWS], int x, int y)
{
	int red_count;
	int black_count;

	if (field[x][y] == 0 && !can_be_placed(x, field)) {
		field[x][y] = red->id;
		red_count = get_longest_line(x, y, red->id, field);
		field[x][y] = black->id;
		black_count = get_longest_line(x, y, black->id, field);
		field[x][y] = 0;

		if (red_count >= WIN_LENGTH && black_count >= WIN_LENGTH) {
			if (y % 2 == 0) {
				red->attacks.shared_even_count++;
				black->attacks.shared_even_count++;
			} else {
				red->attacks.shared_odd_count++;
				black->attacks.shared_odd_count++;
			}

			return TRUE;
		}

		if (red_count >= WIN_LENGTH) {
			if (y % 2 == 0)
				red->attacks.shared_even_count++;
			else
				red->attacks.shared_odd_count++;

			return TRUE;
		}

		if (black_count >= WIN_LENGTH) {
			if (y % 2 == 0)
				black->attacks.shared_even_count++;
			else
				black->attacks.shared_odd_count++;

			return TRUE;
		}
	};

	return FALSE;
}

static void get_attack_point_counts(struct Player *red, struct Player *black,
		int field[][MAX_FIELD_ROWS])
{
	int x;
	int y;

	for (x = 0; x < game.settings.field_columns; ++x) {
		for (y = 0; y < game.settings.field_rows; ++y) {
			if (increment_points(red, black, field, x, y))
				break;
		}
	}
}

static void fill_column_fill(void)
{
	int x;
	int y;

	for (x = 0; x < game.settings.field_columns; ++x) {
		game.column_fill[x] = 0;
	}

	if (game.round > 1) {
		int count = 0;

		for (x = 0; x < game.settings.field_columns; ++x) {
			for (y = 0; y < game.settings.field_rows; ++y) {
				if (game.field[x][y] != 0) {
					game.column_fill[x]++;
					count++;
				}

				if (count >= game.round - 1)
					break;
			}

			if (count >= game.round - 1)
				break;
		}
	}
}

static int has_advantage(struct Player chk_player, struct Player opp_player)
{
	return (chk_player.id == 1 &&
			(
			 chk_player.attacks.unshared_odd_count == opp_player.attacks.unshared_odd_count + 1 ||
			 (chk_player.attacks.unshared_odd_count == opp_player.attacks.unshared_odd_count &&
			  chk_player.attacks.shared_odd_count % 2 != 0) ||
			 (opp_player.attacks.unshared_odd_count == 0 && (chk_player.attacks.shared_odd_count +
									 chk_player.attacks.unshared_odd_count) % 2 != 0)
			)
	       ) || (chk_player.id == 2 &&
		       (
			(opp_player.attacks.shared_odd_count + opp_player.attacks.unshared_odd_count == 0 &&
			 chk_player.attacks.shared_even_count + chk_player.attacks.unshared_even_count > 0) ||
			chk_player.attacks.unshared_odd_count == opp_player.attacks.unshared_odd_count + 2 ||
			(chk_player.attacks.unshared_odd_count == opp_player.attacks.unshared_odd_count &&
			 opp_player.attacks.shared_odd_count > 0 && (opp_player.attacks.shared_odd_count % 2) == 0) ||
			(chk_player.attacks.unshared_odd_count == opp_player.attacks.unshared_odd_count + 1 &&
			 opp_player.attacks.unshared_odd_count > 0) ||
			(opp_player.attacks.unshared_odd_count == 0 &&
			 chk_player.attacks.unshared_odd_count == 1 &&
			 opp_player.attacks.shared_odd_count > 0) ||
			(opp_player.attacks.unshared_odd_count = 0 &&
			 chk_player.attacks.shared_odd_count + chk_player.attacks.unshared_odd_count > 0 &&
			 (chk_player.attacks.shared_odd_count + chk_player.attacks.unshared_odd_count) % 2 == 0)
		       )
		    );
}

int calc_best_column(struct Game game)
{
	struct Player me;
	struct Player them;

	int col_to_drop = game.settings.field_columns / 2;

	me.id = game.settings.your_botid;
	them.id = game.settings.their_botid;

	fill_column_fill();

	get_attack_point_counts(&me, &them, game.field);

	if (has_advantage(me, them))
		fprintf(stderr, "Your advantage!\n");
	else if (has_advantage(them, me))
		fprintf(stderr, "Their advantage...\n");

	int ordering[MAX_FIELD_COLUMNS];

	int i = game.settings.field_columns / 2;
	int j;

	ordering[0] = i;

	for (j = 1; j < game.settings.field_columns; ++j) {
		if (j % 2 == 0)
			ordering[j] = i + j;
		else
			ordering[j] = i - j;
		i = ordering[j];
	}

	int maxAB = INT_MIN;

	int x;
	int y;

	for (x = 0; x < game.settings.field_columns; ++x) {
		for (y = 0; y < game.settings.field_rows; ++y) {
			int col = ordering[x];

			if (can_be_placed(col, game.field)) {
				game.field[col][y] = game.settings.your_botid;
				int resAB = alpha_beta(game.field, col, y,
						ALPHABETA_LEVEL, INT_MIN, INT_MAX, FALSE);
				game.field[col][y] = 0;

				fprintf(stderr, "(%d, %d): %d\n", col, y, resAB);

				if (resAB > maxAB) {
					maxAB = resAB;
					col_to_drop = col;
				}
			}
		}
	}

	return col_to_drop;
}

#endif

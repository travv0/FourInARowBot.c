#ifndef __FOURGAME_H
#define __FOURGAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "gametypes.h"
#include "fourgame.h"

const struct AttackPoints ATTACK_POINTS_DEFAULT = {
	0, 0, 0, 0
};

int min(int i, int j)
{
	if (i < j)
		return i;
	else
		return j;
}

int max(int i, int j)
{
	if (i > j)
		return i;
	else
		return j;
}

static int board_full(int rnd)
{
	return rnd > game.settings.field_columns * game.settings.field_rows;
}

static int can_be_placed(int x, int y, int field[][MAX_FIELD_ROWS])
{
	return field[x][y] == 0 && (
			y == game.settings.field_rows - 1 ||
			field[x][y + 1] != 0
			);
}

static int get_count_in_dir(int x, int y, int dx, int dy, int botid,
		int field[][MAX_FIELD_ROWS])
{
	int tempx = x;
	int tempy = y;

	int tempx_prev = -1;
	int tempy_prev = -1;

	int count = 0;

	while (TRUE) {
		if (tempx < game.settings.field_columns &&
				tempy < game.settings.field_rows &&
				(tempx != tempx_prev || tempy != tempy_prev)) {
			tempx_prev = tempx;
			tempy_prev = tempy;

			if (field[tempx][tempy] == botid) {
				count++;

				if (tempx > 0 || dx != -1)
					tempx = tempx + dx;
				else
					break;

				if (tempy > 0 || dy != -1)
					tempy = tempy + dy;
				else
					break;
			} else
				break;
		} else
			break;
	}

	return count;
}

static int get_longest_line(int x, int y, int botid, int field[][MAX_FIELD_ROWS])
{
	int max_count = 0;
	int count;

	int i;
	int j;

	for (i = -1; i <= 1; ++i) {
		for (j = -1; j <= 0; ++j) {
			count = 0;

			/* exclude redundant directions */
			if ((i != 0 || j != 0) && (i != 1 || j != 0)) {
				count = get_count_in_dir(x, y, i, j,
						botid, field);

				count += get_count_in_dir(x - i, y - j, -i, -j,
						botid, field);

				if (count > max_count)
					max_count = count;
			}
		}
	}

	/* fprintf(stderr, "%d\n", max_count); */
	return max_count;
}

/*
 * returns true if it finds an attack point
 */
static int increment_points(struct Player *red, struct Player *black,
		int field[][MAX_FIELD_ROWS], int x, int y)
{
	int red_count;
	int black_count;

	if (field[x][y] == 0 && !can_be_placed(x, y, field)) {
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
				red->attacks.unshared_even_count++;
			else
				red->attacks.unshared_odd_count++;

			return TRUE;
		}

		if (black_count >= WIN_LENGTH) {
			if (y % 2 == 0)
				black->attacks.unshared_even_count++;
			else
				black->attacks.unshared_odd_count++;

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
		for (y = game.settings.field_rows - 1; y >= 0; --y) {
			if (increment_points(red, black, field, x, y))
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

static int evaluate(int botid, int winnerid, int lastx, int lasty,
		int last_player, int field[][MAX_FIELD_ROWS])
{
	int modifier = 0;
	int bad_modifier = 0;

	struct Player you;
	struct Player them;

	you.id = game.settings.your_botid;
	them.id = game.settings.their_botid;

	you.attacks = ATTACK_POINTS_DEFAULT;
	them.attacks = ATTACK_POINTS_DEFAULT;

	if (winnerid == game.settings.their_botid)
		return -WIN_SCORE;
	else if (winnerid == game.settings.your_botid)
		return WIN_SCORE;
	else {
		field[lastx][lasty] = 0;
		if (you.id < them.id) /* if you're red */
			get_attack_point_counts(&you, &them, field);
		else /* if they're red */
			get_attack_point_counts(&them, &you, field);
		field[lastx][lasty] = last_player;

		if (has_advantage(them, you))
			bad_modifier = STRATEGY_BONUS;
		else if (has_advantage(you, them))
			modifier = STRATEGY_BONUS;

		// fprintf(stderr, "%d\n", modifier - bad_modifier);

		return modifier - bad_modifier;
	}
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

	int v;

	if (depth <= 0 || their_longest >= WIN_LENGTH ||
			your_longest >= WIN_LENGTH) {
		if (maximizing_player)
			last_player = 2;
		else
			last_player = 1;

		if (their_longest >= WIN_LENGTH)
			return evaluate(game.settings.your_botid,
					game.settings.their_botid,
					x, y, last_player, field)
				+ (ALPHABETA_LEVEL - depth);
		else if (your_longest >= WIN_LENGTH)
			return evaluate(game.settings.your_botid,
					game.settings.your_botid,
					x, y, last_player, field)
				- (ALPHABETA_LEVEL - depth);
		else
			return evaluate(game.settings.your_botid, 0,
					x, y, last_player, field)
				- (ALPHABETA_LEVEL - depth);
	} else if (board_full(game.round + ALPHABETA_LEVEL - depth))
		return 0;

	if (maximizing_player) {
		v = INT_MIN;

		for (chldx = 0; chldx < game.settings.field_columns; ++chldx) {
			for (chldy = 0; chldy < game.settings.field_rows; ++chldy) {
				if (can_be_placed(chldx, chldy, field)) {
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
		v = INT_MAX;

		for (chldx = 0; chldx < game.settings.field_columns; ++chldx) {
			for (chldy = 0; chldy < game.settings.field_rows; ++chldy) {
				if (can_be_placed(chldx, chldy, field)) {
					field[chldx][chldy] = game.settings.their_botid;

					v = min(v, alpha_beta(field, chldx, chldy, depth - 1, alpha, beta, TRUE));
					beta = min(beta, v);

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
	}
}

int calc_best_column(void)
{
	struct Player me;
	struct Player them;

	int col_to_drop = game.settings.field_columns / 2;

	int ordering[MAX_FIELD_COLUMNS];
	int col;

	int i;
	int j;

	int maxAB = INT_MIN;
	int resAB;

	int x;
	int y;

	me.id = game.settings.your_botid;
	them.id = game.settings.their_botid;

	me.attacks = ATTACK_POINTS_DEFAULT;
	them.attacks = ATTACK_POINTS_DEFAULT;

	get_attack_point_counts(&me, &them, game.field);

	fprintf(stderr, "Your shared odd count: %d\n", me.attacks.shared_odd_count);
	fprintf(stderr, "Their shared odd count: %d\n", them.attacks.shared_odd_count);
	fprintf(stderr, "Your shared even count: %d\n", me.attacks.shared_even_count);
	fprintf(stderr, "Their shared even count: %d\n", them.attacks.shared_even_count);
	fprintf(stderr, "Your unshared odd count: %d\n", me.attacks.unshared_odd_count);
	fprintf(stderr, "Their unshared odd count: %d\n", them.attacks.unshared_odd_count);
	fprintf(stderr, "Your unshared even count: %d\n", me.attacks.unshared_even_count);
	fprintf(stderr, "Their unshared even count: %d\n", them.attacks.unshared_even_count);

	if (has_advantage(me, them))
		fprintf(stderr, "Your advantage!\n");
	else if (has_advantage(them, me))
		fprintf(stderr, "Their advantage...\n");

	i = game.settings.field_columns / 2;
	ordering[0] = i;

	for (j = 1; j < game.settings.field_columns; ++j) {
		if (j % 2 == 0)
			ordering[j] = i + j;
		else
			ordering[j] = i - j;
		i = ordering[j];
	}

	for (x = 0; x < game.settings.field_columns; ++x) {
		for (y = 0; y < game.settings.field_rows; ++y) {
			col = ordering[x];

			if (can_be_placed(col, y, game.field)) {

				/* game.field[col][y] = game.settings.your_botid; */
				/* fprintf(stderr, "(%d, %d) Your longest line: %d\n", col, y, get_longest_line(col, y, game.settings.your_botid, game.field)); */
				/* game.field[col][y] = game.settings.their_botid; */
				/* fprintf(stderr, "(%d, %d) Their longest line: %d\n", col, y, get_longest_line(col, y, game.settings.their_botid, game.field)); */
				/* game.field[col][y] = 0; */

				game.field[col][y] = game.settings.your_botid;
				resAB = alpha_beta(game.field, col, y,
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

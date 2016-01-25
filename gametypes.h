#ifndef __GAMETYPES_H
#define __GAMETYPES_H

#define WIN_LENGTH		4
#define WIN_SCORE		100000
#define ALPHABETA_LEVEL		8
#define STRATEGY_BONUS		500
#define POT_STRATEGY_BONUS	100
#define TIMEBANK_LOW		4000

#define TRUE	1
#define FALSE	0

#define MAX_FIELD_COLUMNS	10
#define MAX_FIELD_ROWS		10

struct Settings {
	int		timebank;
	int		time_per_move;
	int		your_botid;
	int		their_botid;
	int		field_columns;
	int		field_rows;
	char		*player_names;
	char		*your_bot;
};

struct Game {
	struct Settings	settings;
	int		round;
	int		field[MAX_FIELD_COLUMNS][MAX_FIELD_ROWS];
	int		time_remaining;
	int		column_fill[MAX_FIELD_COLUMNS];
	int		advantageid;
} game;

struct AttackPoints {
	int	shared_odd_count;
	int	shared_even_count;
	int	unshared_odd_count;
	int	unshared_even_count;
	int	minor_shared_odd_count;
	int	minor_shared_even_count;
	int	minor_unshared_odd_count;
	int	minor_unshared_even_count;
};

struct Player {
	int			id;
	struct AttackPoints	attacks;
};

#endif

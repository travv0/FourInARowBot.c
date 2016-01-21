#ifndef __FOURGAME_H
#define __FOURGAME_H

#include <stdio.h>

#include "gametypes.h"
#include "fourgame.h"

/* void split(char delimiter, char *input, */

int calc_best_column(struct Game game)
{
	return game.settings.field_columns / 2;
}

#endif

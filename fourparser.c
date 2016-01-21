#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "fourparser.h"
#include "fourgame.h"

char *trim(char *str)
{
	char *end;

	while (isspace(*str))
		str++;

	if (*str == 0)
		return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	*(end + 1) = 0;

	return str;
}

void dump_game(const struct Game game)
{

}

void field_from_string(char *str)
{
	int i;
	int j;
	char *token;
	char *end;
	char *line_hold[MAX_FIELD_ROWS];

	for (i = 1; i < MAX_FIELD_ROWS; ++i)
		line_hold[i] = NULL;

	if ((line_hold[0] = strtok(str, ";")) != NULL) {
		for (i = 1; i < MAX_FIELD_COLUMNS; ++i) {
			if ((token = strtok(NULL, ";")) != NULL)
				line_hold[i] = token;
		}
	}

	for (i = 0; i < MAX_FIELD_COLUMNS; ++i) {
		if (line_hold[i] != NULL) {
			token = strtok(line_hold[i], ",");

			for (j = 0; j < MAX_FIELD_ROWS; ++j) {
				if (token != NULL) {
					game.field[i][j] = strtol(token, &end, 10);
				}

				token = strtok(NULL, ",");
			}
		}

		token = strtok(NULL, ";");
	}
}

void parse_input(void)
{
	char *line = NULL;
	size_t size;
	ssize_t len = 0;

	while ((len = getline(&line, &size, stdin)) != -1) {
		char *line_trm = trim(line);

		if (handle_command(line_trm) == 1)
			break;
	}

	free(line);
}

int handle_command(char *cmd)
{
	if (strlen(cmd) <= 0) {
		fprintf(stderr, "No input found.\n");
		return 2;
	}

	char *token;
	char *end;

	if ((token = strtok(cmd, " ")) != NULL) {
		if (strcmp(token, "settings") == 0) {

			if ((token = strtok(NULL, " ")) != NULL) {
				if (strcmp(token, "timebank") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.timebank = strtol(token, &end, 10);
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "time_per_move") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.time_per_move = strtol(token, &end, 10);
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "player_names") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.player_names = token;
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "your_bot") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.your_bot = token;
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "your_botid") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.your_botid = strtol(token, &end, 10);

						game.settings.their_botid = game.settings.your_botid % 2 + 1;
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "field_columns") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.field_columns = strtol(token, &end, 10);

						if (game.settings.field_columns > MAX_FIELD_COLUMNS) {
							fprintf(stderr, "field_columns exceeds max value of %d\n", MAX_FIELD_COLUMNS);
							return 1;
						}
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else if (strcmp(token, "field_rows") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.settings.field_rows = strtol(token, &end, 10);

						if (game.settings.field_rows > MAX_FIELD_ROWS) {
							fprintf(stderr, "field_rows exceeds max value of %d\n", MAX_FIELD_ROWS);
							return 1;
						}
					} else {
						fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
						return 2;
					}

				} else {
					fprintf(stderr, "\"%s\" is not a valid command.\n", token);
					return 2;
				}
			} else {
				fprintf(stderr, "Not enough arguments for command \"settings.\"\n");
				return 2;
			}

		} else if (strcmp(token, "update") == 0) {

			if ((token = strtok(NULL, " ")) != NULL) {
				if (strcmp(token, "game") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						if (strcmp(token, "round") == 0) {
							if ((token = strtok(NULL, " ")) != NULL) {
								game.round = strtol(token, &end, 10);
							} else {
								fprintf(stderr, "Not enough arguments for command \"update.\"\n");
								return 2;
							}
						} else if (strcmp(token, "field") == 0) {
							if ((token = strtok(NULL, " ")) != NULL) {
								field_from_string(token);
							} else {
								fprintf(stderr, "Not enough arguments for command \"update.\"\n");
								return 2;
							}
						} else {
							fprintf(stderr, "\"%s\" is not a valid command.\n", token);
							return 2;
						}
					} else {
						fprintf(stderr, "Not enough arguments for command \"update.\"\n");
						return 2;
					}

				} else {
					fprintf(stderr, "\"%s\" is not a valid command.\n", token);
					return 2;
				}

			} else {
				fprintf(stderr, "Not enough arguments for command \"update.\"\n");
				return 2;
			}

		} else if (strcmp(token, "action") == 0) {

			if ((token = strtok(NULL, " ")) != NULL) {
				if (strcmp(token, "move") == 0) {

					if ((token = strtok(NULL, " ")) != NULL) {
						game.time_remaining = strtol(token, &end, 10);

						printf("place_disc %d\n", calc_best_column(game));
					} else {
						fprintf(stderr, "Not enough arguments for command \"action.\"\n");
						return 2;
					}

				} else {
					fprintf(stderr, "\"%s\" is not a valid command.\n", token);
					return 2;
				}

			} else {
				fprintf(stderr, "Not enough arguments for command \"action.\"\n");
				return 2;
			}

		} else {
			fprintf(stderr, "\"%s\" is not a valid command.\n", token);
			return 2;
		}
	} else {
		fprintf(stderr, "Unable to parse command.\n");
		return 2;
	}

	return 0;
}

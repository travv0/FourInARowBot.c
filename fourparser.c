#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "fourparser.h"
#include "fourgame.h"
#include "mystrlib.h"

void dump_game(void)
{
	int x;
	int y;

	fprintf(stderr, "timebank: %d\n", game.settings.timebank);
	fprintf(stderr, "time_per_move: %d\n", game.settings.time_per_move);
	fprintf(stderr, "player_names: %s\n", game.settings.player_names);
	fprintf(stderr, "your_bot: %s\n", game.settings.your_bot);
	fprintf(stderr, "your_botid: %d\n", game.settings.your_botid);
	fprintf(stderr, "their_botid: %d\n", game.settings.their_botid);
	fprintf(stderr, "field_columns: %d\n", game.settings.field_columns);
	fprintf(stderr, "field_rows: %d\n", game.settings.field_rows);
	fprintf(stderr, "round: %d\n", game.round);

	fprintf(stderr, "field: ");
	for (y = 0; y < game.settings.field_rows; ++y) {
		for (x = 0; x < game.settings.field_columns; ++x) {
			if (y > 0 && x == 0) {
				fprintf(stderr, "       ");
			}

			fprintf(stderr, "%d ", game.field[x][y]);
		}
		fprintf(stderr, "\n");
	}
}

void field_from_string(char *str)
{
	int x;
	int y;

	char *token;
	char *end;
	char *line_hold[MAX_FIELD_ROWS];

	for (y = 1; y < MAX_FIELD_ROWS; ++y)
		line_hold[y] = NULL;

	if ((line_hold[0] = strtok(str, ";")) != NULL) {
		for (y = 1; y < MAX_FIELD_ROWS; ++y) {
			if ((token = strtok(NULL, ";")) != NULL)
				line_hold[y] = token;
		}
	}

	for (y = 0; y < MAX_FIELD_ROWS; ++y) {
		if (line_hold[y] != NULL) {
			token = strtok(line_hold[y], ",");

			for (x = 0; x < MAX_FIELD_COLUMNS; ++x) {
				if (token != NULL) {
					game.field[x][y] = strtol(token, &end, 10);
				}

				token = strtok(NULL, ",");
			}
		}

		token = strtok(NULL, ";");
	}
}

/*
 * handle_command is a big messy function that populates the game struct.
 * returns 0 if no errors, 1 if critical error, and 2 if non-critical error
 */
static int handle_command(char *cmd)
{
	char *token;
	char *end;

	int best_move;

	if (strlen(cmd) <= 0) {
		fprintf(stderr, "No input found.\n");
		return 2;
	}

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

						dump_game();

						best_move = calc_best_column();
						fprintf(stderr, "place_disc %d\n", best_move);
						printf("place_disc %d\n", best_move);
						fflush(stdout);
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

		} else if (strcmp(token, "dump") == 0) {
			dump_game();
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

void parse_input(void)
{
	int size = 1024;
	char *line = (char *) malloc(size * sizeof(char *));
	char *line_trm;

	while (fgets(line, size, stdin)) {
		line_trm = trim(line);

		if (handle_command(line_trm) == 1)
			break;
	}

	free(line);
}

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "fourparser.h"
#include "fourgame.h"
#include "mystrlib.h"

void dump_game()
{
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
	int size = 1024;
	ssize_t len = 0;

	while (fgets(line, size, stdin)) {
		char *line_trm = trim(line);

		if (handle_command(line_trm) == 1)
			break;
	}

	free(line);
}

/*
 * handle_command is a big messy function that populates the game struct.
 * returns 0 if no errors, 1 if critical error, and 2 if non-critical error
 */
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

						dump_game();
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

/******************************************************************************
 * INF 610 - Estruturas de Dados e Algoritmos - 2018/2                        *
 *                                                                            *
 * Hashiwokakero solution. Obvious bridges are added, than a backtracking is  *
 * used to solve the remaining problem.                                       *
 *                                                                            *
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
	int line;
	int column;
	int value; // Original value
	int current_value; // Value after a bridge is attached
} Island;

typedef struct {
	int n, m, n_islands, n_bridges, n_placed_bridges;
	int** board;
	Island* islands;
	int (*neighbour_ids)[4]; // Each island has, at most, 4 neighbours
	int* visitado; // Auxiliary
} Game;

typedef enum {
 	EMPTY = -1,
 	VERTICAL_BRIDGE = -2,
 	DOUBLE_VERTICAL_BRIDGE = -3,
 	HORIZONTAL_BRIDGE = -4,
 	DOUBLE_HORIZONTAL_BRIDGE = -5,
 	BORDER = -6
} State;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;


/*
 * Auxiliary functions
 */
inline int min(int a, int b) {
	return a < b ? a : b;
}

/*
 * Finds each island neighbours for faster queries
 */
void findneighbours(Game* g) {
	int l, c;

	for (int i = 0; i < g->n_islands; i++) {
		// UP
		l = g->islands[i].line;
		c = g->islands[i].column;

		while (g->board[--l][c] < 0 && g->board[l][c] != BORDER);

		g->neighbour_ids[i][UP] = g->board[l][c];

		// DOWN
		l = g->islands[i].line;

		while (g->board[++l][c] < 0 && g->board[l][c] != BORDER);

		g->neighbour_ids[i][DOWN] = g->board[l][c];

		// LEFT
		l = g->islands[i].line;

		while (g->board[l][--c] < 0 && g->board[l][c] != BORDER);

		g->neighbour_ids[i][LEFT] = g->board[l][c];

		// RIGHT
		c = g->islands[i].column;

		while (g->board[l][++c] < 0 && g->board[l][c] != BORDER);

		g->neighbour_ids[i][RIGHT] = g->board[l][c];

#ifndef NDEBUG
		printf("%3d: %3d %3d %3d %3d\n", i, g->neighbour_ids[i][UP],
			g->neighbour_ids[i][DOWN], g->neighbour_ids[i][LEFT],
			g->neighbour_ids[i][RIGHT]);
#endif	// NDEBUG
	}
}

/*
 * Reads input file and creates the game
 */
void create(Game* g) {
	// Board's dimension and number of islands
	scanf("%d %d %d", &g->n, &g->m, &g->n_islands);

	// Board creation
	g->board = (int**)malloc(3 * g->n * sizeof(int*));

	for (int i = 0; i < 3 * g->n; i++) {
		g->board[i] = (int*)malloc(3 * g->m * sizeof(int));

		// Vertical borders
		g->board[i][0] = g->board[i][3 * g->m - 1] = BORDER;

		for (int j = 1; j < 3 * g->m - 1; j++)
			g->board[i][j] = EMPTY;
	}

	// Horizontal borders
	for (int i = 0; i < 3 * g->m; i++)
		g->board[0][i] = g->board[3 * g->n - 1][i] = BORDER;

	// Islands creation
	int x, y;

	g->islands = (Island*)malloc(g->n_islands * sizeof(Island));
	g->neighbour_ids = malloc(g->n_islands * sizeof(int[4]));
	g->visitado = (int*)malloc(g->n_islands * sizeof(int));

	g->n_bridges = g->n_placed_bridges = 0;

	for (int i = 0; i < g->n_islands; i++) {
		scanf("%d %d %d", &x, &y, &g->islands[i].value);

		g->islands[i].line = 3 * (g->n - y - 1) + 1;
		g->islands[i].column = 3 * x + 1;
		g->islands[i].current_value = g->islands[i].value;
		g->n_bridges += g->islands[i].value;

		// Positive numbers in board are the corresponding island id
		g->board[g->islands[i].line][g->islands[i].column] = i;
	}

	g->n_bridges /= 2; // Bridges were counted twice

	findneighbours(g);
}

/*
 * Prints the current instance of the game
 */
void print(Game* g) {
	puts("");

	for (int i = 0; i < g->m; i++)
		printf("*********");
	puts("");

	for (int i = 0; i < 3 * g->n; i++) {
		for (int j = 0; j < 3 * g->m; j++) {
			if (g->board[i][j] == EMPTY)
				printf("   ");
			else if (g->board[i][j] == VERTICAL_BRIDGE)
				printf(" | ");
			else if (g->board[i][j] == DOUBLE_VERTICAL_BRIDGE)
				printf(" D ");
			else if (g->board[i][j] == HORIZONTAL_BRIDGE)
				printf("---");
			else if (g->board[i][j] == DOUBLE_HORIZONTAL_BRIDGE)
				printf("DDD");
			else if (g->board[i][j] == BORDER)
				printf("   ");
			else
				printf("(%d)", g->islands[g->board[i][j]].current_value);
		}

		puts("");
	}

	for (int i = 0; i < g->m; i++)
		printf("*********");

	puts("\n");
}

/*
 * Adds a bridge from 'g->islands[id]' to next island in direction 'dir'.
 * Returns 1 if the bridge was successfully added.
 * Returns 0 if there was an intersection or the destiny island
 * can`t have more bridges.
 */
int addBridge(Game* g, int id, Direction dir) {
	int success = 1, i, previous;

	switch (dir) {
	case UP:
		i = g->islands[id].line;
		previous = g->board[i - 1][g->islands[id].column];

		if (g->board[i - 1][g->islands[id].column] == EMPTY) {
			// If there's no bridge
			while (g->board[--i][g->islands[id].column] == EMPTY)
				g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;
		}
		else if (g->board[i - 1][g->islands[id].column] == VERTICAL_BRIDGE) {
			// If there's one bridge
			while (g->board[--i][g->islands[id].column] == VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = DOUBLE_VERTICAL_BRIDGE;
		}

		// If couldn't add bridge
		if (g->board[i][g->islands[id].column] < 0) {
			success = 0;

			while (i < g->islands[id].line - 1)
				g->board[++i][g->islands[id].column] = previous;
		}
		else {
			// Updates the islands bridge counter
			g->islands[id].current_value--;
			g->islands[g->board[i][g->islands[id].column]].current_value--;
		}

		break;

	case DOWN:
		i = g->islands[id].line;
		previous = g->board[i + 1][g->islands[id].column];

		if (g->board[i + 1][g->islands[id].column] == EMPTY) {
			// If there's no bridge
			while (g->board[++i][g->islands[id].column] == EMPTY)
				g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;
		}
		else if (g->board[i + 1][g->islands[id].column] == VERTICAL_BRIDGE) {
			// If there's one bridge
			while (g->board[++i][g->islands[id].column] == VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = DOUBLE_VERTICAL_BRIDGE;
		}

		// If couldn't add bridge
		if (g->board[i][g->islands[id].column] < 0) {
			success = 0;

			while (i > g->islands[id].line + 1)
				g->board[--i][g->islands[id].column] = previous;
		}
		else {
			// Updates the islands bridge counter
			g->islands[id].current_value--;
			g->islands[g->board[i][g->islands[id].column]].current_value--;
		}

		break;

	case LEFT:
		i = g->islands[id].column;
		previous = g->board[g->islands[id].line][i - 1];

		if (g->board[g->islands[id].line][i - 1] == EMPTY) {
			// If there's no bridge
			while (g->board[g->islands[id].line][--i] == EMPTY)
				g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;
		}
		else if (g->board[g->islands[id].line][i - 1] == HORIZONTAL_BRIDGE) {
			// If there's one bridge
			while (g->board[g->islands[id].line][--i] == HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = DOUBLE_HORIZONTAL_BRIDGE;
		}

		// If couldn't add bridge
		if (g->board[g->islands[id].line][i] < 0) {
			success = 0;

			while (i < g->islands[id].column - 1)
				g->board[g->islands[id].line][++i] = previous;
		}
		else {
			// Updates the islands bridge counter
			g->islands[id].current_value--;
			g->islands[g->board[g->islands[id].line][i]].current_value--;
		}

		break;

	case RIGHT:
		i = g->islands[id].column;
		previous = g->board[g->islands[id].line][i + 1];

		if (g->board[g->islands[id].line][i + 1] == EMPTY) {
			// If there's no bridge
			while (g->board[g->islands[id].line][++i] == EMPTY)
				g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;
		}
		else if (g->board[g->islands[id].line][i + 1] == HORIZONTAL_BRIDGE) {
			// If there's one bridge
			while (g->board[g->islands[id].line][++i] == HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = DOUBLE_HORIZONTAL_BRIDGE;
		}

		// If couldn't add bridge
		if (g->board[g->islands[id].line][i] < 0) {
			success = 0;

			while (i > g->islands[id].column + 1)
				g->board[g->islands[id].line][--i] = previous;
		}
		else {
			// Updates the islands bridge counter
			g->islands[id].current_value--;
			g->islands[g->board[g->islands[id].line][i]].current_value--;
		}
	}

	if (success)
		g->n_placed_bridges++;

	return success;
}

/*
 * Removes the bridge from 'g->islands[id]' to next island in direction 'dir'.
 * Assumes the bridge exists.
 */
void removeBridge(Game* g, int id, Direction dir) {
	int i;

	// This island can support a new bridge now
	g->islands[id].current_value++;

	switch (dir) {
	case UP:
		i = g->islands[id].line;

		if (g->board[i - 1][g->islands[id].column] == VERTICAL_BRIDGE)
			while (g->board[--i][g->islands[id].column] == VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = EMPTY;
		else
			while (g->board[--i][g->islands[id].column] ==
					DOUBLE_VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;

		// This island can support a new bridge now
		g->islands[g->neighbour_ids[id][UP]].current_value++;

		break;

	case DOWN:
		i = g->islands[id].line;

		if (g->board[i + 1][g->islands[id].column] == VERTICAL_BRIDGE)
			while (g->board[++i][g->islands[id].column] == VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = EMPTY;
		else
			while (g->board[++i][g->islands[id].column] ==
					DOUBLE_VERTICAL_BRIDGE)
				g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;

		// This island can support a new bridge now
		g->islands[g->neighbour_ids[id][DOWN]].current_value++;

		break;

	case LEFT:
		i = g->islands[id].column;

		if (g->board[g->islands[id].line][i - 1] == HORIZONTAL_BRIDGE)
			while (g->board[g->islands[id].line][--i] == HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = EMPTY;
		else
			while (g->board[g->islands[id].line][--i] ==
					DOUBLE_HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;

		// This island can support a new bridge now
		g->islands[g->neighbour_ids[id][LEFT]].current_value++;

		break;

	case RIGHT:
		i = g->islands[id].column;

		if (g->board[g->islands[id].line][i + 1] == HORIZONTAL_BRIDGE)
			while (g->board[g->islands[id].line][++i] == HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = EMPTY;
		else
			while (g->board[g->islands[id].line][++i] ==
					DOUBLE_HORIZONTAL_BRIDGE)
				g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;

		// This island can support a new bridge now
		g->islands[g->neighbour_ids[id][RIGHT]].current_value++;
	}

	g->n_placed_bridges--;
}

/*
 * Procedure of 'checkIfIsolated' function.
 * Returns the number of visited islands.
 */
int isolatedProcedure(Game* g, int id) {
	g->visitado[id] = g->islands[id].current_value + 1;

	int sum = 0;

	int l = g->islands[id].line;
	int c = g->islands[id].column;

	// UP
	if (g->neighbour_ids[id][UP] != BORDER &&
		(g->board[l - 1][c] == VERTICAL_BRIDGE ||
		g->board[l - 1][c] == DOUBLE_VERTICAL_BRIDGE) &&
		g->visitado[g->neighbour_ids[id][UP]] == 0)
		sum += isolatedProcedure(g, g->neighbour_ids[id][UP]);

	// DOWN
	if (g->neighbour_ids[id][DOWN] != BORDER &&
		(g->board[l + 1][c] == VERTICAL_BRIDGE ||
		g->board[l + 1][c] == DOUBLE_VERTICAL_BRIDGE) &&
		g->visitado[g->neighbour_ids[id][DOWN]] == 0)
		sum += isolatedProcedure(g, g->neighbour_ids[id][DOWN]);

	// LEFT
	if (g->neighbour_ids[id][LEFT] != BORDER &&
		(g->board[l][c - 1] == HORIZONTAL_BRIDGE ||
		g->board[l][c - 1] == DOUBLE_HORIZONTAL_BRIDGE) &&
		g->visitado[g->neighbour_ids[id][LEFT]] == 0)
		sum += isolatedProcedure(g, g->neighbour_ids[id][LEFT]);

	// RIGHT
	if (g->neighbour_ids[id][RIGHT] != BORDER &&
		(g->board[l][c + 1] == HORIZONTAL_BRIDGE ||
		g->board[l][c + 1] == DOUBLE_HORIZONTAL_BRIDGE) &&
		g->visitado[g->neighbour_ids[id][RIGHT]] == 0)
		sum += isolatedProcedure(g, g->neighbour_ids[id][RIGHT]);

	return sum + 1;
}

/*
 * Checks if the subgraph with g->islands[id] forms an isolated component
 * Returns 1 if true, 0 otherwise.
 */
int isolated(Game* g, int id) {
	for (int i = 0; i < g->n_islands; i++)
		g->visitado[i] = 0;

	int n_visited = isolatedProcedure(g, id);

	if (n_visited == g->n_islands)
		return 0;

	for (int i = 0; i < g->n_islands; i++)
		if (g->visitado[i] > 1)
			return 0;

	return 1;
}

/*
 * Checks if island needs more bridges and there are neighbours which
 * can have more bridges attached.
 * Returns 1 if possible, 0 otherwise.
 */
int numberOfBridgesCanBeSatisfied(Game* g, int id) {
	int l = g->islands[id].line;
	int c = g->islands[id].column;

	int sum = 0;

	// UP
	if (g->neighbour_ids[id][UP] != BORDER) {
		if (g->board[l - 1][c] == EMPTY)
			sum += min(2,
				g->islands[g->neighbour_ids[id][UP]].current_value);
		else if (g->board[l - 1][c] == VERTICAL_BRIDGE)
			sum += min(1,
				g->islands[g->neighbour_ids[id][UP]].current_value);
	}

	// DOWN
	if (g->neighbour_ids[id][DOWN] != BORDER) {
		if (g->board[l + 1][c] == EMPTY)
			sum += min(2,
				g->islands[g->neighbour_ids[id][DOWN]].current_value);
		else if (g->board[l + 1][c] == VERTICAL_BRIDGE)
			sum += min(1,
				g->islands[g->neighbour_ids[id][DOWN]].current_value);
	}

	// LEFT
	if (g->neighbour_ids[id][LEFT] != BORDER) {
		if (g->board[l][c - 1] == EMPTY)
			sum += min(2,
				g->islands[g->neighbour_ids[id][LEFT]].current_value);
		else if (g->board[l][c - 1] == HORIZONTAL_BRIDGE)
			sum += min(1,
				g->islands[g->neighbour_ids[id][LEFT]].current_value);
	}

	// RIGHT
	if (g->neighbour_ids[id][RIGHT] != BORDER) {
		if (g->board[l][c + 1] == EMPTY)
			sum += min(2,
				g->islands[g->neighbour_ids[id][RIGHT]].current_value);
		else if (g->board[l][c + 1] == HORIZONTAL_BRIDGE)
			sum += min(1,
				g->islands[g->neighbour_ids[id][RIGHT]].current_value);
	}

	if (sum >= g->islands[id].current_value)
		return 1;
	else
		return 0;
}

/*
 * Backtracking. Returns 1 when done.
 */
int backtracking(Game* g, int id) {
	for (int i = 0; i < g->n_islands; i++)
		if (numberOfBridgesCanBeSatisfied(g, i) == 0)
			// Impossible to continue
			return 0;

	if (g->n_bridges == g->n_placed_bridges) // All bridges are set
		return 1;

	int result;

	int l = g->islands[id].line;
	int c = g->islands[id].column;

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][UP] != BORDER &&
		g->islands[g->neighbour_ids[id][UP]].current_value > 0 &&
		g->board[l - 1][c] != DOUBLE_VERTICAL_BRIDGE) {
		result = addBridge(g, id, UP);

		if (result == 1) {
			if (isolated(g, id) == 0) {
				// If edge didn't make a isolated component
#ifndef NDEBUG
				print(g);
#endif	// NDEBUG

				if (backtracking(g, id) == 1)
					return 1;
			}

			removeBridge(g, id, UP);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][DOWN] != BORDER &&
		g->islands[g->neighbour_ids[id][DOWN]].current_value > 0 &&
		g->board[l + 1][c] != DOUBLE_VERTICAL_BRIDGE) {
		result = addBridge(g, id, DOWN);

		if (result == 1) {
			if (isolated(g, id) == 0) {
				// If edge didn't make a isolated component
#ifndef NDEBUG
				print(g);
#endif	// NDEBUG
				
				if (backtracking(g, id) == 1)
					return 1;
			}

			removeBridge(g, id, DOWN);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][LEFT] != BORDER &&
		g->islands[g->neighbour_ids[id][LEFT]].current_value > 0 &&
		g->board[l][c - 1] != DOUBLE_HORIZONTAL_BRIDGE) {
		result = addBridge(g, id, LEFT);

		if (result == 1) {
			if (isolated(g, id) == 0) {
				// If edge didn't make a isolated component
#ifndef NDEBUG
				print(g);
#endif	// NDEBUG

				if (backtracking(g, id) == 1)
					return 1;
			}

			removeBridge(g, id, LEFT);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][RIGHT] != BORDER &&
		g->islands[g->neighbour_ids[id][RIGHT]].current_value > 0 &&
		g->board[l][c + 1] != DOUBLE_HORIZONTAL_BRIDGE) {
		result = addBridge(g, id, RIGHT);

		if (result == 1) {
			if (isolated(g, id) == 0) {
				// If edge didn't make a isolated component	
#ifndef NDEBUG
				print(g);
#endif	// NDEBUG

				if (backtracking(g, id) == 1)
					return 1;
			}

			removeBridge(g, id, RIGHT);
		}
	}

	if (id < g->n_islands - 1)
		return backtracking(g, id + 1);

	return 0;
}

/*
 * Adds all obvious bridges to the board.
 * Fon instance, is an island needs 8 bridges, it must have 2 bridges
 * in all four directions.
 */
void addObviousBridges(Game* g) {
	// Number islands which had no bridge addition per iteration
	int not_modified;

	do {
		not_modified = 0;

		for (int i = 0; i < g->n_islands; i++) {
			switch (g->islands[i].current_value) {
				case 1:
					if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER)
						addBridge(g, i, RIGHT);
					else if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER)
						addBridge(g, i, LEFT);
					else if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER)
						addBridge(g, i, DOWN);
					else if (g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER)
						addBridge(g, i, UP);
					else
						not_modified++;

					break;

				case 2:
					if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER) {
						addBridge(g, i, RIGHT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, LEFT);
						addBridge(g, i, LEFT);
					}
					else if (g->neighbour_ids[i][UP] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, DOWN);
						addBridge(g, i, DOWN);
					}
					else if (g->neighbour_ids[i][DOWN] == BORDER &&
						g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, UP);
					}
					else
						not_modified++;

					break;

				case 3:
					if (g->neighbour_ids[i][UP] == BORDER) {
						if (g->neighbour_ids[i][DOWN] == BORDER) {
							addBridge(g, i, LEFT);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][LEFT] == BORDER) {
							addBridge(g, i, DOWN);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][RIGHT] == BORDER) {
							addBridge(g, i, DOWN);
							addBridge(g, i, LEFT);
						}
						else
							not_modified++;
					}
					else if (g->neighbour_ids[i][DOWN] == BORDER) {
						if (g->neighbour_ids[i][LEFT] == BORDER) {
							addBridge(g, i, UP);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][RIGHT] == BORDER) {
							addBridge(g, i, UP);
							addBridge(g, i, LEFT);
						}
						else
							not_modified++;
					}					
					else if (g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
					}
					else
						not_modified++;

					break;

				case 4:
					if (g->neighbour_ids[i][UP] == BORDER) {
						if (g->neighbour_ids[i][DOWN] == BORDER) {
							addBridge(g, i, LEFT);
							addBridge(g, i, LEFT);
							addBridge(g, i, RIGHT);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][LEFT] == BORDER) {
							addBridge(g, i, DOWN);
							addBridge(g, i, DOWN);
							addBridge(g, i, RIGHT);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][RIGHT] == BORDER) {
							addBridge(g, i, DOWN);
							addBridge(g, i, DOWN);
							addBridge(g, i, LEFT);
							addBridge(g, i, LEFT);
						}
						else
							not_modified++;
					}
					else if (g->neighbour_ids[i][DOWN] == BORDER) {
						if (g->neighbour_ids[i][LEFT] == BORDER) {
							addBridge(g, i, UP);
							addBridge(g, i, UP);
							addBridge(g, i, RIGHT);
							addBridge(g, i, RIGHT);
						}
						else if (g->neighbour_ids[i][RIGHT] == BORDER) {
							addBridge(g, i, UP);
							addBridge(g, i, UP);
							addBridge(g, i, LEFT);
							addBridge(g, i, LEFT);
						}
						else
							not_modified++;
					}					
					else if (g->neighbour_ids[i][LEFT] == BORDER &&
						g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
						addBridge(g, i, DOWN);
					}
					else
						not_modified++;

					break;

				case 5:
					if (g->neighbour_ids[i][UP] == BORDER) {
						addBridge(g, i, DOWN);
						addBridge(g, i, LEFT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][DOWN] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, LEFT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][LEFT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
						addBridge(g, i, LEFT);
					}
					else
						not_modified++;

					break;

				case 6:
					if (g->neighbour_ids[i][UP] == BORDER) {
						addBridge(g, i, DOWN);
						addBridge(g, i, DOWN);
						addBridge(g, i, LEFT);
						addBridge(g, i, LEFT);
						addBridge(g, i, RIGHT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][DOWN] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, UP);
						addBridge(g, i, LEFT);
						addBridge(g, i, LEFT);
						addBridge(g, i, RIGHT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][LEFT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
						addBridge(g, i, DOWN);
						addBridge(g, i, RIGHT);
						addBridge(g, i, RIGHT);
					}
					else if (g->neighbour_ids[i][RIGHT] == BORDER) {
						addBridge(g, i, UP);
						addBridge(g, i, UP);
						addBridge(g, i, DOWN);
						addBridge(g, i, DOWN);
						addBridge(g, i, LEFT);
						addBridge(g, i, LEFT);
					}
					else
						not_modified++;

					break;

				case 7:
					addBridge(g, i, UP);
					addBridge(g, i, DOWN);
					addBridge(g, i, LEFT);
					addBridge(g, i, RIGHT);

					break;

				case 8:
					addBridge(g, i, UP);
					addBridge(g, i, UP);
					addBridge(g, i, DOWN);
					addBridge(g, i, DOWN);
					addBridge(g, i, LEFT);
					addBridge(g, i, LEFT);
					addBridge(g, i, RIGHT);
					addBridge(g, i, RIGHT);

					break;

				default:
					not_modified++;
			}
		}

#ifndef NDEBUG
		printf("%d\n", not_modified);
		print(g);
#endif	// NDEBUG
	}
	while (not_modified < g->n_islands);
}

/*
 * Solves the game.
 */
void play(Game* g) {
	addObviousBridges(g);
	puts("\nObvious bridges added\n"); // To find in solution with 'ctrl + f'
	backtracking(g, 0);
}

/*
 * Frees memory
 */
void clear(Game* g) {
	for (int i = 0; i < g->n; i++)
		free(g->board[i]);

	free(g->board);
	free(g->islands);
	free(g->neighbour_ids);
	free(g->visitado);
}


int main() {
	Game game;

	create(&game);
	print(&game);

#if defined (NDEBUG)
	clock_t start, end;
	start = clock();
#endif	// NDEBUG

	play(&game);

#if defined (NDEBUG)
	end = clock();

	printf("Time taken: %lf\n", ((double)(end - start)) / CLOCKS_PER_SEC);
	printf("Bridges placed: %d/%d\n",
		game.n_placed_bridges, game.n_bridges);
#endif	// NDEBUG

	print(&game);
	clear(&game);

	puts("\nDone");

	return 0;
}
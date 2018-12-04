/******************************************************************************
 * INF 610 - Estruturas de Dados e Algoritmos - 2018/2                        *
 *                                                                            *
 * Hashiwokakero brute-force/backtracking solution.                           *
 *                                                                            *
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>


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
	int (*neighbour_ids)[4]; // Each island has, at most, 4 neighbors
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

inline int max(int a, int b) {
	return a > b ? a : b;
}

/*
 * Finds each island neighbors for faster queries
 */
void findNeighbors(Game* g) {
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

		printf("%3d: %3d %3d %3d %3d\n", i, g->neighbour_ids[i][UP],
			g->neighbour_ids[i][DOWN], g->neighbour_ids[i][LEFT],
			g->neighbour_ids[i][RIGHT]);
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

	findNeighbors(g);
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
		else if (g->board[i - 1][g->islands[id].column] ==
				DOUBLE_VERTICAL_BRIDGE) {
			success = 0;
			break;
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
		else if (g->board[i + 1][g->islands[id].column] ==
				DOUBLE_VERTICAL_BRIDGE) {
			success = 0;
			break;
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
		else if (g->board[g->islands[id].line][i - 1] ==
				DOUBLE_HORIZONTAL_BRIDGE) {
			success = 0;
			break;
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
		else if (g->board[g->islands[id].line][i + 1] ==
				DOUBLE_HORIZONTAL_BRIDGE) {
			success = 0;
			break;
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
 * Procedure of 'checkIfIsolated' function
 */
int isolatedProcedure(Game* g, int id) {
	if (g->visitado[id] == 1)
		return 1;

	g->visitado[id] = 1;

	int possible = 0;

	if (g->islands[id].current_value > 0) {
		for (int i = 0; i < 4; i++) {
			if (g->neighbour_ids[id][i] != BORDER &&
				g->islands[g->neighbour_ids[id][i]].current_value > 0) {
				possible = 1;
				break;
			}
		}

		if (possible == 1) // Not isolated
			return 0;
	}

	for (int i = 0; i < 4; i++)
		if (g->neighbour_ids[id][i] != BORDER &&
			isolatedProcedure(g, g->neighbour_ids[id][i]) == 0)
			return 0;

	return 1;
}

/*
 * Checks if the subgraph with g->islands[id] forms an isolated component
 * Returns 1 if true, 0 otherwise.
 */
int isolated(Game* g, int id) {
	for (int i = 0; i < g->n_islands; i++)
		g->visitado[i] = 0;

	if (isolatedProcedure(g, id) == 1) {
		for (int i = 0; i < g->n_islands; i++) {
printf("%d ", g->visitado[i]);
			if (g->visitado[i] == 0)
				return 1;
		}
puts("");
	}

	return 0;
}

/*
 * Checks if island needs more bridges and there are neighbors which
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

printf("%d %d ", sum, g->islands[id].current_value);

	if (sum >= g->islands[id].current_value)
		return 1;
	else
		return 0;
}

/*
 * Solves the game
 */
void play(Game* g, int id) {
	if (numberOfBridgesCanBeSatisfied(g, id) == 0) {
puts("DEU RUIM");
		return;
	}

	int result;

	int l = g->islands[id].line;
	int c = g->islands[id].column;

printf("%d ", id);

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][UP] != BORDER &&
		g->islands[g->neighbour_ids[id][UP]].current_value > 0 &&
		g->board[l - 1][c] != DOUBLE_VERTICAL_BRIDGE) {
		result = addBridge(g, id, UP);
printf("UP %d", result);

		if (result == 1) {
			//if (isolated(g, id) == 0) {
			{
				// If edge didn't make a isolated component
				print(g);
				play(g, id);
			}

			removeBridge(g, id, UP);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][DOWN] != BORDER &&
		g->islands[g->neighbour_ids[id][DOWN]].current_value > 0 &&
		g->board[l + 1][c] != DOUBLE_VERTICAL_BRIDGE) {
		result = addBridge(g, id, DOWN);
printf("DOWN %d", result);

		if (result == 1) {
			//if (isolated(g, id) == 0) {
			{
				// If edge didn't make a isolated component
				print(g);
				play(g, id);
			}

			removeBridge(g, id, DOWN);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][LEFT] != BORDER &&
		g->islands[g->neighbour_ids[id][LEFT]].current_value > 0 &&
		g->board[l][c - 1] != DOUBLE_HORIZONTAL_BRIDGE) {
		result = addBridge(g, id, LEFT);
printf("LEFT %d", result);

		if (result == 1) {
			//if (isolated(g, id) == 0) {
			{
				// If edge didn't make a isolated component
				print(g);
				play(g, id);
			}

			removeBridge(g, id, LEFT);
		}
	}

	if (g->islands[id].current_value > 0 &&
		g->neighbour_ids[id][RIGHT] != BORDER &&
		g->islands[g->neighbour_ids[id][RIGHT]].current_value > 0 &&
		g->board[l][c + 1] != DOUBLE_HORIZONTAL_BRIDGE) {
		result = addBridge(g, id, RIGHT);
printf("RIGHT %d", result);

		if (result == 1) {
			//if (isolated(g, id) == 0) {
			{
				// If edge didn't make a isolated component
				print(g);
				play(g, id);
			}

			removeBridge(g, id, RIGHT);
		}
	}

	if (id < g->n_islands - 1)
		play(g, id + 1);
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
/*
	addBridge(&game, 9, UP);
	addBridge(&game, 9, DOWN);
	addBridge(&game, 9, RIGHT);
	addBridge(&game, 12, UP);
	addBridge(&game, 9, LEFT);

	print(&game);

	removeBridge(&game, 9, UP);
	removeBridge(&game, 9, DOWN);
	removeBridge(&game, 9, RIGHT);
	removeBridge(&game, 12, UP);
	removeBridge(&game, 9, LEFT);

	addBridge(&game, 9, UP);
	print(&game);
	addBridge(&game, 9, UP);
	print(&game);
	addBridge(&game, 9, DOWN);
	print(&game);
	addBridge(&game, 9, DOWN);
	print(&game);
	addBridge(&game, 9, LEFT);
	print(&game);
	addBridge(&game, 9, LEFT);
	print(&game);
*/
	play(&game, 0);

	clear(&game);

	return 0;
}
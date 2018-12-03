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
	int n, m, n_islands;
	int** board;
	Island* islands;
	int (*neighbour_ids)[4]; // Each island has, at most, 4 neighbors
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

	for (int i = 0; i < g->n_islands; i++) {
		scanf("%d %d %d", &x, &y, &g->islands[i].value);

		g->islands[i].line = 3 * (g->n - y - 1) + 1;
		g->islands[i].column = 3 * x + 1;
		g->islands[i].current_value = g->islands[i].value;

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
				printf("(%d)", g->islands[g->board[i][j]].value);
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
	int success = 1, i;

	switch (dir) {
	case UP:
		i = g->islands[id].line;

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
				g->board[++i][g->islands[id].column] = -1;
		}

		break;

	case DOWN:
		i = g->islands[id].line;

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
				g->board[--i][g->islands[id].column] = -1;
		}

		break;

	case LEFT:
		i = g->islands[id].column;

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
				g->board[g->islands[id].line][++i] = -1;
		}

		break;

	case RIGHT:
		i = g->islands[id].column;

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
				g->board[g->islands[id].line][--i] = -1;
		}
	}

	return success;
}

/*
 * Removes the bridge from 'g->islands[id]' to next island in direction 'dir'.
 * Assumes the bridge exists.
 */
void removeBridge(Game* g, int id, Direction dir) {
	int i;

	switch (dir) {
	case UP:
		i = g->islands[id].line;

		while (g->board[--i][g->islands[id].column] == VERTICAL_BRIDGE)
			g->board[i][g->islands[id].column] = EMPTY;

		break;

	case DOWN:
		i = g->islands[id].line;

		while (g->board[++i][g->islands[id].column] == VERTICAL_BRIDGE)
			g->board[i][g->islands[id].column] = EMPTY;

		break;

	case LEFT:
		i = g->islands[id].column;

		while (g->board[g->islands[id].line][--i] == HORIZONTAL_BRIDGE)
			g->board[g->islands[id].line][i] = EMPTY;

		break;

	case RIGHT:
		i = g->islands[id].column;

		while (g->board[g->islands[id].line][++i] == HORIZONTAL_BRIDGE)
			g->board[g->islands[id].line][i] = EMPTY;
	}
}

/*
 * Solves the game
 */
void play(Game* g, int id) {
	if (id >= g->n_islands)
		return;

	int a;

	a = addBridge(g, id, UP);
	print(g);
	printf("%d %s\n", id, a ? "UP S" : "UP N");

	a = addBridge(g, id, DOWN);
	print(g);
	printf("%d %s\n", id, a ? "DOWN S" : "DOWN N");

	a = addBridge(g, id, LEFT);
	print(g);
	printf("%d %s\n", id, a ? "LEFT S" : "LEFT N");
	
	a = addBridge(g, id, RIGHT);
	print(g);
	printf("%d %s\n", id, a ? "RIGHT S" : "RIGHT N");

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
*/
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

//	play(&game, 0);

	clear(&game);

	return 0;
}
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
	int value;
} Island;

typedef struct {
	int n, m;
	int** board;
	Island* islands;
} Game;

typedef enum {
 	EMPTY = -1,
 	VERTICAL_BRIDGE = -2,
 	HORIZONTAL_BRIDGE = -3,
 	BORDER = -4
} State;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;


/*
 * Reads input file and creates the game
 */
void create(Game* g) {
	int n_islands;

	// Board's dimension and number of islands
	scanf("%d %d %d", &g->n, &g->m, &n_islands);

	g->board = (int**)malloc(3 * g->n * sizeof(int*));

	for (int i = 0; i < 3 * g->n; i++) {
		g->board[i] = (int*)malloc(3 * g->m * sizeof(int));
		for (int j = 0; j < 3 * g->m; j++)
			g->board[i][j] = EMPTY;
	}

	g->islands = (Island*)malloc(n_islands * sizeof(Island));

	// Border
	for (int i = 0; i < 3 * g->n; i++)
		g->board[i][0] = g->board[i][3 * g->m - 1] = BORDER;

	for (int i = 0; i < 3 * g->m; i++)
		g->board[0][i] = g->board[3 * g->n - 1][i] = BORDER;

	// Islands
	int x, y;

	for (int i = 0; i < n_islands; i++) {
		scanf("%d %d %d", &x, &y, &g->islands[i].value);

		g->islands[i].line = 3 * (g->n - y - 1) + 1;
		g->islands[i].column = 3 * x + 1;

		g->board[g->islands[i].line][g->islands[i].column] =
			g->islands[i].value;
	}
}

/*
 * Prints the current instance of the game
 */
void print(Game* g) {
	puts("");

	for (int i = 0; i < 3 * g->n; i++) {
		for (int j = 0; j < 3 * g->m; j++) {
			if (g->board[i][j] == EMPTY)
				printf("   ");
			else if (g->board[i][j] == VERTICAL_BRIDGE)
				printf(" | ");
			else if (g->board[i][j] == HORIZONTAL_BRIDGE)
				printf("---");
			else if (g->board[i][j] == BORDER)
				printf(" * ");
			else
				printf("(%d)", g->board[i][j]);
		}

		puts("");
	}

	puts("");
}

/*
 * Adds a bridge from 'g->islands[id]' to next island in direction 'dir'.
 * Returns 1 if the bridge was successfully added.
 * Returns 0 if there was an intersection.
 */
int addBridge(Game* g, int id, Direction dir) {
	int success = 1, i;

	switch (dir) {
	case UP:
		i = g->islands[id].line;

		while (g->board[--i][g->islands[id].column] == EMPTY)
			g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;

		// If couldn't add bridge
		if (g->board[i][g->islands[id].column] <= 0) {
			success = 0;

			while (i < g->islands[id].line - 1)
				g->board[++i][g->islands[id].column] = -1;
		}

		break;

	case DOWN:
		i = g->islands[id].line;

		while (g->board[++i][g->islands[id].column] == EMPTY)
			g->board[i][g->islands[id].column] = VERTICAL_BRIDGE;

		// If couldn't add bridge
		if (g->board[i][g->islands[id].column] <= 0) {
			success = 0;

			while (i > g->islands[id].line + 1)
				g->board[--i][g->islands[id].column] = -1;
		}

		break;

	case LEFT:
		i = g->islands[id].column;

		while (g->board[g->islands[id].line][--i] == EMPTY)
			g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;

		// If couldn't add bridge
		if (g->board[g->islands[id].line][i] <= 0) {
			success = 0;

			while (i < g->islands[id].line - 1)
				g->board[g->islands[id].line][++i] = -1;
		}

		break;

	case RIGHT:
		i = g->islands[id].column;

		while (g->board[g->islands[id].line][++i] == EMPTY)
			g->board[g->islands[id].line][i] = HORIZONTAL_BRIDGE;

		// If couldn't add bridge
		if (g->board[g->islands[id].line][i] <= 0) {
			success = 0;

			while (i > g->islands[id].line + 1)
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
void play(Game* g) {

}

/*
 * Frees memory
 */
void clear(Game* g) {
	for (int i = 0; i < g->n; i++)
		free(g->board[i]);

	free(g->board);
}


int main() {
	Game game;

	create(&game);

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

	print(&game);

	clear(&game);

	return 0;
}
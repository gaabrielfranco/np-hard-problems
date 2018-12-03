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

enum State {
 	NO_ISLAND = -1,
 	BRIDGE = -2,
 	BORDER = -3
};

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};


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
			g->board[i][j] = NO_ISLAND;
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
			if (g->board[i][j] == NO_ISLAND)
				printf("   ");
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
 * Adds a bridge from point (x, y)
bool addBridge(Game* game, int x, int y, Direction dir) {

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
	print(&game);
	clear(&game);

	return 0;
}
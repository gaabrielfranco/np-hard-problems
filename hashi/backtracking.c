#include <stdio.h>
#include <stdlib.h>


typedef struct {
	int n, m;
	int** board;
} Game;

enum State {
 	NO_ISLAND = -1,
 	BRIDGE = -2
};


/*
 * Reads input file and creates the game
 */
void create(Game* game) {
	int n_islands;

	// Board's dimension and number of islands
	scanf("%d %d %d", &game->n, &game->m, &n_islands);

	game->board = (int**)malloc(3 * game->n * sizeof(int*));

	for (int i = 0; i < 3 * game->n; i++) {
		game->board[i] = (int*)malloc(3 * game->m * sizeof(int));
		for (int j = 0; j < 3 * game->m; j++)
			game->board[i][j] = NO_ISLAND;
	}

	// Island's coordinates and value
	int x, y, v;

	for (int i = 0; i < n_islands; i++) {
		scanf("%d %d %d", &x, &y, &v);
		game->board[3 * (game->n - y - 1) + 1][3 * x + 1] = v;
	}
}

/*
 * Prints the current instance of the game
 */
void print(Game* game) {
	for (int i = 0; i < 3 * game->n; i++) {
		for (int j = 0; j < 3 * game->m; j++) {
			if (game->board[i][j] != NO_ISLAND)
				printf("(%d)", game->board[i][j]);
			else
				printf("   ");
		}

		puts("");
	}
}

/*
 * Frees memory
 */
void clear(Game* game) {
	for (int i = 0; i < game->n; i++)
		free(game->board[i]);

	free(game->board);
}


int main() {
	Game game;

	create(&game);
	print(&game);
	clear(&game);

	return 0;
}
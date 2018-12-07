/******************************************************************************
 * INF 610 - Estruturas de Dados e Algoritmos - 2018/2                        *
 *                                                                            *
 * Hashiwokakero solution.                                                    *
 *   - Bridge intersections are pre calculated to give a planar solution.     *
 *   - Obvious bridges are added.                                             *
 *   - If the problem isn't solved yet, a backtracking is applied in order to *
 *     find the remaining bridges.                                            *
 *                                                                            *
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>


struct bridge;

typedef struct {
	int id;
	int line;
	int column;
	int value; // Original value
	int current_value; // Value after a bridge is attached
	struct bridge* bridges[4]; // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
} Island;

typedef struct bridge {
	int id;
	Island* island_a;
	Island* island_b;
} Bridge;

typedef struct {
	int n, m; // Board dimensions
	int total_bridges, n_placed_bridges; // Bridges counter
	int n_islands, n_bridges; // Arrays lengths
	Island* islands;
	Bridge* bridges;
	int* placed_bridges; // Ids of currently placed bridges
	int* n_crosses; // Number of intersections for each bridge
	int** crosses; // List of bridge intersecions
} Game;


/*
 * Force-brute finding of each island neighbours for faster queries.
 */
void findNeighbours(Game* g) {
	int l, c, nearer, neighbour_id, bridge_id = 0;

	// Initializing
	for (int i = 0; i < g->n_islands; i++)
		for (int j = 0; j < 4; j++)
			g->islands[i].bridges[j] = NULL;

#ifndef NDEBUG
	puts("Finding neighbours...");
	fflush(stdout);
#endif	// NDEBUG

	// Finding neighbours
	for (int i = 0; i < g->n_islands; i++) {
		l = g->islands[i].line;
		c = g->islands[i].column;

		//------------------------------------------------------------------ UP
		nearer = -g->n * g->m; // 'Minus infinity'
		neighbour_id = -1; // Border

		for (int j = 0; j < g->n_islands; j++) {
			if (g->islands[j].column == c &&
				g->islands[j].line < l &&
				g->islands[j].line > nearer) {
				neighbour_id = j;
				nearer = g->islands[j].line;
			}
		}

		// If neighbour was found and was not previously created
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[1]) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].island_a = g->islands + i;
			g->bridges[bridge_id].island_b = g->islands + neighbour_id;
			g->islands[i].bridges[0] = g->bridges + bridge_id;
			g->islands[neighbour_id].bridges[1] = g->bridges + bridge_id;
			bridge_id++;
		}

		//---------------------------------------------------------------- DOWN
		nearer = g->n * g->m; // 'Infinity'
		neighbour_id = -1; // Border

		for (int j = 0; j < g->n_islands; j++) {
			if (g->islands[j].column == c &&
				g->islands[j].line > l &&
				g->islands[j].line < nearer) {
				neighbour_id = j;
				nearer = g->islands[j].line;
			}
		}

		// If neighbour was found and was not previously created
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[0]) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].island_a = g->islands + i;
			g->bridges[bridge_id].island_b = g->islands + neighbour_id;
			g->islands[i].bridges[1] = g->bridges + bridge_id;
			g->islands[neighbour_id].bridges[0] = g->bridges + bridge_id;
			bridge_id++;
		}

		//---------------------------------------------------------------- LEFT
		nearer = -g->n * g->m; // 'Minus infinity'
		neighbour_id = -1; // Border

		for (int j = 0; j < g->n_islands; j++) {
			if (g->islands[j].line == l &&
				g->islands[j].column < c &&
				g->islands[j].column > nearer) {
				neighbour_id = j;
				nearer = g->islands[j].column;
			}
		}

		// If neighbour was found and was not previously created
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[3]) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].island_a = g->islands + i;
			g->bridges[bridge_id].island_b = g->islands + neighbour_id;
			g->islands[i].bridges[2] = g->bridges + bridge_id;
			g->islands[neighbour_id].bridges[3] = g->bridges + bridge_id;
			bridge_id++;
		}

		//--------------------------------------------------------------- RIGHT
		nearer = g->n * g->m; // 'Infinity'
		neighbour_id = -1; // Border

		for (int j = 0; j < g->n_islands; j++) {
			if (g->islands[j].line == l &&
				g->islands[j].column > c &&
				g->islands[j].column < nearer) {
				neighbour_id = j;
				nearer = g->islands[j].column;
			}
		}

		// If neighbour was found and was not previously created
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[2]) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].island_a = g->islands + i;
			g->bridges[bridge_id].island_b = g->islands + neighbour_id;
			g->islands[i].bridges[3] = g->bridges + bridge_id;
			g->islands[neighbour_id].bridges[2] = g->bridges + bridge_id;
			bridge_id++;
		}
	}

#ifndef NDEBUG
	printf("\nReallocating bridges (%d -> %d)\n\n",
		g->total_bridges, bridge_id);
	fflush(stdout);
#endif

	g->n_bridges = bridge_id;
	g->bridges = (Bridge*)realloc(g->bridges, g->n_bridges * sizeof(Bridge));

	g->placed_bridges = (int*)malloc(g->n_bridges * sizeof(int));
	g->n_crosses = (int*)malloc(g->n_bridges * sizeof(int));
	g->crosses = (int**)malloc(g->n_bridges * sizeof(int*));

	for (int i = 0; i < g->n_bridges; i++) {
		g->n_crosses[i] = 0;
		g->crosses[i] = (int*)malloc(g->n_bridges * sizeof(int));
	}

#ifndef NDEBUG
	puts("Neighbours:\n");
	for (int i = 0; i < g->n_islands; i++) {
		printf("%d:", i);
		for (int j = 0; j < 4; j++)
			printf(" %d", g->islands[i].bridges[j] ?
				g->islands[i].bridges[j]->island_a->id == i ?
					g->islands[i].bridges[j]->island_b->id :
					g->islands[i].bridges[j]->island_a->id : -1);
		puts("");		
		fflush(stdout);
	}

	printf("\nBridges (%d):\n\n", g->n_bridges);
	for (int i = 0; i < g->n_bridges; i++) {
		printf("%d: %d %d\n", g->bridges[i].id, g->bridges[i].island_a->id,
			g->bridges[i].island_b->id);
		fflush(stdout);
	}
	puts("");
#endif // NDEBUG
}

/*
 * Checks if two bridges intersect.
 */
inline int intersect(Bridge* a, Bridge* b) {
	int vec_a_l = a->island_a->line - a->island_b->line;
	int vec_a_c = a->island_a->column - a->island_b->column;

	int vec_b_l = b->island_a->line - b->island_b->line;
	int vec_b_c = b->island_a->column - b->island_b->column;

	if (((vec_a_l * (b->island_a->column - a->island_b->column) -
			vec_a_c * (b->island_a->line - a->island_b->line)) *
		(vec_a_l * (b->island_b->column - a->island_b->column) -
			vec_a_c * (b->island_b->line - a->island_b->line)) < 0) &&
		((vec_b_l * (a->island_a->column - b->island_b->column) -
			vec_b_c * (a->island_a->line - b->island_b->line)) *
		(vec_b_l * (a->island_b->column - b->island_b->column) -
			vec_b_c * (a->island_b->line - b->island_b->line)) < 0))
		return 1;

	return 0;
}

/*
 * Force-brute finding of bridge intersections for faster queries.
 */
void calcIntersections(Game* g) {
#ifndef NDEBUG
	puts("\nIntersections:\n");
#endif	// NDEBUG

	for (int i = 0; i < g->n_bridges; i++) {
		for (int j = i + 1; j < g->n_bridges; j++) {
			if (intersect(g->bridges + i, g->bridges + j) == 1) {
				g->crosses[i][g->n_crosses[i]++] = j;
				g->crosses[j][g->n_crosses[j]++] = i;

#ifndef NDEBUG
				printf("%d %d\n", i, j);
#endif	// NDEBUG
			}
		}
	}

#ifndef NDEBUG
	puts("");
#endif	// NDEBUG
}

/*
 * Reads input file and creates the game
 */
void create(Game* g) {
	// Board's dimension and number of islands
	scanf("%d %d %d", &g->n, &g->m, &g->n_islands);

	// Islands creation
	int x, y;

	g->islands = (Island*)malloc(g->n_islands * sizeof(Island));
	g->bridges = (Bridge*)malloc(4 * g->n_islands * sizeof(Bridge));

	g->total_bridges = g->n_placed_bridges = 0;

	for (int i = 0; i < g->n_islands; i++) {
		scanf("%d %d %d", &x, &y, &g->islands[i].value);

		g->islands[i].id = i;
		g->islands[i].line = g->n - y - 1;
		g->islands[i].column = x;
		g->islands[i].current_value = g->islands[i].value;
		g->total_bridges += g->islands[i].value;
	}

	g->total_bridges >>= 1; // Bridges were counted twice

	findNeighbours(g);
	calcIntersections(g);
}

/*
 * Prints the solution
 */
void print(Game* g) {
	puts("");

	int** board = (int**)malloc(3 * g->n * sizeof(int*));

	for (int i = 0; i < 3 * g->n; i++) {
		board[i] = (int*)malloc(3 * g->m * sizeof(int));

		for (int j = 0; j < 3 * g->m; j++)
			board[i][j] = -1;
	}

	for (int i = 0; i < g->n_islands; i++)
		board[3 * g->islands[i].line + 1][3 * g->islands[i].column + 1]
			= g->islands[i].current_value;
/*
g->n_placed_bridges = 16;
g->placed_bridges[0] = 1;
g->placed_bridges[1] = 3;
g->placed_bridges[2] = 8;
g->placed_bridges[3] = 14;
g->placed_bridges[4] = 19;
g->placed_bridges[5] = 21;
g->placed_bridges[6] = 24;
g->placed_bridges[7] = 25;

g->placed_bridges[8] = 9;
g->placed_bridges[9] = 7;
g->placed_bridges[10] = 13;
g->placed_bridges[11] = 18;
g->placed_bridges[12] = 26;
g->placed_bridges[13] = 23;
g->placed_bridges[14] = 27;
g->placed_bridges[15] = 26;
*/
	int l, c;

	for (int i = 0; i < g->n_placed_bridges; i++) {
		l = 3 * g->bridges[g->placed_bridges[i]].island_a->line + 1;
		c = 3 * g->bridges[g->placed_bridges[i]].island_a->column + 1;

		if (g->bridges[g->placed_bridges[i]].island_a->column ==
			g->bridges[g->placed_bridges[i]].island_b->column) {
			if (g->bridges[g->placed_bridges[i]].island_a->line >
				g->bridges[g->placed_bridges[i]].island_b->line)
				while (board[--l][c] < 0)
					board[l][c]--;
			else
				while (board[++l][c] < 0)
					board[l][c]--;
		}
		else {
			if (g->bridges[g->placed_bridges[i]].island_a->column >
				g->bridges[g->placed_bridges[i]].island_b->column)
				while (board[l][--c] < 0)
					board[l][c]--;
			else
				while (board[l][++c] < 0)
					board[l][c]--;
		}
	}

	for (int i = 0; i < g->m; i++)
		printf("---------");
	puts("");

	for (int i = 0; i < 3 * g->n; i++) {
		for (int j = 0; j < 3 * g->m; j++) {
			if (board[i][j] == -1)
				printf("   ");
			else if (board[i][j] == -2)
				printf(" * ");
			else if (board[i][j] == -3)
				printf(" D ");
			else
				printf("(%d)", g->islands[board[i][j]].current_value);
		}

		puts("");
	}

	for (int i = 0; i < g->m; i++)
		printf("---------");

	puts("\n");

	for (int i = 0; i < 3 * g->n; i++)
		free(board[i]);
	free(board);
}

/*
 * Adds all obvious bridges to the board.
 * Fon instance, is an island needs 8 bridges, it must have 2 bridges
 * in all four directions.
 */
void addObviousBridges(Game* g) {
}

/*
 * Solves the game.
 */
void play(Game* g) {
	addObviousBridges(g);
	print(g);

#ifndef NDEBUG
	puts("\nObvious bridges added\n");
#endif	// NDEBUG

	//backtracking(g, 0);
}

/*
 * Frees memory
 */
void clear(Game* g) {
#ifndef NDEBUG
	puts("Free placed bridges");
#endif	// NDEBUG

	free(g->placed_bridges);

#ifndef NDEBUG
	puts("Free crosses");
#endif	// NDEBUG

	for (int i = 0; i < g->n_bridges; i++)
		free(g->crosses[i]);
	free(g->crosses);
	free(g->n_crosses);

#ifndef NDEBUG
	puts("Free bridges");
	fflush(stdout);
#endif	// NDEBUG

	free(g->bridges);

#ifndef NDEBUG
	puts("Free islands");
	fflush(stdout);
#endif	// NDEBUG

	free(g->islands);
}


int main() {
	Game game;

	create(&game);
	play(&game);
	clear(&game);

#ifndef NDEBUG
	puts("\nClean execution test");
	fflush(stdout);
#endif	// NDEBUG

	return 0;
}
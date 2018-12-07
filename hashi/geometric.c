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
	puts("\nFinding neighbours...");
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
		g->islands[i].line = 3 * (g->n - y - 1) + 1;
		g->islands[i].column = 3 * x + 1;
		g->islands[i].current_value = g->islands[i].value;
		g->total_bridges += g->islands[i].value;
	}

	g->total_bridges >>= 1; // Bridges were counted twice

	findNeighbours(g);
}

/*
 * Frees memory
 */
void clear(Game* g) {
#ifndef NDEBUG
	puts("Free islands");
	fflush(stdout);
#endif	// NDEBUG

	free(g->islands);

#ifndef NDEBUG
	puts("Free bridges");
	fflush(stdout);
#endif	// NDEBUG

	free(g->bridges);
}


int main() {
	Game game;

	create(&game);
	clear(&game);

#ifndef NDEBUG
	puts("\nClean execution test");
	fflush(stdout);
#endif	// NDEBUG

	return 0;
}
/******************************************************************************
 * INF 610 - Estruturas de Dados e Algoritmos - 2018/2                        *
 *                                                                            *
 * Greedy hashiwokakero 'solution'.                                           *
 *   - Bridge intersections are calculated first.                             *
 *   - The bridges are added to the board by its number of intersections      *
 *     (lower first).                                                         *
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
	int used;
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
	int* visited; // Auxiliary for the 'isolated' function
} Game;


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
		// and neighter the current island nor its neighbour support
		// only one bridge
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[1] &&
			(g->islands[i].value > 1 || g->islands[neighbour_id].value > 1)) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].used = 0;
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
		// and neighter the current island nor its neighbour support
		// only one bridge
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[0] &&
			(g->islands[i].value > 1 || g->islands[neighbour_id].value > 1)) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].used = 0;
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
		// and neighter the current island nor its neighbour support
		// only one bridge
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[3] &&
			(g->islands[i].value > 1 || g->islands[neighbour_id].value > 1)) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].used = 0;
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
		// and neighter the current island nor its neighbour support
		// only one bridge
		if (neighbour_id >= 0 && !g->islands[neighbour_id].bridges[2] &&
			(g->islands[i].value > 1 || g->islands[neighbour_id].value > 1)) {
			g->bridges[bridge_id].id = bridge_id;
			g->bridges[bridge_id].used = 0;
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

	g->placed_bridges = (int*)malloc(g->total_bridges * sizeof(int));
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
inline int doIntersect(Bridge* a, Bridge* b) {
	// Parallel bridges never cross
	if (a->island_a->line == a->island_b->line &&
		b->island_a->line == b->island_b->line ||
		a->island_a->column == a->island_b->column &&
		b->island_a->column == b->island_b->column)
		return 0;

	int a_sup_line = min(a->island_a->line, a->island_b->line);
	int a_inf_line = max(a->island_a->line, a->island_b->line);
	int b_sup_line = min(b->island_a->line, b->island_b->line);
	int b_inf_line = max(b->island_a->line, b->island_b->line);

	int a_sup_column = max(a->island_a->column, a->island_b->column);
	int a_inf_column = min(a->island_a->column, a->island_b->column);
	int b_sup_column = max(b->island_a->column, b->island_b->column);
	int b_inf_column = min(b->island_a->column, b->island_b->column);

	if (a_sup_line == a_inf_line &&
		a_sup_line > b_sup_line &&
		a_sup_line < b_inf_line &&
		a_sup_column > b_sup_column &&
		a_inf_column < b_sup_column)
		return 1;
	else if (b_sup_line > a_sup_line &&
		b_sup_line < a_inf_line &&
		b_sup_column > a_sup_column &&
		b_inf_column < a_inf_column)
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
			if (doIntersect(g->bridges + i, g->bridges + j) == 1) {
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
	g->visited = (int*)malloc(g->n_islands * sizeof(int));

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
					board[l][c] -= 3;
			else
				while (board[l][++c] < 0)
					board[l][c] -= 3;
		}
	}

	for (int i = 0; i < g->m; i++)
		printf("---------");
	puts("");

	for (int i = 0; i < 3 * g->n; i++) {
		for (int j = 0; j < 3 * g->m; j++) {
			if (board[i][j] == 9 || board[i][j] == -1)
				printf("   ");
			else if (board[i][j] == -2)
				printf(" | ");
			else if (board[i][j] == -3)
				printf(" D ");
			else if (board[i][j] == -4)
				printf("---");
			else if (board[i][j] == -7)
				printf("DDD");
			else
				printf("(%d)", board[i][j]);
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
 * Checks if the added bridge will not cross any of the bridges in the board
 * Brute-force, improve later
 */
int canAddBridge(Game* g, int bridge_id) {
	if (g->bridges[bridge_id].island_a->current_value == 0 ||
		g->bridges[bridge_id].island_b->current_value == 0 ||
		g->bridges[bridge_id].used == 2)
		return 0;

	for (int i = 0; i < g->n_placed_bridges; i++)
		for (int j = 0; j < g->n_crosses[bridge_id]; j++)
			if (g->crosses[bridge_id][j] == g->placed_bridges[i])
				return 0;

	return 1;
}

/*
 * Checks if island needs more bridges and there are neighbours which
 * can have more bridges attached.
 * Returns 1 if possible, 0 otherwise.
 */
int numberOfBridgesCanBeSatisfied(Game* g, int id) {
	int sum = 0;

	// UP
	if (g->islands[id].bridges[0]) {
		if (g->islands[id].bridges[0]->used == 0) {
			if (g->islands[id].bridges[0]->island_a->id == id)
				sum +=
					min(2, g->islands[id].bridges[0]->island_a->current_value);
			else
				sum +=
					min(2, g->islands[id].bridges[0]->island_b->current_value);
		}
		else if (g->islands[id].bridges[0]->used == 1) {
			if (g->islands[id].bridges[0]->island_a->id == id)
				sum +=
					min(1, g->islands[id].bridges[0]->island_a->current_value);
			else
				sum +=
					min(1, g->islands[id].bridges[0]->island_b->current_value);
		}
	}

	// DOWN
	if (g->islands[id].bridges[1]) {
		if (g->islands[id].bridges[1]->used == 0) {
			if (g->islands[id].bridges[1]->island_a->id == id)
				sum +=
					min(2, g->islands[id].bridges[1]->island_a->current_value);
			else
				sum +=
					min(2, g->islands[id].bridges[1]->island_b->current_value);
		}
		else if (g->islands[id].bridges[1]->used == 1) {
			if (g->islands[id].bridges[1]->island_a->id == id)
				sum +=
					min(1, g->islands[id].bridges[1]->island_a->current_value);
			else
				sum +=
					min(1, g->islands[id].bridges[1]->island_b->current_value);
		}
	}

	// LEFT
	if (g->islands[id].bridges[2]) {
		if (g->islands[id].bridges[2]->used == 0) {
			if (g->islands[id].bridges[2]->island_a->id == id)
				sum +=
					min(2, g->islands[id].bridges[2]->island_a->current_value);
			else
				sum += 
					min(2, g->islands[id].bridges[2]->island_b->current_value);
		}
		else if (g->islands[id].bridges[2]->used == 1) {
			if (g->islands[id].bridges[2]->island_a->id == id)
				sum +=
					min(1, g->islands[id].bridges[2]->island_a->current_value);
			else
				sum +=
					min(1, g->islands[id].bridges[2]->island_b->current_value);
		}
	}

	// RIGHT
	if (g->islands[id].bridges[3]) {
		if (g->islands[id].bridges[3]->used == 0) {
			if (g->islands[id].bridges[3]->island_a->id == id)
				sum +=
					min(2, g->islands[id].bridges[3]->island_a->current_value);
			else
				sum +=
					min(2, g->islands[id].bridges[3]->island_b->current_value);
		}
		else if (g->islands[id].bridges[3]->used == 1) {
			if (g->islands[id].bridges[3]->island_a->id == id)
				sum +=
					min(1, g->islands[id].bridges[3]->island_a->current_value);
			else
				sum +=
					min(1, g->islands[id].bridges[3]->island_b->current_value);
		}
	}

	if (sum >= g->islands[id].current_value)
		return 1;
	else
		return 0;
}

/*
 * Procedure of 'checkIfIsolated' function.
 * Returns the number of visited islands.
 */
int isolatedProcedure(Game* g, int id) {
	g->visited[id] = g->islands[id].current_value + 1;

	int sum = 0;

	// UP
	if (g->islands[id].bridges[0] &&
		g->islands[id].bridges[0]->used > 0) {
		if (id == g->islands[id].bridges[0]->island_a->id &&
			g->visited[g->islands[id].bridges[0]->island_b->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[0]->island_b->id);
		else if (g->visited[g->islands[id].bridges[0]->island_a->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[0]->island_a->id);
	}

	// DOWN
	if (g->islands[id].bridges[1] &&
		g->islands[id].bridges[1]->used > 0) {
		if (id == g->islands[id].bridges[1]->island_a->id &&
			g->visited[g->islands[id].bridges[1]->island_b->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[1]->island_b->id);
		else if (g->visited[g->islands[id].bridges[1]->island_a->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[1]->island_a->id);
	}

	// LEFT
	if (g->islands[id].bridges[2] &&
		g->islands[id].bridges[2]->used > 0) {
		if (id == g->islands[id].bridges[2]->island_a->id &&
			g->visited[g->islands[id].bridges[2]->island_b->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[2]->island_b->id);
		else if (g->visited[g->islands[id].bridges[2]->island_a->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[2]->island_a->id);
	}

	// RIGHT
	if (g->islands[id].bridges[3] &&
		g->islands[id].bridges[3]->used > 0) {
		if (id == g->islands[id].bridges[3]->island_a->id &&
			g->visited[g->islands[id].bridges[3]->island_b->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[3]->island_b->id);
		else if (g->visited[g->islands[id].bridges[3]->island_a->id] == 0)
			sum +=
				isolatedProcedure(g, g->islands[id].bridges[3]->island_a->id);
	}

	return sum + 1;
}

/*
 * Checks if the subgraph with g->islands[id] forms an isolated component
 * Returns 1 if true, 0 otherwise.
 */
int isolated(Game* g, int id) {
	for (int i = 0; i < g->n_islands; i++)
		g->visited[i] = 0;

	int n_visited = isolatedProcedure(g, id);

	if (n_visited == g->n_islands)
		return 0;

	for (int i = 0; i < g->n_islands; i++)
		if (g->visited[i] > 1)
			return 0;

	return 1;
}

// Struct used in function below ('greedy')
typedef struct {
	int id;
	int n_crosses;
} BridgeIds;

// Comparison function for function below ('greedy')
int bridgeCmp(const void* a, const void* b) {
	return ((BridgeIds*)a)->n_crosses - ((BridgeIds*)b)->n_crosses;
}

/*
 * Sorts the bridges by the number of intersections.
 * Adds bridges to the board in sorted order (only if it can be added).
 */
void greedy(Game* g) {
	BridgeIds* ids = (BridgeIds*)malloc(g->n_bridges * sizeof(BridgeIds));

	for (int i = 0; i < g->n_bridges; i++) {
		ids[i].id = g->bridges[i].id;
		ids[i].n_crosses = g->n_crosses[ids[i].id];
	}

	qsort(ids, g->n_bridges, sizeof(BridgeIds), bridgeCmp);

	int can_add_bridge;

	// Bridges that do not invalidate the solution are added first
	// (in 'number of intersections' order)
	for (int i = 0; i < g->n_bridges; i++) {
		if (canAddBridge(g, ids[i].id)) {
			for (int j = 0; j < g->n_islands; j++) {
				can_add_bridge = numberOfBridgesCanBeSatisfied(g, j);

				if (!can_add_bridge)
					break;
			}

			if (can_add_bridge) {
				g->placed_bridges[g->n_placed_bridges++] = ids[i].id;

				if (!isolated(g, g->bridges[ids[i].id].island_a->id)) {
					g->bridges[ids[i].id].island_a->current_value--;
					g->bridges[ids[i].id].island_b->current_value--;
					g->bridges[ids[i].id].used++;
				}
				else
					g->n_placed_bridges--;
			}
		}
	}

#ifndef NDEBUG
	printf("First 'for' added: %d\n", g->n_placed_bridges);
	print(g);
#endif	// NDEBUG

	int second_for_cont = g->n_placed_bridges;

	// Remaining bridges are added by 'number of intersections' order
	for (int i = 0; i < g->n_bridges; i++) {
		if (canAddBridge(g, ids[i].id)) {
			g->placed_bridges[g->n_placed_bridges++] = ids[i].id;
			g->bridges[ids[i].id].island_a->current_value--;
			g->bridges[ids[i].id].island_b->current_value--;
			g->bridges[ids[i].id].used++;
		}
	}

#ifndef NDEBUG
	printf("Second 'for' added: %d\n", g->n_placed_bridges - second_for_cont);
#endif	// NDEBUG

	free(ids);
}

/*
 * Solves the game.
 */
void play(Game* g) {
	print(g);
	greedy(g);
	print(g);
}

/*
 * Frees memory
 */
void clear(Game* g) {
#ifndef NDEBUG
	puts("Free placed bridges");
	fflush(stdout);
#endif	// NDEBUG

	free(g->placed_bridges);

#ifndef NDEBUG
	puts("Free crosses");
	fflush(stdout);
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

#ifndef NDEBUG
	puts("Free visited");
	fflush(stdout);
#endif	// NDEBUG

	free(g->visited);
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANK 3
#define GRID_WIDTH (RANK*RANK) // 9 if rank is 3
#define GRID_SIZE (GRID_WIDTH*GRID_WIDTH) // 81 if rank is 3
#define EMPTY 0 // empty cell has value of 0

typedef enum { false, true } bool;

typedef struct _grid {
    int cells[GRID_SIZE];
} Grid;

// struct _component. Not intended as a writable
// point of entry to the Grid. All changes to the Grid
// MUST be done through the Grid itself
struct _component {
    const int *cells[GRID_WIDTH];
};

typedef struct _component Row;
typedef struct _component Column;
typedef struct _component Block;

// returns absolute (0 to GRID_SIZE-1) index
// given relative (row y, col x) index
int grid_index(int y, int x) {
    return (y * GRID_WIDTH) + x;
}

/// --- GET COMPONENT FUNCTIONS --- ///
// These functions return a const _component
// corresponding to the indexed row, column, or block
// in the given grid.
// NOTE: Even though the Grid cannot be edited by the _component objects,
// the grid's values can still change during the lifetime of the _component.
// in addition, the _component may live longer than the Grid, so watch out for
// dangling pointers.
const Row get_row(const Grid *g, int row_index) {
	Row row;
	for (int i = 0; i < GRID_WIDTH; i++) {
        int index = (row_index * GRID_WIDTH) + i;
		row.cells[i] = &g->cells[index];
    }
	return row;
}

const Column get_column(const Grid *g, int col_index) {
    Column col;
    for (int i = 0; i < GRID_WIDTH; i++) {
        int index = (i * GRID_WIDTH) + col_index;
        col.cells[i] = &g->cells[index];
    }
    return col;
}

const Block get_block(const Grid *g, int block_index) {
    Block block;
    int band_index = block_index / RANK;
    int stack_index = block_index % RANK;
    for (int i = 0; i < GRID_WIDTH; i++) {
        int col = i % RANK;
        int row = i / RANK;
	// here, grid_index aligns with the absolute indices of the grid
	// within the 0-th block as the loop progresses.
        int grid_index = (row * GRID_WIDTH) + col;
	// use band and stack as the "row and column" respectively to index a particular
	// block, and shift grid_index to match 
        grid_index = grid_index + (GRID_WIDTH * RANK * band_index);
        grid_index = grid_index + (RANK * stack_index);
	// add address of cells from grid to block
        block.cells[i] = &g->cells[grid_index];
    }
    return block;
}

// returns a random value from lower to upper inclusive
int random_range(int lower, int upper) {
    srand(clock());
    int r = rand();
    r = (r % upper) + lower;
    return r;
}

void print_row(const Row r) {
    for (int i = 0; i < GRID_WIDTH; i++) 
        printf("%d ", *r.cells[i]);
    printf("\n");
}

void print_column(const Column c) {
    for (int i = 0; i < GRID_WIDTH; i++)
        printf("%d\n", *c.cells[i]);
}

void print_block(const Block b) {
    for (int i = 0; i < GRID_WIDTH; i++) {
        printf("%d ", *b.cells[i]);
        if (i % RANK == (RANK-1))
            printf("\n");
    }
}

void print_grid(const Grid g, bool no_zeros) {
    for (int i = 0; i < GRID_SIZE; i++) {
        int cell = g.cells[i];
        if (no_zeros && cell == EMPTY) 
            printf("  ");
        else
            printf("%d ", cell);
        if ((i % GRID_WIDTH) == GRID_WIDTH - 1)
            printf("\n");
    }
}

bool component_is_valid(const struct _component comp) {
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = i+1; j < GRID_WIDTH; j++) {
            if (*comp.cells[i] == *comp.cells[j] && *comp.cells[i] != EMPTY) {
                return false;
            }
        }
    }
    return true;
}

bool grid_is_valid(Grid grid) {
    for (int i = 0; i < GRID_WIDTH; i++) {
        const Row row = get_row(&grid, i);
        const Column col = get_column(&grid, i);
        const Block block = get_block(&grid, i);
        if (!component_is_valid(row)) return false;
        if (!component_is_valid(col)) return false;
        if (!component_is_valid(block)) return false;
    }
    return true;
}

bool value_is_valid(Grid grid, int index, int value) {
    grid.cells[index] = value;
    if (grid_is_valid(grid)) return true;
    return false;
}

/*
int count_empty(Grid grid) {
    for (int i = 0; i < GRID_SIZE; i++) {

    }
}
*/

bool grid_is_complete(Grid grid) {
    if (!grid_is_valid(grid)) return false;
    if (grid_empty(grid)) return false;
    return true;
}

void get_random_permutation(int *permutation) {
    // fill permutation with values from 1 to GRID_WIDTH
    for (int i = 0; i < GRID_WIDTH; i++) {
        permutation[i] = i+1;
    }
    // swap permutation[i] with a random element in permutation
    for (int i = 0; i < GRID_WIDTH; i++) {
        int j = random_range(0, GRID_WIDTH-1);
        int t = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = t;
    }
}

bool solve(const Grid *grid, Grid *solution, int index) {
    // see if we reached the end of the grid
    if (index >= GRID_SIZE) {
        if (grid_is_complete(*solution)) return true;
        return false;
    }
    if (grid->cells[index] != EMPTY) {
        // skip this index
        solution->cells[index] = grid->cells[index];
        index++;
    }
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation);
    for (int i = 0; i < GRID_WIDTH; i++) {
        // move to next value in permutation if invalid
        if (!value_is_valid(*solution, index, permutation[i])) continue;
        // put value in grid and see if it leads to a solution
        solution->cells[index] = permutation[i];
        if (solve(solution, solution, index+1)) return true;
        // did not lead to a solution, clear value
        solution->cells[index] = EMPTY;
    }
    // no valid values were found
    return false;
}

bool accept(Grid solution) {
    return grid_is_complete(solution);
}

bool reject(Grid solution) {
    if (!grid_is_valid(solution)) return true;
    return false;
}

void count_solutions(Grid *solution, int index, int *num_solutions) {
    if (reject(*solution)) return;
    if (accept(*solution)) {
        *num_solutions = *num_solutions + 1;
        return;
    }
    while (solution->cells[index] != EMPTY && index < GRID_SIZE) {
        // skip this index
        index++;
    }
    if (index >= GRID_SIZE) return;
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation);
    for (int i = 0; i < GRID_WIDTH; i++) {
        if (!value_is_valid(*solution, index, permutation[i])) continue;
        solution->cells[index] = permutation[i];
        count_solutions(solution, index+1, num_solutions);
        solution->cells[index] = EMPTY;
    }
}

Grid get_solution(Grid grid) {
    Grid solution = {EMPTY};
    if (!solve(&grid, &solution, 0))
        fprintf(stderr, "Solution not found\n");
    return solution;
}

int get_num_solutions(Grid grid) {
    int num_solutions = 0;
    Grid solution = grid;
    count_solutions(&solution, 0, &num_solutions);
    return num_solutions;
}

void count_solutions_mod(Grid *solution, int index, int *num_solutions) {
    if (*num_solutions > 1) return;
    if (reject(*solution)) return;
    if (accept(*solution)) {
        *num_solutions = *num_solutions + 1;
        return;
    }
    while (solution->cells[index] != EMPTY && index < GRID_SIZE) {
        // skip this index
        index++;
    }
    if (index >= GRID_SIZE) return;
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation);
    for (int i = 0; i < GRID_WIDTH; i++) {
        if (!value_is_valid(*solution, index, permutation[i])) continue;
        solution->cells[index] = permutation[i];
        count_solutions_mod(solution, index+1, num_solutions);
        solution->cells[index] = EMPTY;
    }
}

bool has_unique_solution(Grid grid) {
    int num_solutions = 0;
    Grid solution = grid;
    count_solutions_mod(&solution, 0, &num_solutions);
    if (num_solutions > 1) return false;
    return true;
}

void get_ordered_permutation(int *perm) {
    for (int i = 0; i < GRID_WIDTH; i++) {
        perm[i] = i+1;
    }
}

void grid_copy(Grid *to, Grid from) {
    for (int i = 0; i < GRID_SIZE; i++) {
        to->cells[i] = from.cells[i];
    }
}

bool grid_is_empty(Grid grid) {
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid.cells[i] != EMPTY) return false;
    }
    return true;
}

Grid remove_random(Grid grid) {
    if (grid_is_empty(grid)) return grid;
    int index = random_range(0, GRID_SIZE-1);
    while (grid.cells[index] == EMPTY)
        index = random_range(0, GRID_SIZE-1);
    grid.cells[index] = EMPTY;
    return grid;
}

Grid remove_random_symmetric(Grid grid) {
    if (grid_is_empty(grid)) return grid;
    int index = random_range(0, GRID_SIZE-1);
    while (grid.cells[index] == EMPTY)
        index = random_range(0, GRID_SIZE-1);
    grid.cells[index] = EMPTY;
    grid.cells[(GRID_SIZE-1)-index] = EMPTY;
    return grid;
}

Grid get_empty() {
    Grid g = {EMPTY};
    return g;
}

Grid get_playable_symmetric(Grid grid, int num_clues) {
    if (num_clues == GRID_SIZE) return grid;
    int num_removed = 0;
    int num_to_remove = GRID_SIZE - num_clues;
    return grid;
}

Grid get_playable(int num_clues) {
    Grid grid = {EMPTY};
    grid = get_solution(grid);
    if (num_clues == GRID_SIZE) return grid;
    int num_removed = 0;
    int num_to_remove = GRID_SIZE - num_clues;
    while (num_removed < num_to_remove) {
        grid = remove_random(grid);
        if (has_unique_solution(grid)) {
            num_removed += 1;
        }
        else {
            num_removed = 0;
            grid = get_empty();
            grid = get_solution(grid);
        }
    }
    return grid;
}

int count_nonzero(Grid grid) {
    int count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid.cells[i] != EMPTY)
            count++;
    }
    return count;
}

void print_full(Grid grid) {
    printf("----------------------\n");
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        printf("|");
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            int index = (i*GRID_WIDTH) + j;
            if (grid.cells[index] != EMPTY)
                printf("\033[7m%d \033[0m", grid.cells[index]);
            else printf("  ");
            if (j % RANK == RANK-1) printf("|");
        }
        if (i % RANK == RANK-1) printf("\n----------------------");
        printf("\n");
    }
}

int main() {
    // zero-initialize grid
    /*
    Grid grid = {EMPTY};
    Grid solution = get_solution(grid);
    grid = solution;
    print_grid(grid, true);
    printf("\n");
    */
    Grid grid = {0};
    int num_clues = 32;
    while (1) {
        grid = get_playable_symmetric(grid, num_clues);
        print_full(grid);
        printf("removed: %d\n\n", GRID_SIZE-num_clues);
    }
    
    return 0;
}

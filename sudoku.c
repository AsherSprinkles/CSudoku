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

Grid get_empty() {
    Grid g = {EMPTY};
    return g;
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

int count_empty_cells(Grid grid) {
    int count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid.cells[i] == EMPTY) count++;
    }
    return count;
}

int count_filled_cells(Grid grid) {
    return (GRID_SIZE - count_empty_cells(grid));
}

bool grid_is_empty(Grid grid) {
    int filled = count_filled_cells(grid);
    if (filled != 0) return false;
    return true;
}

bool grid_is_filled(Grid grid) {
    int empty = count_empty_cells(grid);
    if (empty != 0) return false;
    return true;
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

bool grid_is_complete(Grid grid) {
    if (!grid_is_valid(grid)) return false;
    if (!grid_is_filled(grid)) return false;
    return true;
}

void shuffle(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        int j = random_range(0, size-1);
        int t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

// gets random permutation of integers between start & end inclusive
void get_random_permutation(int *permutation, int start, int end) {
    // fill permutation with values from 1 to GRID_WIDTH
    int p_size = (end-start) + 1;
    for (int i = 0; i < p_size; i++) {
        permutation[i] = i+start;
    }
    // shuffle permutation
    shuffle(permutation, p_size);
}


bool solve(Grid *solution, int index) {
    // end this search path because it leads nowhere
    if (!grid_is_valid(*solution)) return false;
    // end this search path because it leads to a solution
    if (grid_is_complete(*solution)) return true;
    while (index < GRID_SIZE && solution->cells[index] != EMPTY)
        index++;
    if (index >= GRID_SIZE) return false;
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation, 1, GRID_WIDTH);
    for (int i = 0; i < GRID_WIDTH; i++) {
        solution->cells[index] = permutation[i];
        if(solve(solution, index+1)) return true;
        solution->cells[index] = EMPTY;
    }
    return false;
}

Grid get_solution(Grid grid) {
    if (!solve(&grid, 0))
        fprintf(stderr, "Solution not found\n");
    return grid;
}

Grid get_random_solution() {
    return get_solution(get_empty());
}

// Not efficient for most purposes; solutions increase exponentially with more empty spaces
int count_solutions(Grid *solution, int index) {
    // end this search path because it leads nowhere
    if (!grid_is_valid(*solution)) return 0;
    // end this search path because it leads to a solution
    if (grid_is_complete(*solution)) return 1;
    while (index < GRID_SIZE && solution->cells[index] != EMPTY)
        index++;
    if (index >= GRID_SIZE) return 0;
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation, 1, GRID_WIDTH);
    int num_solutions = 0;
    for (int i = 0; i < GRID_WIDTH; i++) {
        solution->cells[index] = permutation[i];
        num_solutions += count_solutions(solution, index+1);
        solution->cells[index] = EMPTY;
    }
    return num_solutions;
}

// More efficient version of count_solutions that stops after two solutions are found
void uniqueness_test(Grid *solution, int index, int *num_solutions) {
    if (*num_solutions > 1) return;
    // end this search path because it leads nowhere
    if (!grid_is_valid(*solution)) return;
    // end this search path because it leads to a solution
    if (grid_is_complete(*solution)) {
        *num_solutions += 1;
        return;
    }
    while (index < GRID_SIZE && solution->cells[index] != EMPTY)
        index++;
    if (index >= GRID_SIZE) return;
    int permutation[GRID_WIDTH];
    get_random_permutation(permutation, 1, GRID_WIDTH);
    for (int i = 0; i < GRID_WIDTH; i++) {
        solution->cells[index] = permutation[i];
        if (*num_solutions <= 1) uniqueness_test(solution, index+1, num_solutions);
        else return;
        solution->cells[index] = EMPTY;
    }
}

int get_num_solutions(Grid grid) {
    return count_solutions(&grid, 0);
}

bool has_unique_solution(Grid grid) {
    int num_solutions = 0;
    uniqueness_test(&grid, 0, &num_solutions);
    if (num_solutions > 1) return false;
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

Grid get_playable_symmetric(int num_clues) {
    if (num_clues == GRID_SIZE) return get_random_solution();
    Grid grid = get_random_solution();
    Grid copy;
    while (count_filled_cells(grid) > num_clues) {
        grid = get_random_solution();
        copy = grid;
        while (has_unique_solution(copy)) {
            grid = copy;
            copy = remove_random_symmetric(copy);
        }
    }
    return grid;
}

Grid get_playable(Grid grid, int num_clues) {
    if (num_clues == GRID_SIZE) return grid;
    while (count_filled_cells(grid) > num_clues) {
        Grid copy = grid;
        copy = remove_random(grid);
        if (has_unique_solution(copy))
            grid = copy;
    }
    return grid;
}

bool is_locally_minimal(Grid grid) {
    for (int i = 0; i < GRID_SIZE; i++) {
        // clear the cell
        int removed = grid.cells[i];
        grid.cells[i] = EMPTY;
        // check for unique solution and replace the cell
        if (has_unique_solution(grid)) return false;
        grid.cells[i] = removed;
    }
    return true;
}

int get_filled_indices(Grid grid, int indices[]) {
    // it is assumed that the indices array has a size of GRID_SIZE
    int size = 0;
    // add indices of all nonempty cells in grid to indices array
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid.cells[i] != EMPTY) {
            indices[size] = i;
            size++;
        }
    }
    return size;
}

bool locally_minimize(Grid *grid) {
    if (is_locally_minimal(*grid)) return true;
    if (!has_unique_solution(*grid)) return false;
    int filled[GRID_SIZE];
    int size = get_filled_indices(*grid, filled);
    shuffle(filled, size);
    for (int i = 0; i < size; i++) {
        int removed = grid->cells[filled[i]];
        grid->cells[filled[i]] = EMPTY;
        if (locally_minimize(grid)) return true;
        grid->cells[filled[i]] = removed;
    }
    return false;
}

Grid get_local_minimum(Grid grid) {
    int indices[GRID_SIZE];
    int size = get_filled_indices(grid, indices);
    for (int i = 0; i < size; i++) {
        int removed = grid.cells[indices[i]];
        grid.cells[indices[i]] = EMPTY;
        if (!has_unique_solution(grid)) {
            grid.cells[indices[i]] = removed;
        }
    }
    return grid;
}

int main() {
    Grid grid = get_random_solution();
    print_full(grid);
    grid = get_local_minimum(grid);
    print_full(grid);
    printf("Num clues: %d\n", count_filled_cells(grid));

    return 0;
}

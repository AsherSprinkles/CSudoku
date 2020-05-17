#include <ncurses.h>
#include <stdio.h>
#include <uchar.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>

#define TEMPLATE_FILE "pl.txt"
#define GRID_WIDTH 37
#define GRID_HEIGHT 19

const char *GRID_TEMPLATE[GRID_HEIGHT] = {"\u250F\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u252F\u2501\u2501\u2501\u2513\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2523\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u254B\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u254B\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u252B\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2523\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u254B\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u254B\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u253F\u2501\u2501\u2501\u252B\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2520\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2542\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u253C\u2500\u2500\u2500\u2528\n",
"\u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503   \u2502   \u2502   \u2503\n",
"\u2517\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u2537\u2501\u2501\u2501\u251B\n"};

const int dy = 2;
const int dx = 4;

#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68
#define W 119
#define A 97
#define S 115
#define D 100

#define X_MIN 2
#define Y_MIN 1
#define X_MAX 34
#define Y_MAX 17

void get_print_offset(int *y, int *x) {
    getmaxyx(stdscr, *y, *x);
    *y = *y/2 - GRID_HEIGHT/2;
    *x = *x/2 - GRID_WIDTH/2;
}

void print_grid_template() {
    int y, x;
    get_print_offset(&y, &x);
    for (int i = 0; i < GRID_HEIGHT; i++) 
        mvprintw(y+i, x, GRID_TEMPLATE[i]);
    refresh();
    move(0,0);
}

void update_yx(int c, int *y, int *x) {
    switch (c)
    {
    case UP: 
    case W:
        if (*y > Y_MIN) *y -= dy;
        break;
    case DOWN: 
    case S:
        if (*y < Y_MAX) *y += dy;
        break;
    case LEFT: 
    case A:
        if (*x > X_MIN) *x -= dx;
        break;
    case RIGHT: 
    case D:
        if (*x < X_MAX) *x += dx;
    }
}

void update_num(int c) {
    if (c == 48) {
        addch(' ');
    }
    else if (c >= 49 && c <= 57) {
        addch((char) c);
    }
}

bool size_changed(int ymax, int xmax) {
    int newy, newx;
    getmaxyx(stdscr, newy, newx);
    if (newy != ymax || newx != xmax) return true;
    return false;
}

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    int y = 1;
    int x = 2;
    int c;
    int y_off, x_off;
    int ymax, xmax;
    getmaxyx(stdscr, ymax, xmax);
    get_print_offset(&y_off, &x_off);
    print_grid_template();
    while (1) {
        move(y+y_off,x+x_off);
        refresh();

        c = getch();
        update_yx(c, &y, &x);
        update_num(c);
        // conditionally clearing the screen prevents glitches
        if (size_changed(ymax, xmax)) {
            getmaxyx(stdscr, ymax, xmax);
            get_print_offset(&y_off, &x_off);
            clear();
            print_grid_template();
        }
    }

    endwin();
    return 0;
}

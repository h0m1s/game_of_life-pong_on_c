#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25

void init_screen();
void render(int** current_field);
int input(int* game_speed);
void render_end(int generation);
void input_start_field(int** current_field);

int** allocate_array();
void free_array(int** array);

char myTolower(char c);

int alive_point(int** array);

int alive_near(int** mtx, int i, int j);
void game_process(int** old, int** new, int* changes);

int main() {
    init_screen();

    int** current_field = allocate_array();
    int** past_field = allocate_array();
    input_start_field(current_field);
    if (!freopen("/dev/tty", "r", stdin)) {
        perror("/dev/tty");
    }
    render(current_field);

    int game_speed = 5;
    int generation = 0;
    int changes = 1;
    
    while (alive_point(current_field) != 0 && changes != 0) {
        if (input(&game_speed)) {
            break;
        }

        game_process(current_field, past_field, &changes);
        render(current_field);
        usleep(game_speed * 100000);
        generation++;
    }
    render_end(generation);
    while (input(&game_speed) != 1) {
    }
    endwin();
    free_array(current_field);
    free_array(past_field);

    return 0;
}

int** allocate_array() {
    int** arr = (int**)malloc(HEIGHT * sizeof(int*));
    for (int i = 0; i < HEIGHT; i++) {
        arr[i] = (int*)malloc(WIDTH * sizeof(int));
    }
    return arr;
}

void free_array(int** array) {
    for (int i = 0; i < HEIGHT; i++) {
        free(array[i]);
    }
    free(array);
}

void input_start_field(int** field) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            scanf("%d", &field[i][j]);
        }
    }
}

char myTolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
}

int input(int* game_speed) {
    char ctrl = myTolower(getch());

    if (ctrl == 'a' && *game_speed != 10) {
        *game_speed = *game_speed + 1;
    } else if (ctrl == 'd' && *game_speed != 1) {
        *game_speed = *game_speed - 1;
    }
    return (ctrl == 'q');
}

int alive_point(int** array) {
    int alive = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (array[i][j] == 1) alive++;
        }
    }
    return alive;
}

void init_screen() {
    initscr();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
}

int alive_near(int** mtx, int i, int j) {
    int alive = 0;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = (i + di + HEIGHT) % HEIGHT;
            int nj = (j + dj + WIDTH) % WIDTH;
            alive += mtx[ni][nj];
        }
    }
    return alive;
}

void game_process(int** old, int** new, int* changes) {
    *changes = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int alive = alive_near(old, i, j);
            if (old[i][j] == 1) {
                if (alive < 2 || alive > 3) {
                    new[i][j] = 0;
                } else {
                    new[i][j] = 1;
                }
            } else {
                if (alive == 3) {
                    new[i][j] = 1;
                } else {
                    new[i][j] = 0;
                }
            }
            if (old[i][j] != new[i][j]) (*changes)++;
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            old[i][j] = new[i][j];
        }
    }
}

void render(int** current_field) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (current_field[y][x] == 1) {
                mvprintw(y, x, "*");
            } else {
                mvprintw(y, x, " ");
            }
        }
    }
    refresh();
}
void render_end(int generation) {
    clear();
    mvprintw(HEIGHT / 2 - 1, (WIDTH - 20) / 2, "GAME OVER!");
    mvprintw(HEIGHT / 2, (WIDTH - 20) / 2, "Generations: %d", generation);
}
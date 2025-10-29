// gcc -Wall -Werror -Wextra -lncurses pong_interactive.c
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define OFFSET 10

char myTolower(char);
void render(int[], float[], int[]);
int input(int[]);
void gameProccess(float[], float[], int[], int[]);
void renderWinner(int[]);

int main() {
    initscr();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    const int WON_SCORE = 21;
    int score[2] = {0, 0};
    int startPos = ceil(HEIGHT / 2);
    int playerPosition[2] = {startPos, startPos};
    float ball[2] = {WIDTH / 2.f, startPos + 1};
    float vel[2] = {-1.f, 0.f};
    render(playerPosition, ball, score);
    while (score[0] < WON_SCORE && score[1] < WON_SCORE) {
        if (input(playerPosition)) {
            return 0;
        }
        gameProccess(ball, vel, playerPosition, score);
        render(playerPosition, ball, score);
        usleep(100000);
    }
    renderWinner(score);
    endwin();
}

void gameProccess(float ball[], float vel[], int pPos[], int score[]) {
    int x = floor(ball[0]);
    int y = floor(ball[1]);
    int target = vel[0] > 0;
    if ((x == OFFSET - 1 && y >= pPos[0] && y <= pPos[0] + 2) ||
        (x == WIDTH - OFFSET && y >= pPos[1] && y <= pPos[1] + 2)) {
        vel[0] *= -1;
        if (y == pPos[target]) {
            vel[1] += 0.1f;
        } else if (y == pPos[target] + 2) {
            vel[1] -= 0.1f;
        }
    }
    if (ball[1] <= 0.f && vel[1] < 0.f) {
        vel[1] *= -1.f;
    } else if (ball[1] >= HEIGHT - 1.f && vel[1] > 0.f) {
        vel[1] *= -1.f;
    }
    if (ball[0] <= 0 || ball[0] >= WIDTH - 1) {
        int startPos = ceil(HEIGHT / 2);
        ball[0] = WIDTH / 2;
        ball[1] = startPos + 1;
        vel[1] = 0.f;
        pPos[0] = startPos;
        pPos[1] = startPos;
        score[vel[0] < 0]++;
    } else {
        ball[0] += vel[0];
        ball[1] += vel[1];
    }
}

int input(int pPos[]) {
    char ctrl = myTolower(getch());
    if (ctrl == 'a' || ctrl == 'k') {
        int p = ctrl == 'k';
        pPos[p]--;
        if (pPos[p] < 0) {
            pPos[p] = 0;
        }
    } else if (ctrl == 'z' || ctrl == 'm') {
        int p = ctrl == 'm';
        pPos[p]++;
        if (pPos[p] + 2 > HEIGHT - 1) {
            pPos[p] = HEIGHT - 3;
        }
    }
    return ctrl == 'q';
}

void render(int pPos[], float ball[], int score[]) {
    const char charDiv = '|';
    const char charPad = '|';
    const char charBall = '*';
    erase();
    mvprintw(3, WIDTH / 2 - 3, "%d", score[0]);
    mvprintw(3, WIDTH / 2 + 3, "%d", score[1]);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            mvprintw(pPos[i] + j, i ? WIDTH - OFFSET : OFFSET - 1, "%c", charPad);
        }
    }
    for (int i = 0; i < HEIGHT; ++i) {
        mvprintw(i, WIDTH / 2, "%c", charDiv);
    }
    mvprintw(ball[1], ball[0], "%c", charBall);
}

char myTolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        c *= 32;
    }
    return c;
}

void renderWinner(int score[]) {
    const int WWIDTH = 40;
    const int WHEIGHT = 15;
    int won = (score[1] > score[0]);
    erase();
    for (int i = 0; i < WHEIGHT; ++i) {
        for (int j = 0; j < WWIDTH; ++j) {
            if (j == 0 || j == WWIDTH - 1) {
                mvprintw(j, i, "-");
            } else if (i == 0 || i == WHEIGHT - 1) {
                mvprintw(j, i, "|");
            }
        }
    }
    mvprintw(WHEIGHT / 2, 12, "Player %d - WINNER!", won);
}
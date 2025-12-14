#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int W = 40;
const int H = 20;

struct Pos { int x, y; };
struct Enemy { Pos p; };

char map[H][W];
Pos player, keyPos, exitPos;
vector<Enemy> enemies;

int hp;
bool hasKey, gameOver, win;
int frame;

void initMap() {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (y == 0 || y == H - 1 || x == 0 || x == W - 1)
                map[y][x] = '#';
            else
                map[y][x] = '.';
        }
    }
}

void initGame() {
    initMap();
    player = {1, 1};
    keyPos = {W / 2, H / 2};
    exitPos = {W - 2, H - 2};

    enemies.clear();
    enemies.push_back({{W - 5, 2}});
    enemies.push_back({{2, H - 5}});

    hp = 100;
    hasKey = false;
    gameOver = false;
    win = false;
    frame = 0;
}

bool canWalk(int x, int y) {
    return map[y][x] == '.';
}

void movePlayer(int dx, int dy) {
    int nx = player.x + dx;
    int ny = player.y + dy;

    if (canWalk(nx, ny)) {
        player.x = nx;
        player.y = ny;
    }

    if (player.x == keyPos.x && player.y == keyPos.y)
        hasKey = true;

    if (player.x == exitPos.x && player.y == exitPos.y && hasKey)
        win = true;
}

void moveEnemies() {
    frame++;
    if (frame % 3 != 0) return; // MUSUH GERAK SETIAP 3 LANGKAH PLAYER

    for (auto &e : enemies) {
        int dx = (player.x > e.p.x) ? 1 : (player.x < e.p.x ? -1 : 0);
        int dy = (player.y > e.p.y) ? 1 : (player.y < e.p.y ? -1 : 0);

        int nx = e.p.x + dx;
        int ny = e.p.y + dy;

        if (canWalk(nx, ny)) {
            e.p.x = nx;
            e.p.y = ny;
        }

        if (e.p.x == player.x && e.p.y == player.y) {
            hp -= 5; // SERANG PELAN
            if (hp <= 0) gameOver = true;
        }
    }
}

void draw() {
    clear();
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            mvaddch(y, x, map[y][x]);

    if (!hasKey) mvaddch(keyPos.y, keyPos.x, 'K');
    mvaddch(exitPos.y, exitPos.x, 'X');

    for (auto &e : enemies)
        mvaddch(e.p.y, e.p.x, 'E');

    mvaddch(player.y, player.x, '@');

    mvprintw(0, W + 2, "HP: %d", hp);
    mvprintw(1, W + 2, "KEY: %s", hasKey ? "YES" : "NO");
    mvprintw(3, W + 2, "WASD = Move");

    if (gameOver)
        mvprintw(10, 5, "GAME OVER - R Restart | Q Quit");

    if (win)
        mvprintw(10, 5, "YOU WIN! - R Restart | Q Quit");

    refresh();
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    timeout(-1); // ⬅️ INPUT BLOKING, PLAYER PASTI GERAK

    initGame();

    while (true) {
        draw();
        int ch = getch();

        if (!gameOver && !win) {
            if (ch == 'w' || ch == 'W') movePlayer(0, -1);
            else if (ch == 's' || ch == 'S') movePlayer(0, 1);
            else if (ch == 'a' || ch == 'A') movePlayer(-1, 0);
            else if (ch == 'd' || ch == 'D') movePlayer(1, 0);
        } else {
            if (ch == 'r' || ch == 'R') initGame();
            else if (ch == 'q' || ch == 'Q') break;
        }

        moveEnemies();
    }

    endwin();
    return 0;
}

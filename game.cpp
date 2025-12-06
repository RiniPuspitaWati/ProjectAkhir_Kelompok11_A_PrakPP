#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

// Konstanta
#define MAP_WIDTH 50
#define MAP_HEIGHT 20
#define MAX_ENEMIES 10
#define MAX_ITEMS 5
#define SAVE_FILE "dungeon_save.txt"

// Enum untuk tipe tile
enum TileType {
    EMPTY,
    WALL,
    DOOR,
    TREASURE
};

// Enum untuk tipe item
enum ItemType {
    HEALTH_POTION,
    ATTACK_BOOST,
    DEFENSE_BOOST,
    KEY_ITEM
};

// Struct untuk posisi
struct Position {
    int x;
    int y;
};

// Struct untuk Player
struct Player {
    Position pos;
    char symbol;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int keys;
    int gold;
    int level;
    int exp;
    string name;
};

// Struct untuk Enemy
struct Enemy {
    Position pos;
    char symbol;
    int health;
    int maxHealth;
    int attack;
    bool isAlive;
    string name;
};

// Struct untuk Item
struct Item {
    Position pos;
    ItemType type;
    char symbol;
    int value;
    bool isCollected;
};

// Struct untuk Game State
struct GameState {
    Player player;
    vector<Enemy> enemies;
    vector<Item> items;
    TileType map[MAP_HEIGHT][MAP_WIDTH];
    int currentLevel;
    bool gameOver;
    bool victory;
    bool inCombat;
    Enemy* currentEnemy;
    string lastMessage;
};

// Global variable
GameState game;

// Fungsi utilitas Windows
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clearScreen() {
    system("cls");
}

void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// Deklarasi fungsi
void initGame();
void createPlayer();
void generateMap();
void generateEnemies();
void generateItems();
void gameLoop();
void handleInput(char key);
void movePlayer(int dx, int dy);
void updateGame();
void renderGame();
void renderStats();
void renderMessages(const string& msg);
void combat(Enemy* enemy);
void collectItem(Item* item);
void levelUp();
void saveGame();
void loadGame();
void showMenu();
void showGameOver();
void drawBox(int x, int y, int width, int height);

// Implementasi fungsi

void initGame() {
    srand(time(NULL));
    hideCursor();
    
    game.gameOver = false;
    game.victory = false;
    game.inCombat = false;
    game.currentEnemy = nullptr;
    game.currentLevel = 1;
    
    createPlayer();
    generateMap();
    generateEnemies();
    generateItems();
}

void createPlayer() {
    clearScreen();
    cout << "Enter your name: ";
    getline(cin, game.player.name);
    
    if (game.player.name.empty()) {
        game.player.name = "Hero";
    }
    
    game.player.pos.x = 2;
    game.player.pos.y = 2;
    game.player.symbol = '@';
    game.player.health = 100;
    game.player.maxHealth = 100;
    game.player.attack = 10;
    game.player.defense = 5;
    game.player.keys = 0;
    game.player.gold = 0;
    game.player.level = 1;
    game.player.exp = 0;
}

void generateMap() {
    // Initialize with empty
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game.map[y][x] = EMPTY;
        }
    }
    
    // Create walls (border)
    for (int x = 0; x < MAP_WIDTH; x++) {
        game.map[0][x] = WALL;
        game.map[MAP_HEIGHT - 1][x] = WALL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        game.map[y][0] = WALL;
        game.map[y][MAP_WIDTH - 1] = WALL;
    }
    
    // Create random walls (rooms)
    for (int i = 0; i < 15; i++) {
        int x = rand() % (MAP_WIDTH - 4) + 2;
        int y = rand() % (MAP_HEIGHT - 4) + 2;
        int width = rand() % 5 + 3;
        int height = rand() % 4 + 2;
        
        for (int dy = 0; dy < height; dy++) {
            for (int dx = 0; dx < width; dx++) {
                if (x + dx < MAP_WIDTH - 1 && y + dy < MAP_HEIGHT - 1) {
                    game.map[y + dy][x + dx] = WALL;
                }
            }
        }
    }
    
    // Place door (exit)
    game.map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = DOOR;
    
    // Place treasure
    int tx = rand() % (MAP_WIDTH - 10) + 5;
    int ty = rand() % (MAP_HEIGHT - 10) + 5;
    game.map[ty][tx] = TREASURE;
}

void generateEnemies() {
    game.enemies.clear();
    
    string enemyNames[] = {"Goblin", "Orc", "Skeleton", "Zombie", "Spider"};
    int enemyCount = 5 + rand() % 6;
    
    for (int i = 0; i < enemyCount; i++) {
        Enemy enemy;
        enemy.pos.x = rand() % (MAP_WIDTH - 4) + 2;
        enemy.pos.y = rand() % (MAP_HEIGHT - 4) + 2;
        
        while (game.map[enemy.pos.y][enemy.pos.x] != EMPTY ||
               (enemy.pos.x == game.player.pos.x && enemy.pos.y == game.player.pos.y)) {
            enemy.pos.x = rand() % (MAP_WIDTH - 4) + 2;
            enemy.pos.y = rand() % (MAP_HEIGHT - 4) + 2;
        }
        
        enemy.symbol = 'E';
        enemy.health = 20 + rand() % 30;
        enemy.maxHealth = enemy.health;
        enemy.attack = 5 + rand() % 8;
        enemy.isAlive = true;
        enemy.name = enemyNames[rand() % 5];
        
        game.enemies.push_back(enemy);
    }
}

void generateItems() {
    game.items.clear();
    int itemCount = 3 + rand() % 3;
    
    for (int i = 0; i < itemCount; i++) {
        Item item;
        item.pos.x = rand() % (MAP_WIDTH - 4) + 2;
        item.pos.y = rand() % (MAP_HEIGHT - 4) + 2;
        
        while (game.map[item.pos.y][item.pos.x] != EMPTY) {
            item.pos.x = rand() % (MAP_WIDTH - 4) + 2;
            item.pos.y = rand() % (MAP_HEIGHT - 4) + 2;
        }
        
        item.type = (ItemType)(rand() % 4);
        item.isCollected = false;
        
        switch (item.type) {
            case HEALTH_POTION:
                item.symbol = 'H';
                item.value = 30;
                break;
            case ATTACK_BOOST:
                item.symbol = 'A';
                item.value = 5;
                break;
            case DEFENSE_BOOST:
                item.symbol = 'D';
                item.value = 3;
                break;
            case KEY_ITEM:
                item.symbol = 'K';
                item.value = 1;
                break;
        }
        
        game.items.push_back(item);
    }
}

void drawBox(int x, int y, int width, int height) {
    gotoxy(x, y);
    cout << char(201); // Top-left corner
    for (int i = 0; i < width - 2; i++) cout << char(205); // Top border
    cout << char(187); // Top-right corner
    
    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        cout << char(186); // Left border
        gotoxy(x + width - 1, y + i);
        cout << char(186); // Right border
    }
    
    gotoxy(x, y + height - 1);
    cout << char(200); // Bottom-left corner
    for (int i = 0; i < width - 2; i++) cout << char(205); // Bottom border
    cout << char(188); // Bottom-right corner
}

void gameLoop() {
    showMenu();
    
    while (!game.gameOver) {
        renderGame();
        renderStats();
        
        if (_kbhit()) {
            char key = _getch();
            handleInput(key);
        }
        
        updateGame();
        Sleep(50);
    }
    
    showGameOver();
}

void handleInput(char key) {
    if (game.inCombat) {
        switch (key) {
            case '1':
                if (game.currentEnemy != nullptr) {
                    combat(game.currentEnemy);
                }
                break;
            case '2':
                game.inCombat = false;
                game.currentEnemy = nullptr;
                renderMessages("You fled from combat!");
                break;
        }
        return;
    }
    
    switch (key) {
        case 'w':
        case 'W':
        case 72: // Arrow Up
            movePlayer(0, -1);
            break;
        case 's':
        case 'S':
        case 80: // Arrow Down
            movePlayer(0, 1);
            break;
        case 'a':
        case 'A':
        case 75: // Arrow Left
            movePlayer(-1, 0);
            break;
        case 'd':
        case 'D':
        case 77: // Arrow Right
            movePlayer(1, 0);
            break;
        case 'i':
        case 'I':
            saveGame();
            renderMessages("Game saved!");
            break;
        case 'q':
        case 'Q':
            game.gameOver = true;
            break;
    }
}

void movePlayer(int dx, int dy) {
    int newX = game.player.pos.x + dx;
    int newY = game.player.pos.y + dy;
    
    if (newX < 1 || newX >= MAP_WIDTH - 1 || newY < 1 || newY >= MAP_HEIGHT - 1) {
        return;
    }
    
    if (game.map[newY][newX] == WALL) {
        return;
    }
    
    if (game.map[newY][newX] == DOOR) {
        if (game.player.keys > 0) {
            game.victory = true;
            game.gameOver = true;
            return;
        } else {
            renderMessages("Door is locked! Find a key!");
            return;
        }
    }
    
    if (game.map[newY][newX] == TREASURE) {
        game.player.gold += 100;
        game.map[newY][newX] = EMPTY;
        renderMessages("Found treasure! +100 gold");
    }
    
    // Check enemy collision
    for (size_t i = 0; i < game.enemies.size(); i++) {
        if (game.enemies[i].isAlive && 
            game.enemies[i].pos.x == newX && 
            game.enemies[i].pos.y == newY) {
            game.inCombat = true;
            game.currentEnemy = &game.enemies[i];
            renderMessages("Encountered " + game.enemies[i].name + "! HP: " + to_string(game.enemies[i].health));
            return;
        }
    }
    
    // Check item collision
    for (size_t i = 0; i < game.items.size(); i++) {
        if (!game.items[i].isCollected && 
            game.items[i].pos.x == newX && 
            game.items[i].pos.y == newY) {
            collectItem(&game.items[i]);
            return;
        }
    }
    
    game.player.pos.x = newX;
    game.player.pos.y = newY;
}

void combat(Enemy* enemy) {
    int damage = game.player.attack - (enemy->maxHealth / 20);
    if (damage < 1) damage = 1;
    enemy->health -= damage;
    
    string msg = "You hit " + enemy->name + " for " + to_string(damage) + " damage! ";
    
    if (enemy->health <= 0) {
        enemy->isAlive = false;
        game.inCombat = false;
        game.currentEnemy = nullptr;
        int expGain = 15 + rand() % 10;
        game.player.exp += expGain;
        game.player.gold += 10 + rand() % 20;
        msg += "Enemy defeated!";
        
        if (game.player.exp >= game.player.level * 50) {
            levelUp();
        }
    } else {
        int enemyDamage = enemy->attack - game.player.defense;
        if (enemyDamage < 1) enemyDamage = 1;
        game.player.health -= enemyDamage;
        
        msg += enemy->name + " hits you for " + to_string(enemyDamage) + " damage!";
        
        if (game.player.health <= 0) {
            game.gameOver = true;
        }
    }
    
    renderMessages(msg);
}

void collectItem(Item* item) {
    string msg;
    item->isCollected = true;
    
    switch (item->type) {
        case HEALTH_POTION:
            game.player.health += item->value;
            if (game.player.health > game.player.maxHealth) {
                game.player.health = game.player.maxHealth;
            }
            msg = "Found Health Potion! +" + to_string(item->value) + " HP";
            break;
        case ATTACK_BOOST:
            game.player.attack += item->value;
            msg = "Found Attack Boost! +" + to_string(item->value) + " Attack";
            break;
        case DEFENSE_BOOST:
            game.player.defense += item->value;
            msg = "Found Defense Boost! +" + to_string(item->value) + " Defense";
            break;
        case KEY_ITEM:
            game.player.keys += item->value;
            msg = "Found a Key! Keys: " + to_string(game.player.keys);
            break;
    }
    
    renderMessages(msg);
}

void levelUp() {
    game.player.level++;
    game.player.maxHealth += 20;
    game.player.health = game.player.maxHealth;
    game.player.attack += 3;
    game.player.defense += 2;
    
    renderMessages("LEVEL UP! Now Level " + to_string(game.player.level));
}

void updateGame() {
    // Simple enemy AI
    for (size_t i = 0; i < game.enemies.size(); i++) {
        if (!game.enemies[i].isAlive) continue;
        
        if (rand() % 5 == 0) {
            int dx = (rand() % 3) - 1;
            int dy = (rand() % 3) - 1;
            
            int newX = game.enemies[i].pos.x + dx;
            int newY = game.enemies[i].pos.y + dy;
            
            if (newX > 0 && newX < MAP_WIDTH - 1 && 
                newY > 0 && newY < MAP_HEIGHT - 1 &&
                game.map[newY][newX] == EMPTY) {
                game.enemies[i].pos.x = newX;
                game.enemies[i].pos.y = newY;
            }
        }
    }
}

void renderGame() {
    gotoxy(0, 0);
    
    // Draw map
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char ch = ' ';
            int color = 7; // White
            
            switch (game.map[y][x]) {
                case WALL: 
                    ch = '#'; 
                    color = 8; // Dark gray
                    break;
                case DOOR: 
                    ch = 'X'; 
                    color = 14; // Yellow
                    break;
                case TREASURE: 
                    ch = '$'; 
                    color = 6; // Yellow
                    break;
                default: 
                    ch = '.'; 
                    color = 8;
                    break;
            }
            
            setColor(color);
            cout << ch;
        }
        cout << endl;
    }
    
    // Draw items
    for (size_t i = 0; i < game.items.size(); i++) {
        if (!game.items[i].isCollected) {
            gotoxy(game.items[i].pos.x, game.items[i].pos.y);
            setColor(11); // Cyan
            cout << game.items[i].symbol;
        }
    }
    
    // Draw enemies
    for (size_t i = 0; i < game.enemies.size(); i++) {
        if (game.enemies[i].isAlive) {
            gotoxy(game.enemies[i].pos.x, game.enemies[i].pos.y);
            setColor(12); // Red
            cout << game.enemies[i].symbol;
        }
    }
    
    // Draw player
    gotoxy(game.player.pos.x, game.player.pos.y);
    setColor(10); // Green
    cout << game.player.symbol;
    
    setColor(7); // Reset color
}

void renderStats() {
    int statX = MAP_WIDTH + 2;
    
    gotoxy(statX, 0);
    setColor(15);
    cout << "=== " << game.player.name << " ===" << endl;
    gotoxy(statX, 2);
    cout << "Level: " << game.player.level << endl;
    gotoxy(statX, 3);
    cout << "EXP: " << game.player.exp << "/" << game.player.level * 50 << endl;
    gotoxy(statX, 4);
    cout << "HP: " << game.player.health << "/" << game.player.maxHealth << endl;
    gotoxy(statX, 5);
    cout << "Attack: " << game.player.attack << endl;
    gotoxy(statX, 6);
    cout << "Defense: " << game.player.defense << endl;
    gotoxy(statX, 7);
    cout << "Gold: " << game.player.gold << endl;
    gotoxy(statX, 8);
    cout << "Keys: " << game.player.keys << endl;
    
    gotoxy(statX, 10);
    cout << "--- Controls ---" << endl;
    gotoxy(statX, 11);
    cout << "WASD: Move" << endl;
    gotoxy(statX, 12);
    cout << "I: Save" << endl;
    gotoxy(statX, 13);
    cout << "Q: Quit" << endl;
    
    if (game.inCombat) {
        gotoxy(statX, 15);
        setColor(12);
        cout << "--- COMBAT ---" << endl;
        gotoxy(statX, 16);
        cout << "1: Attack" << endl;
        gotoxy(statX, 17);
        cout << "2: Flee" << endl;
        setColor(7);
    }
}

void renderMessages(const string& msg) {
    game.lastMessage = msg;
    gotoxy(0, MAP_HEIGHT + 1);
    setColor(14);
    cout << ">> " << msg;
    for (int i = msg.length(); i < MAP_WIDTH + 30; i++) cout << " ";
    setColor(7);
}

void saveGame() {
    ofstream file(SAVE_FILE);
    if (file.is_open()) {
        file << game.player.name << endl;
        file << game.player.pos.x << " " << game.player.pos.y << endl;
        file << game.player.health << " " << game.player.attack << " ";
        file << game.player.defense << " " << game.player.level << endl;
        file << game.player.exp << " " << game.player.gold << " " << game.player.keys << endl;
        file.close();
    }
}

void loadGame() {
    ifstream file(SAVE_FILE);
    if (file.is_open()) {
        getline(file, game.player.name);
        file >> game.player.pos.x >> game.player.pos.y;
        file >> game.player.health >> game.player.attack;
        file >> game.player.defense >> game.player.level;
        file >> game.player.exp >> game.player.gold >> game.player.keys;
        file.close();
    }
}

void showMenu() {
    clearScreen();
    setColor(14);
    cout << "\n\n";
    cout << "  ╔════════════════════════════════╗\n";
    cout << "  ║   DUNGEON EXPLORER GAME        ║\n";
    cout << "  ╚════════════════════════════════╝\n\n";
    setColor(7);
    cout << "  Objective: Find the key and\n";
    cout << "             reach the exit (X)\n\n";
    cout << "  Symbols:\n";
    cout << "    @ = You\n";
    cout << "    E = Enemy\n";
    cout << "    H = Health Potion\n";
    cout << "    A = Attack Boost\n";
    cout << "    D = Defense Boost\n";
    cout << "    K = Key\n";
    cout << "    $ = Treasure\n";
    cout << "    X = Exit Door\n\n";
    setColor(11);
    cout << "  Press any key to start...\n";
    setColor(7);
    
    _getch();
    clearScreen();
}

void showGameOver() {
    clearScreen();
    
    if (game.victory) {
        setColor(10);
        cout << "\n\n";
        cout << "  ╔════════════════════════╗\n";
        cout << "  ║    VICTORY!!!          ║\n";
        cout << "  ╚════════════════════════╝\n\n";
    } else {
        setColor(12);
        cout << "\n\n";
        cout << "  ╔════════════════════════╗\n";
        cout << "  ║    GAME OVER           ║\n";
        cout << "  ╚════════════════════════╝\n\n";
    }
    
    setColor(7);
    cout << "  Final Stats:\n";
    cout << "    Level: " << game.player.level << endl;
    cout << "    Gold Collected: " << game.player.gold << endl;
    
    int defeated = 0;
    for (size_t i = 0; i < game.enemies.size(); i++) {
        if (!game.enemies[i].isAlive) defeated++;
    }
    cout << "    Enemies Defeated: " << defeated << endl;
    
    cout << "\n  Thanks for playing, " << game.player.name << "!\n";
    cout << "  Press any key to exit...\n";
    
    _getch();
}

int main() {
    initGame();
    gameLoop();
    
    return 0;
}
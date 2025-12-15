#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============ KONSTANTA ============
const int LEBAR_MAP = 30;
const int TINGGI_MAP = 12;

// ============ WARNA TERMINAL ============
const string RESET = "\033[0m";
const string MERAH = "\033[1;31m";
const string HIJAU = "\033[1;32m";
const string KUNING = "\033[1;33m";
const string BIRU = "\033[1;34m";
const string MAGENTA = "\033[1;35m";
const string CYAN = "\033[1;36m";
const string PUTIH = "\033[1;37m";
const string ABU = "\033[0;37m";

// ============ FUNGSI UTILITY ============
void bersihkanLayar() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ============ CLASS PEMAIN ============
class Pemain {
private:
    int x, y;
    int hp, maxHp;
    int serangan;
    int kunci;
    int emas;
    
public:
    Pemain(int posX, int posY) {
        x = posX;
        y = posY;
        hp = 100;
        maxHp = 100;
        serangan = 15;
        kunci = 0;
        emas = 0;
    }
    
    // Getter
    int getX() { return x; }
    int getY() { return y; }
    int getHp() { return hp; }
    int getMaxHp() { return maxHp; }
    int getSerangan() { return serangan; }
    int getKunci() { return kunci; }
    int getEmas() { return emas; }
    
    // Aksi
    void gerak(int dx, int dy) {
        x += dx;
        y += dy;
    }
    
    void kenaSerangan(int damage) {
        hp -= damage;
        if(hp < 0) hp = 0;
    }
    
    void sembuh(int jumlah) {
        hp += jumlah;
        if(hp > maxHp) hp = maxHp;
    }
    
    void tambahKunci() { kunci++; }
    void pakaiKunci() { if(kunci > 0) kunci--; }
    void tambahEmas(int jumlah) { emas += jumlah; }
    void tambahSerangan(int jumlah) { serangan += jumlah; }
    
    bool masihHidup() { return hp > 0; }
};

// ============ CLASS MONSTER ============
class Monster {
private:
    int x, y;
    int hp;
    int serangan;
    string nama;
    char simbol;
    
public:
    Monster(int posX, int posY, string nm, int health, int atk, char sym) {
        x = posX;
        y = posY;
        nama = nm;
        hp = health;
        serangan = atk;
        simbol = sym;
    }
    
    int getX() { return x; }
    int getY() { return y; }
    int getHp() { return hp; }
    int getSerangan() { return serangan; }
    string getNama() { return nama; }
    char getSimbol() { return simbol; }
    
    void kenaSerangan(int damage) {
        hp -= damage;
        if(hp < 0) hp = 0;
    }
    
    bool masihHidup() { return hp > 0; }
};

// ============ CLASS MAP ============
class Map {
private:
    vector<vector<char>> tiles;
    
public:
    Map() {
        tiles.resize(TINGGI_MAP, vector<char>(LEBAR_MAP, '#'));
        buatRuangan();
    }
    
    void buatRuangan() {
        // Ruangan 1 (kiri atas)
        for(int i = 2; i < 6; i++) {
            for(int j = 2; j < 12; j++) {
                tiles[i][j] = '.';
            }
        }
        
        // Ruangan 2 (kanan atas)
        for(int i = 2; i < 6; i++) {
            for(int j = 18; j < 28; j++) {
                tiles[i][j] = '.';
            }
        }
        
        // Ruangan 3 (kiri bawah)
        for(int i = 7; i < 10; i++) {
            for(int j = 2; j < 12; j++) {
                tiles[i][j] = '.';
            }
        }
        
        // Koridor horizontal
        for(int j = 12; j < 18; j++) {
            tiles[3][j] = '.';
        }
        
        // Koridor vertikal
        for(int i = 6; i < 7; i++) {
            tiles[i][7] = '.';
        }
        
        // Pintu dan Exit
        tiles[3][15] = '+';  // Pintu
        tiles[8][10] = 'E';  // Exit
    }
    
    char getTile(int x, int y) {
        if(x < 0 || x >= LEBAR_MAP || y < 0 || y >= TINGGI_MAP) 
            return '#';
        return tiles[y][x];
    }
    
    void setTile(int x, int y, char c) {
        if(x >= 0 && x < LEBAR_MAP && y >= 0 && y < TINGGI_MAP) {
            tiles[y][x] = c;
        }
    }
    
    bool bisaDilewati(int x, int y) {
        char t = getTile(x, y);
        return (t == '.' || t == 'E');
    }
};

// ============ CLASS GAME ============
class Game {
private:
    Pemain* pemain;
    Map* peta;
    vector<Monster*> monsters;
    vector<string> pesan;
    bool jalan;
    
    // Item posisi
    int posKunciX, posKunciY;
    int posRamuan1X, posRamuan1Y;
    int posRamuan2X, posRamuan2Y;
    int posEmasX, posEmasY;
    bool kunciAda, ramuan1Ada, ramuan2Ada, emasAda;
    
public:
    Game() {
        peta = new Map();
        pemain = new Pemain(3, 3);
        jalan = true;
        
        // Spawn monsters
        monsters.push_back(new Monster(20, 3, "Goblin", 40, 8, 'G'));
        monsters.push_back(new Monster(8, 8, "Orc", 60, 12, 'O'));
        
        // Spawn items
        posKunciX = 5; posKunciY = 3;
        posRamuan1X = 23; posRamuan1Y = 4;
        posRamuan2X = 3; posRamuan2Y = 8;
        posEmasX = 25; posEmasY = 3;
        
        kunciAda = true;
        ramuan1Ada = true;
        ramuan2Ada = true;
        emasAda = true;
        
        tambahPesan("Selamat datang di Dungeon!");
        tambahPesan("Cari Exit (E) untuk menang!");
        tambahPesan("WASD = Gerak, Q = Keluar");
    }
    
    ~Game() {
        delete pemain;
        delete peta;
        for(auto m : monsters) delete m;
    }
    
    void tambahPesan(string msg) {
        pesan.push_back(msg);
        if(pesan.size() > 3) {
            pesan.erase(pesan.begin());
        }
    }
    
    void prosesInput(char input) {
        int newX = pemain->getX();
        int newY = pemain->getY();
        
        // Tentukan arah
        if(input == 'w' || input == 'W') newY--;
        else if(input == 's' || input == 'S') newY++;
        else if(input == 'a' || input == 'A') newX--;
        else if(input == 'd' || input == 'D') newX++;
        else if(input == 'q' || input == 'Q') {
            jalan = false;
            return;
        }
        else return;
        
        // Cek pintu
        if(peta->getTile(newX, newY) == '+') {
            if(pemain->getKunci() > 0) {
                pemain->pakaiKunci();
                peta->setTile(newX, newY, '.');
                tambahPesan("Pintu terbuka!");
            } else {
                tambahPesan("Pintu terkunci! Butuh kunci.");
                return;
            }
        }
        
        // Cek monster
        for(int i = 0; i < monsters.size(); i++) {
            if(monsters[i]->getX() == newX && monsters[i]->getY() == newY) {
                if(monsters[i]->masihHidup()) {
                    combat(monsters[i]);
                    return;
                }
            }
        }
        
        // Cek Exit
        if(peta->getTile(newX, newY) == 'E') {
            tambahPesan("SELAMAT! Kamu menemukan exit!");
            jalan = false;
            return;
        }
        
        // Cek bisa jalan
        if(!peta->bisaDilewati(newX, newY)) {
            tambahPesan("Tidak bisa lewat!");
            return;
        }
        
        // Gerakkan pemain
        pemain->gerak(newX - pemain->getX(), newY - pemain->getY());
        
        // Cek item
        cekItem();
    }
    
    void combat(Monster* monster) {
        int dmgPemain = pemain->getSerangan() + (rand() % 5);
        int dmgMonster = monster->getSerangan() + (rand() % 3);
        
        monster->kenaSerangan(dmgPemain);
        tambahPesan("Kamu serang " + monster->getNama() + " -" + to_string(dmgPemain) + "HP");
        
        if(monster->masihHidup()) {
            pemain->kenaSerangan(dmgMonster);
            tambahPesan(monster->getNama() + " serang balik -" + to_string(dmgMonster) + "HP");
        } else {
            tambahPesan(monster->getNama() + " kalah!");
        }
    }
    
    void cekItem() {
        int px = pemain->getX();
        int py = pemain->getY();
        
        // Kunci
        if(kunciAda && px == posKunciX && py == posKunciY) {
            pemain->tambahKunci();
            kunciAda = false;
            tambahPesan("Dapat Kunci!");
        }
        
        // Ramuan 1
        if(ramuan1Ada && px == posRamuan1X && py == posRamuan1Y) {
            pemain->sembuh(30);
            ramuan1Ada = false;
            tambahPesan("Dapat Ramuan! +30 HP");
        }
        
        // Ramuan 2
        if(ramuan2Ada && px == posRamuan2X && py == posRamuan2Y) {
            pemain->tambahSerangan(5);
            ramuan2Ada = false;
            tambahPesan("Dapat Ramuan Kekuatan! +5 ATK");
        }
        
        // Emas
        if(emasAda && px == posEmasX && py == posEmasY) {
            pemain->tambahEmas(100);
            emasAda = false;
            tambahPesan("Dapat Emas! +100");
        }
    }
    
    void render() {
        bersihkanLayar();
        
        cout << KUNING << "\n ╔══════════════════════════════╗\n";
        cout << " ║     DUNGEON PETUALANGAN      ║\n";
        cout << " ╚══════════════════════════════╝" << RESET << "\n\n";
        
        // Gambar map
        for(int i = 0; i < TINGGI_MAP; i++) {
            cout << "  ";
            for(int j = 0; j < LEBAR_MAP; j++) {
                bool digambar = false;
                
                // Pemain
                if(pemain->getX() == j && pemain->getY() == i) {
                    cout << KUNING << '@' << RESET;
                    digambar = true;
                }
                
                // Monster
                if(!digambar) {
                    for(auto m : monsters) {
                        if(m->masihHidup() && m->getX() == j && m->getY() == i) {
                            cout << MERAH << m->getSimbol() << RESET;
                            digambar = true;
                            break;
                        }
                    }
                }
                
                // Item
                if(!digambar) {
                    if(kunciAda && j == posKunciX && i == posKunciY) {
                        cout << CYAN << 'K' << RESET;
                        digambar = true;
                    }
                    else if(ramuan1Ada && j == posRamuan1X && i == posRamuan1Y) {
                        cout << HIJAU << 'R' << RESET;
                        digambar = true;
                    }
                    else if(ramuan2Ada && j == posRamuan2X && i == posRamuan2Y) {
                        cout << MAGENTA << 'S' << RESET;
                        digambar = true;
                    }
                    else if(emasAda && j == posEmasX && i == posEmasY) {
                        cout << KUNING << '$' << RESET;
                        digambar = true;
                    }
                }
                
                // Tile
                if(!digambar) {
                    char tile = peta->getTile(j, i);
                    if(tile == '#') cout << ABU << tile << RESET;
                    else if(tile == '+') cout << CYAN << tile << RESET;
                    else if(tile == 'E') cout << HIJAU << tile << RESET;
                    else cout << tile;
                }
            }
            cout << endl;
        }
        
        // UI dengan warna
        cout << BIRU << "\n  ══════════════════════════════════════" << RESET << "\n";
        
        // HP dengan warna kondisional
        cout << "  ";
        if(pemain->getHp() > 60) cout << HIJAU;
        else if(pemain->getHp() > 30) cout << KUNING;
        else cout << MERAH;
        cout << "HP: " << pemain->getHp() << "/" << pemain->getMaxHp() << RESET;
        
        cout << " | " << MERAH << "ATK: " << pemain->getSerangan() << RESET;
        cout << " | " << CYAN << "Kunci: " << pemain->getKunci() << RESET;
        cout << " | " << KUNING << "Emas: " << pemain->getEmas() << RESET << "\n";
        
        cout << BIRU << "  ══════════════════════════════════════" << RESET << "\n";
        
        // Pesan
        for(auto& msg : pesan) {
            cout << PUTIH << "  » " << msg << RESET << endl;
        }
        
        cout << "\n  " << ABU << "Perintah: " << HIJAU << "[W]" << ABU << " atas " 
             << HIJAU << "[A]" << ABU << " kiri " << HIJAU << "[S]" << ABU << " bawah " 
             << HIJAU << "[D]" << ABU << " kanan " << MERAH << "[Q]" << ABU << " keluar" << RESET << "\n";
        cout << "  " << PUTIH << ">>> " << RESET;
    }
    
    bool masihJalan() {
        return jalan && pemain->masihHidup();
    }
    
    void gameOver() {
        bersihkanLayar();
        cout << "\n\n";
        
        if(pemain->masihHidup()) {
            cout << HIJAU;
            cout << "  ╔════════════════════════════════════╗\n";
            cout << "  ║                                    ║\n";
            cout << "  ║     🎉  SELAMAT! KAMU MENANG! 🎉   ║\n";
            cout << "  ║                                    ║\n";
            cout << "  ╚════════════════════════════════════╝\n" << RESET;
        } else {
            cout << MERAH;
            cout << "  ╔════════════════════════════════════╗\n";
            cout << "  ║                                    ║\n";
            cout << "  ║       💀  GAME OVER - KALAH  💀    ║\n";
            cout << "  ║                                    ║\n";
            cout << "  ╚════════════════════════════════════╝\n" << RESET;
        }
        
        cout << "\n";
        cout << PUTIH << "  ┌─────────────────────────────────┐\n";
        cout << "  │  " << HIJAU << "HP Akhir  : " << RESET;
        if(pemain->getHp() > 0) cout << HIJAU;
        else cout << MERAH;
        cout << pemain->getHp() << " HP" << PUTIH << "                │\n";
        
        cout << "  │  " << KUNING << "Emas      : " << pemain->getEmas() << " koin" << PUTIH << "             │\n";
        cout << "  └─────────────────────────────────┘" << RESET << "\n\n";
        
        cout << CYAN << "  Terima kasih sudah bermain!" << RESET << "\n";
        cout << BIRU << "  ═══════════════════════════════════\n" << RESET << "\n";
    }
};

// ============ MAIN ============
int main() {
    srand(time(0));
    
    Game game;
    
    while(game.masihJalan()) {
        game.render();
        
        char input;
        cin >> input;
        
        game.prosesInput(input);
    }
    
    game.gameOver();
    
    return 0;
}
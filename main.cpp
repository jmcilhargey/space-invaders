#include <iostream>
// #include "windows.h"
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <Carbon/Carbon.h>
#include <curses.h>

using namespace std;

const int INIT_INVADERS = 8;
WINDOW *stdscr;

struct invObj {
    
    const char alien = 'A';
    bool alive = true;
    int x;
    int y;
    bool moveRight = true;
    
    invObj(int xVal, int yVal) {
        
        x = xVal;
        y = yVal;
    }
    
    //void kill(){ alive = false; }
    
};

struct pacObj {
    
    const char pacCh = '^';
    int x;
    int y;
    void takestep(vector <string> &mymap, bool rightDir);
    
    pacObj() {
        x = 16;
        y = 2;
    }
    
};

int move(int y, int x);
int wmove(WINDOW *win, int y, int x);
int refresh(void);
int wrefresh(WINDOW *win);

bool isPressed(unsigned short inKeyCode) {
    
    unsigned char keyMap[16];
    GetKeys((BigEndianUInt32*) &keyMap);
    return (0 != ((keyMap[ inKeyCode >> 3] >> (inKeyCode & 7)) & 1));
}

//sets cursor to position 0,0 .. no need to modify
/*
bool gotoxy(const WORD x, const WORD y) {
    
    COORD xy;
    xy.X = x;
    xy.Y = y;
    
    return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
}*/

vector<string> loadMap(string myMap){
    
    fstream file;
    string line;
    vector<string> mapFile;
    
    file.open(myMap, ios::in);
    
    
    if (file) {
        
        while (getline(file, line)) {
        
            mapFile.push_back(line);
        }
    } else {
        cout << "Map not found." << endl;
    }
    
    file.close();
    
    return mapFile;
}

//init all elements related to the game (your initial location, aliens, map, etc)

void init(vector <string> &gameMap, vector<invObj> &invaders){
    
    string mapName;
    
    for (int i = 0; i < INIT_INVADERS; i++) {
        
        invObj invader(1, i + 1);
        invaders.push_back(invader);
    }
    
    cout << "Enter the name of the map: ";
    getline(cin, mapName);

    gameMap = loadMap("/Users/jmcilhargey/Downloads/map3.txt");
}

//draws to the screen

void updateScreen(vector<string> &myMap) {
    
    // initscr();
    // move(0, 0);
    // wmove(stdscr, 0, 0);
    // refresh();
    
    for (string line : myMap) {
        cout << line << endl;
    }
    
}

//move invaders 1 step

void moveInvaders(vector<string> &myMap, vector<invObj> &invaders){
    
    if (invaders.back().moveRight) {
        
        if (myMap.at(invaders.back().x).at(invaders.back().y + 1) == '#') {
            
            for (invObj &invader : invaders) {
                invader.moveRight = false;
                
                for (invObj &invader : invaders) {
                    myMap.at(invader.x).at(invader.y) = ' ';
                }
                invader.x++;
            }
        } else {
            
            for (invObj &invader : invaders) {
                invader.y++;
            }
        }
        myMap.at(invaders.front().x).at(invaders.front().y - 1) = ' ';
        
    } else {
        
        if (myMap.at(invaders.front().x).at(invaders.front().y - 1) == '#') {
            
            for (invObj &invader : invaders) {
                
                invader.moveRight = true;
    
                for (invObj &invader : invaders) {
                    myMap.at(invader.x).at(invader.y) = ' ';
                }
                invader.x++;
            }
        } else {
            
            for (invObj &invader : invaders) {
                invader.y--;
            }
        }
        myMap.at(invaders.back().x).at(invaders.back().y + 1) = ' ';
    }
    
    for (invObj &invader : invaders) {
        myMap.at(invader.x).at(invader.y) = invader.alien;
    }
}

//move you one step

void pacObj::takestep(vector <string> &myMap, bool rightDir){
    
    if (rightDir && myMap.at(x).at(y + 1) != '#') {
        myMap.at(x).at(y) = ' ';
        y++;
    } else if (!rightDir && myMap.at(x).at(y - 1) != '#') {
        myMap.at(x).at(y) = ' ';
        y--;
    }
    
    myMap.at(x).at(y) = pacCh;
}

int main() {
    
    int speed = 100;
    int score = 0;
    bool game_running = true;
    vector<string> gameMap;
    vector<invObj> invaders;
    pacObj pac;

    init(gameMap, invaders);
    
    while (game_running) {
        
        this_thread::sleep_for(chrono::milliseconds(speed));
        
        moveInvaders(gameMap, invaders);
        updateScreen(gameMap);
        pac.takestep(gameMap, true);
    }

    do {
        
        if (isPressed(29)) {

            pac.takestep(gameMap, true);
        }
        if (isPressed(28)) {
            
            pac.takestep(gameMap, false);
        }
        if (isPressed(31)) {
    
            speed += 10;
        }
        if (isPressed(30)) {
            
            speed -= 10;
        }
        if (isPressed(27)){
            
            game_running = false;
        }
        updateScreen(gameMap);
    
    } while (game_running == true && score != INIT_INVADERS);

    //    system("cls");
    
    cout << "\n\nGAME OVER";
    
    system("pause>nul");
    
    return 0;
}
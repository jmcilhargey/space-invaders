#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono> // Used for sleep delay
#include <Carbon/Carbon.h> // Used for key press
#include <ncurses.h> // Used for clear screen

/* To get working in terminal, include the Carbon.framework and libncurses.tbd in Xcode using 'Link Binary With Libraries' */

using namespace std;

const int INIT_INVADERS = 8;

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
    void kill(){
        alive = false;
    }
};

struct pacObj {
    
    const char pacCh = '^';
    int x;
    int y;
    void takeStep(vector <string> &myMap, bool rightDir);
    
    pacObj() {
        x = 16;
        y = 3;
    }
};

struct shotObj {
    const char shotCh = '!';
    int x;
    int y;
    bool currShot = false;
    void takeShot(vector <string> &myMap, const pacObj &pac, vector<invObj> &invaders, int &score, bool fire);
};

// Mac version to set cursor position to 0, 0

void gotoxy(int x, int y) {
    printf("%c[%d;%df",0x1B,y,x);
}

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

// Initialize invaders, ask for map name, and load map

void init(vector <string> &gameMap, vector<invObj> &invaders){
    
    string mapName;
    
    for (int i = 0; i < INIT_INVADERS; i++) {
        
        invObj invader(1, i + 1);
        invaders.push_back(invader);
    }
    
    cout << "Enter the name of the map: ";
    getline(cin, mapName);

    gameMap = loadMap("/Users/jmcilhargey/Documents/comp-sci-assignments/Space_Invaders/Space_Invaders/map3.txt");
}

// Draws map to the screen

void updateScreen(vector<string> &myMap) {
    
    for (string line : myMap) {
        cout << line << endl;
    }
    
}

// Moves invaders over 1 character

void moveInvaders(vector<string> &myMap, vector<invObj> &invaders) {
    
    char nextStep = myMap.at(invaders.at((invaders.size() - 1) * invaders.at(0).moveRight).x).at(invaders.at((invaders.size() - 1) * invaders.at(0).moveRight).y + 2 * invaders.at(0).moveRight - 1);
    
    switch (nextStep) {
        case '#'  :
            for (invObj &invader : invaders) {
                invader.moveRight = !invader.moveRight;
                myMap.at(invader.x).at(invader.y) = ' ';
                invader.x++;
            }
            break;
        default :
            for (invObj &invader : invaders) {
                invader.y = invader.y + 2 * (invader.moveRight) - 1;
            }
            myMap.at(invaders.at((invaders.size() - 1) * !invaders.at(0).moveRight).x).at(invaders.at((invaders.size() - 1) * !invaders.at(0).moveRight).y - 2 * invaders.at(0).moveRight + 1) = ' ';
            break;
    };

    for (invObj &invader : invaders) {
        if (invader.alive) {
            myMap.at(invader.x).at(invader.y) = invader.alien;
        } else {
            myMap.at(invader.x).at(invader.y) = ' ';
        }
    }

}

// Moves the ship 1 character

void pacObj::takeStep(vector <string> &myMap, bool rightDir) {
    
    char nextStep = myMap.at(x).at(y - 1 + 2 * rightDir);
    
    switch (nextStep) {
        case ' ':
            myMap.at(x).at(y) = ' ';
            y += 2 * rightDir - 1;
            myMap.at(x).at(y) = pacCh;
            break;
    };
}

// Takes a new shot or moves the shot 1 character

void shotObj::takeShot(vector <string> &myMap, const pacObj &pac, vector<invObj> &invaders, int &score, bool fire) {
    
    if (fire && !currShot) {
        currShot = true;
        x = pac.x - 1;
        y = pac.y;
    }
    
    if (currShot) {
        
        char nextStep = myMap.at(x - 1).at(y);
        
        myMap.at(x).at(y) = ' ';
        
        switch (nextStep) {
        case ' ':
                x--;
                myMap.at(x).at(y) = shotCh;
            break;
        case '#':
                currShot = false;
            break;
        case 'x':
                myMap.at(x - 1).at(y) = ' ';
                currShot = false;
            break;
        case 'A':
                /*
                if (invaders.at(invaders.size() - 1).y == y) {
                    invaders.pop_back();
                }*/
                for (invObj &invader : invaders) {
                    if (invader.y == y) {
                        invader.kill();
                    }
                }
                score++;
                currShot = false;
            break;
        }
    }
    
    
}

int main() {
    
    int speed = 100;
    int score = 0;
    bool game_running = true;
    vector<string> gameMap;
    vector<invObj>  invaders;
    pacObj pac;
    shotObj shot;
    
    init(gameMap, invaders);
    
    // Using ncurses, clear the screen and refresh
    initscr();
    refresh();
    
    do {

        gotoxy(0, 0);
        
        this_thread::sleep_for(chrono::milliseconds(speed));
        
        moveInvaders(gameMap, invaders);
        shot.takeShot(gameMap, pac, invaders, score, false);
        updateScreen(gameMap);
        
        gameMap.at(pac.x).at(pac.y) = pac.pacCh;
        
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_LeftArrow)) {

            pac.takeStep(gameMap, false);
        }
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_RightArrow)) {
            
            pac.takeStep(gameMap, true);
        }
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_Space)) {
            
           shot.takeShot(gameMap, pac, invaders, score, true);
        }
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_DownArrow)) {
    
            speed += 10;
        }
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_UpArrow)) {
            
            speed -= 10;
        }
        if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_Escape)){
            
            game_running = false;
        }
        
        cout << "Current score: " << score << endl;
    
    } while (game_running && score != INIT_INVADERS);

    // system("cls");
    if (score == INIT_INVADERS) {
        cout << "\n\nYOU WIN!";
    }
    
    // system("pause>nul");
    
    return 0;
}
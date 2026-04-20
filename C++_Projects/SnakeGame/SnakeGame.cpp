#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstdlib> 
#include <ctime>
#include <vector>
#include <utility>
#include <deque>
using namespace std;

deque<pair<int, int>> deq; // Deque to store the snake's body as pairs (x, y)

void setCursorPosition(short x, short y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { x, y };
    SetConsoleCursorPosition(hOut, position);
}

void hideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void showCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

enum D { top = 1, down, left, right };

class map {
public: 
    int width, height, fruitx, fruity;
};

class snake {
public:
    int headx, heady;
    D dir;
};

class player {
public:
    int score;
    bool lose;
};

// Global objects
map m;
snake s;
player p;
static int lev = 120;
// Function declarations
void setup();
void game_board();
void input_game();
void movement();
void end();

//-------------------------------------------------------
void setup() {
    srand(time(NULL));
    deq.clear();
    m.width = 40;
    m.height = 20;
    m.fruitx = rand() % (m.width - 2) + 1;
    m.fruity = rand() % (m.height - 2) + 1;
    s.headx = m.width / 2;
    s.heady = m.height / 2;
    s.dir = D::right;
    p.score = 0;
    p.lose = false;
    deq.push_front({ s.headx, s.heady });
}

//-------------------------------------------------------
void game_board() {
    setCursorPosition(0, 0);

    // Top border
    for (int i = 0; i < m.width; i++) {
        cout << "-";
    }
    cout << endl;

    // Game area
    for (int u = 0; u < m.height; u++) {
        cout << "|";
        for (int i = 1; i < m.width - 1; i++) {
            bool printed = false;
            // Check if the current position is part of the snake's body
            for (size_t idx = 0; idx < deq.size(); idx++) {
                if (deq[idx].first == i && deq[idx].second == u) {
                    if (idx == 0) {
                        cout << "O";  // Snake's head (first element in deque)
                    }
                    else {
                        cout << "o";  // Snake's body (rest of the deque)
                    }
                    printed = true;
                    break;
                }
            }

            if (!printed) {
                if (u == m.fruity && i == m.fruitx) {
                    cout << "$";  // Fruit
                }
                else {
                    cout << " ";  // Empty space
                }
            }
        }
       cout << "|" << endl;
    }

    // Bottom border
    for (int i = 0; i < m.width; i++) {
        cout << "-";
    }
    cout << endl;

    // Display the score
    cout << "The player's score is -> " << p.score << endl;
}

//-------------------------------------------------------
void input_game() {
    if (_kbhit()) {
        char c = _getch();
        switch (c) {
        case 'w':
            s.dir = top;
            break;
        case 's':
            s.dir = down;
            break;
        case 'a':
            s.dir = D::left;
            break;
        case 'd':
            s.dir = D::right;
            break;
        case 'x':
            exit(0);
        default:
            break;
        }
    }
}

//-------------------------------------------------------
void movement() {
    // Update the snake's head position based on the direction
    switch (s.dir) {
    case top:
        s.heady--;
        break;
    case down:
        s.heady++;
        break;
    case D::right:
        s.headx++;
        break;
    case D::left:
        s.headx--;
        break;
    }

    // Add the new head position to the front of the deque
    deq.push_front({ s.headx, s.heady });

    // Remove the last part of the tail if the snake hasn't eaten a fruit
    if (s.headx != m.fruitx || s.heady != m.fruity) {
        deq.pop_back();  // Remove the tail part
    }
}

//-------------------------------------------------------
void end() {
    // Check for boundary collision
    if (s.headx <= 0 || s.headx >= m.width - 1 || s.heady < 0 || s.heady == m.height  ) {
        p.lose = true;  // Snake loses if it hits the wall
    }

    // Check if the snake eats the fruit
    if (s.headx == m.fruitx && s.heady == m.fruity) {
        p.score += 10;

        // Generate a new fruit at a random position
        m.fruitx = rand() % (m.width - 2) + 1;
        m.fruity = rand() % (m.height - 2) + 1;
    }

    // Check for self-collision (snake hits its own body)
    for (size_t i = 1; i < deq.size(); i++) {
        if (s.headx == deq[i].first && s.heady == deq[i].second) {
            p.lose = true;  // Snake loses if it hits its own body
        }
    }
}

//-------------------------------------------------------
void levels() {
    int score = p.score;
    for (int i = 0; i < 20; i++) {
        if (score > (i * 20)) {
            lev -= 5;
        }
    }

    if (lev < 30) {  // Minimum level speed
        lev = 30;
    }
}


//-------------------------------------------------------
int main() {
    setup();
    hideCursor();
    setCursorPosition(0, 0);
    while (!p.lose) {
        game_board();
        input_game();
        movement();
        end();
        levels();
        Sleep(lev);
    }

    showCursor();
    setCursorPosition(0, m.height + 3);
    cout << "\nGame Over! Your score is (>_<): " << p.score << endl;
    return 0;
}

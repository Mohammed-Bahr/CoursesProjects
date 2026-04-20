#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <typeinfo>
#include <Windows.h>

using namespace std;
vector<vector<char>> matrix = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'}
};
//----------------------------------------------------
void draw(const vector<vector<char>>& matrix) {
    system("cls");
    cout << " ------- " << endl;
    for (const auto& row : matrix) {
        cout << "| ";
        for (char cell : row) {
            cout << cell << ' ';
        }
        cout << "|" << endl;
    }
    cout << " ------- " << endl;
}
//---------------------------------------------------------
void replace(vector<vector<char>>& matrix, char player) {
    int pos;
    cout << "Enter your position (" << player << "): ";
    cin >> pos;

    char symbol = (player == 'X') ? 'X' : 'O';
    for (auto& row : matrix) {
        for (char& cell : row) {
            if (cell == '0' + pos) {
                cell = symbol;
                return; // Exit early after replacing
            }
        }
    }
    cout << "Invalid position. Try again." << endl;
}
//---------------------------------------------------------------

char who_wins() {
    int x = 0, o = 0, counter = 0;
    for (int i = 0; i < 3; i++) {
        for (int u = 0; u < 3; u++) {
            if (matrix[i][u] == 'X') {
                x++;
            }
            else if (matrix[i][u] == 'O') {
                o++;
            }
        }
        if (x == 3) {
            return 'X';
        }
        else if (o == 3) {
            return 'O';
        }
        x = 0;
        o = 0;
    }

    for (int i = 0; i < 3; i++) {
        for (int u = 0; u < 3; u++) {
            if (matrix[u][u] == 'X')
                x++;
            else if (matrix[u][u] == 'O')
                o++;
        }
        if (x == 3) {
            return 'X';
        }
        else if (o == 3) {
            return 'O';
        }
        x = 0;
        o = 0;
    }

    if (matrix[0][2] == 'X' && matrix[1][1] == 'X' && matrix[2][0] == 'X') {
        return 'X';
    }
    else if (matrix[0][2] == 'O' && matrix[1][1] == 'O' && matrix[2][0] == 'O') {
        return 'O';
    }
}
//---------------------------------------------------------------
int main() {
    char currentPlayer = 'X';
    while (true) {
        draw(matrix);
        replace(matrix, currentPlayer);
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        if (who_wins() == 'X') {
            cout << "player X wins(>_<)";
            return 0;
        }
        else if (who_wins() == 'O') {
            cout << "player O wins(>_<)";
            return 0;
        }
    }

    return 0;
}
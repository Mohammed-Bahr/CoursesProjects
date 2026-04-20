#ifndef _ULTIMATE_TICTACTOE_H
#define _ULTIMATE_TICTACTOE_H

#include "BoardGame_Classes.h"
#include "x3X_O.h"
#include <iostream>
#include <vector>
using namespace std;

// Small 3x3 board class
class Small_cube : public Board<char> {
private:
    vector<vector<char>> cube;
public:
    Small_cube() {
        cube = vector<vector<char>>(3, vector<char>(3, ' '));
        rows = 3;
        columns = 3;
    }

    bool update_board(int x, int y, char symbol) override {
        if (x >= 0 && x < 3 && y >= 0 && y < 3 && cube[x][y] == ' ') {
            cube[x][y] = symbol;
            n_moves++;
            return true;
        }
        return false;
    }

    void display_board() override {
        cout << " _________\n";
        for (int i = 0; i < 3; ++i) {
            cout << "|";
            for (int j = 0; j < 3; ++j) {
                cout << "" << cube[i][j] << "";
                if (j < 2) cout << "|";
            }
            cout << "|\n";
        }
    }

    bool is_win() override {
        for (int i = 0; i < 3; ++i) {
            if (cube[i][0] != ' ' && cube[i][0] == cube[i][1] && cube[i][1] == cube[i][2]) return true;
            if (cube[0][i] != ' ' && cube[0][i] == cube[1][i] && cube[1][i] == cube[2][i]) return true;
        }
        if (cube[0][0] != ' ' && cube[0][0] == cube[1][1] && cube[1][1] == cube[2][2]) return true;
        if (cube[0][2] != ' ' && cube[0][2] == cube[1][1] && cube[1][1] == cube[2][0]) return true;
        return false;
    }

    bool is_draw() override {
        return n_moves == 9 && !is_win();
    }

    bool game_is_over() override {
        return is_win() || is_draw();
    }

    char who_is_winner() {
        return is_win() ? cube[1][1] : ' ';
    }

    const vector<vector<char>>& get_grid() const {
        return cube;
    }
};

// Ultimate Tic-Tac-Toe class
class Ultimate_Tic_Tac_Toe : public Board<char> {
private:
    vector<vector<Small_cube>> largeGrid;
    //w refer to winner 
    vector<vector<char>> w;
    

public:
    Ultimate_Tic_Tac_Toe() {
        rows = 3;
        columns = 3;
        largeGrid = vector<vector<Small_cube>>(3, vector<Small_cube>(3));
        w = vector<vector<char>>(3, vector<char>(3, ' '));

    }

    bool update_board(int x, int y, char symbol) override {
        int bigX = x / 3, bigY = y / 3;
        int smallX = x % 3, smallY = y % 3;

        if (bigX < 3 && bigY < 3 && w[bigX][bigY] == ' ' &&
            largeGrid[bigX][bigY].update_board(smallX, smallY, symbol)) {
            if (largeGrid[bigX][bigY].is_win()) {
                w[bigX][bigY] = symbol;
                n_moves++;
            }
            return true;
        }
       
        return false;
    }

    bool is_win() override {
        for (int i = 0; i < 3; ++i) {
            if (w[i][0] != ' ' && w[i][0] == w[i][1] && w[i][1] == w[i][2]) return true;
            if (w[0][i] != ' ' && w[0][i] == w[1][i] && w[1][i] == w[2][i]) return true;
        }
        if (w[0][0] != ' ' && w[0][0] == w[1][1] && w[1][1] == w[2][2]) return true;
        if (w[0][2] != ' ' && w[0][2] == w[1][1] && w[1][1] == w[2][0]) return true;
        return false;
    }

    void display_board() override {
        for (int i = 0; i < 3; ++i) {
            for (int smallrow = 0; smallrow < 3; ++smallrow) {
                for (int j = 0; j < 3; ++j) {
                    if (w[i][j] != ' ') {
                        cout << "|" << w[i][j] << "|";
                    }
                    else {
                        for (int smallcol = 0; smallcol < 3; ++smallcol) {
                            cout << "|" << largeGrid[i][j].get_grid()[smallrow][smallcol] << "|";
                        }
                    }
                    if (j < 2) cout << " || ";
                }
                cout << endl;
            }
            if (i < 2) cout << "====================" << endl;
        }
       
    }

    




    bool is_draw() override {
        return n_moves == 9 && !is_win();
    }

    bool game_is_over() override {
        return is_win() || is_draw();
    }
};

#endif // _ULTIMATE_TICTACTOE_H
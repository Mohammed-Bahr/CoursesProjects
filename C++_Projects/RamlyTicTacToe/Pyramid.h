#ifndef _PYRAMID_H
#define _PYRAMID_H

#include "minmaxplayer.h"
#include "BoardGame_Classes.h"
#include "x3X_O.h"
#include <iostream>
using namespace std;

template <typename T>
class Pyramid : public Board<T> {
public:
    Pyramid() {
        this->rows = 3;
        this->columns = 5;
        this->board = new T * [this->rows];
        for (int i = 0; i < this->rows; ++i) {
            this->board[i] = new T[this->columns];
        }
        i_pyramid();
    }

    ~Pyramid() {
        for (int i = 0; i < this->rows; ++i) {
            delete[] this->board[i];
        }
        delete[] this->board;
    }

    bool update_board(int x, int y, T symbol) override {
        if ((x == 0 && y == 0) ||
            (x == 1 && (y == 0 || y == 1 || y == 2)) ||
            (x == 2 && (y >= 0 && y < 5))) {
            if (this->board[x][y] == ' ') {
                this->board[x][y] = symbol;
                this->n_moves++;
                return true;
            }
        }
        return false;
    }

    void display_board() override {
        cout << "             _____ " << endl;
        cout << "            |     |" << endl;
        cout << "            |  " << this->board[0][0] << "  |" << endl;
        cout << "      _____||" << endl;
        cout << "      |     |     |     |" << endl;
        cout << "      |  " << this->board[1][0] << "  |  " << this->board[1][1] << "  |  " << this->board[1][2] << "  |" << endl;
        cout << "||||" << endl;
        cout << "|     |     |     |     |     |" << endl;
        cout << "|  " << this->board[2][0] << "  |  " << this->board[2][1] << "  |  " << this->board[2][2] << "  |  " << this->board[2][3] << "  |  " << this->board[2][4] << "  |" << endl;
        cout << "|||||_|" << endl;
    }

    bool is_win() override {
        if (this->board[1][0] != ' ' && this->board[1][0] == this->board[1][1] && this->board[1][1] == this->board[1][2]) return true;
        if (this->board[2][0] != ' ' && this->board[2][0] == this->board[2][1] && this->board[2][1] == this->board[2][2]) return true;
        if (this->board[2][1] != ' ' && this->board[2][1] == this->board[2][2] && this->board[2][2] == this->board[2][3]) return true;
        if (this->board[2][2] != ' ' && this->board[2][2] == this->board[2][3] && this->board[2][3] == this->board[2][4]) return true;
        if (this->board[0][0] != ' ' && this->board[0][0] == this->board[1][1] && this->board[1][1] == this->board[2][2]) return true;
        if (this->board[0][0] != ' ' && this->board[0][0] == this->board[1][2] && this->board[1][1] == this->board[2][4]) return true;
        if (this->board[0][0] != ' ' && this->board[0][0] == this->board[1][0] && this->board[1][0] == this->board[2][0]) return true;
        return false;
    }

    bool is_draw() override {
        return false;
    }

    bool game_is_over() override {
        return is_win() || is_draw();
    }

private:
    void i_pyramid() {
        for (int i = 0; i < this->rows; i++) {
            for (int j = 0; j < this->columns; j++) {
                if ((i == 0 && j == 0) || (i == 1 && (j == 0 || j == 1 || j == 2)) || (i == 2 && j < 5)) {
                    this->board[i][j] = ' ';
                }
                else {
                    this->board[i][j] = '/';
                }
            }
        }
    }
};


template <typename T>
class minplayer : public X_O_MinMax_Player<T> {
public:
    minplayer(T symbol) : X_O_MinMax_Player<T>(symbol) {}
    void getmove(int& x, int& y) override {
        this->X_O_MinMax_Player<T>::getmove(x, y);
    }
};

#endif //_PYRAMID_H
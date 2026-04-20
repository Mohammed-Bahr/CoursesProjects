//describtion for This game "game 8" reimagines the classic tic-tac-toe concept by introducing 
// a "big cube" that consists of a 3x3 grid of smaller tic-tac-toe games,
// referred to as "small cubes." Each small cube operates as an independent 
// game where players alternate turns, aiming to secure a win by forming a row,
// column, or diagonal of their symbol. The overarching goal is to dominate the
// big cube by aligning victories in three small cubes along a row, column, or
// diagonal of the larger grid. Strategic depth arises from the interplay between
// micro-level decisions within each small cube and macro-level planning to control 
// the big cube. Players must carefully choose which small cube to play in whil
// e predicting their opponent's moves, as each decision can have ripple effects
// across the entire board. With its dynamic and layered gameplay, this
// innovative twist on tic-tac-toe challenges players to think critically and plan 
// several moves ahead, delivering a fresh, competitive, and highly engaging experience.
//---------------------------------------------------------------------------------------------------------------
//this is the int main looks like ->

//#include"Ultimate_Tic_Tac_Toe.h";
//int main() {
//    int choice;
//    Player* players[2];
//    GameManager game;  // Our GameManager class
//    string playerXName, playerOName;
//
//    cout << "Welcome to FCAI Big Cube Game. :)\n";
//
//    // Set up player 1 (X)
//    cout << "Enter Player X name: ";
//    cin >> playerXName;
//    cout << "Choose Player X type:\n";
//    cout << "1. Human\n";
//    cout << "2. Random Computer\n";
//    cin >> choice;
//
//    switch (choice) {
//    case 1:
//        players[0] = new Human(playerXName, 'X');
//        break;
//    case 2:
//        players[0] = new RandomPlayer(playerXName, 'X', 3);
//        break;
//    default:
//        cout << "Invalid choice for Player X. Exiting the game.\n";
//        return 1;
//    }
//
//    // Set up player 2 (O)
//    cout << "Enter Player O name: ";
//    cin >> playerOName;
//    cout << "Choose Player O type:\n";
//    cout << "1. Human\n";
//    cout << "2. Random Computer\n";
//    cin >> choice;
//
//    switch (choice) {
//    case 1:
//        players[1] = new Human(playerOName, 'O');
//        break;
//    case 2:
//        players[1] = new RandomPlayer(playerOName, 'O', 3);
//        break;
//    default:
//        cout << "Invalid choice for Player O. Exiting the game.\n";
//        return 1;
//    }
//
//    // Play the game
//    game.play(players[0], players[1]);
//
//    // Clean up memory
//    delete players[0];
//    delete players[1];
//
//    return 0;
//}


#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// SmallCube class representing a 3x3 board
class SmallCube {
private:
    char board[3][3];
    char winner;
    int n_moves;

public:
    SmallCube() : winner(' '), n_moves(0) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                board[i][j] = ' ';
    }

    bool updateBoard(int row, int col, char symbol) {
        if (row < 0 || row >= 3 || col < 0 || col >= 3 || board[row][col] != ' ') {
            cout << "Invalid move. Try again.\n";
            return false;
        }
        board[row][col] = symbol;
        n_moves++;
        if (isWin()) {
            winner = symbol;
        }
        return true;
    }

    void displayBoard() const {
        cout << "\nSmall Cube Board:\n";
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cout << " " << board[i][j];
                if (j < 2) cout << " |";
            }
            cout << "\n";
            if (i < 2) cout << "---|---|---\n";
        }
    }

    bool isWin() const {
        for (int i = 0; i < 3; i++) {
            if ((board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) ||
                (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])) {
                return true;
            }
        }
        if ((board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
            (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])) {
            return true;
        }
        return false;
    }

    bool isDraw() const {
        return n_moves == 9 && !isWin();
    }

    bool gameIsOver() const {
        return isWin() || isDraw();
    }

    char getWinner() const {
        return winner;
    }
};

// Player abstract class
class Player {
protected:
    string name;
    char symbol;

public:
    Player(const string& n, char s) : name(n), symbol(s) {}

    virtual void getMove(int& x, int& y) const = 0;

    char getSymbol() const {
        return symbol;
    }
    void setSymbol(char s) {
        symbol = s;
    }

    string getName() const {
        return name;
    }
};

// RandomPlayer class derived from Player
class RandomPlayer : public Player {
private:
    int dimension;

public:
    RandomPlayer(const string& n, char s, int dim) : Player(n, s), dimension(dim) {}

    void getMove(int& x, int& y) const override {
        x = rand() % dimension;
        y = rand() % dimension;
    }
};

// Human class derived from Player
class Human : public Player {
public:
    Human(const string& n, char s) : Player(n, s) {}

    void getMove(int& x, int& y) const override {
        cout << "Enter row and column (0-2): ";
        cin >> x >> y;
    }
};

// Board class to manage multiple SmallCube instances
class Board {
private:
    SmallCube grid[3][3];
    bool doneStatus[3][3];

public:
    Board() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                doneStatus[i][j] = false;
    }

    void displayBoard() const {
        cout << "\nBig Cube Board (Small Cubes' status):\n";
        for (int bigRow = 0; bigRow < 3; ++bigRow) {
            for (int smallRow = 0; smallRow < 3; ++smallRow) {
                for (int bigCol = 0; bigCol < 3; ++bigCol) {
                    if (doneStatus[bigRow][bigCol]) {
                        char winner = grid[bigRow][bigCol].getWinner();
                        cout << " " << winner;
                    }
                    else {
                        cout << " -";
                    }
                    if (bigCol < 2) cout << " |";
                }
                cout << endl;
            }
            if (bigRow < 2) cout << "-------------------------------\n";
        }
    }

    SmallCube& getSmallCube(int row, int col) {
        return grid[row][col];
    }

    void markDone(int row, int col) {
        doneStatus[row][col] = true;
    }

    bool isDone(int row, int col) const {
        return doneStatus[row][col];
    }

    bool isAllDone() const {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (!doneStatus[i][j])
                    return false;
        return true;
    }
};

// GameManager class to manage the overall game
class GameManager {
private:
    Board board;
    char winner;

    void checkBigCubeWinner() {
        for (int i = 0; i < 3; i++) {
            if (board.isDone(i, 0) && board.isDone(i, 1) && board.isDone(i, 2)) {
                char smallCubeWinner = board.getSmallCube(i, 0).getWinner();
                if (smallCubeWinner == board.getSmallCube(i, 1).getWinner() && smallCubeWinner == board.getSmallCube(i, 2).getWinner()) {
                    winner = smallCubeWinner;
                    displayBigCube();
                    return;
                }
            }
            if (board.isDone(0, i) && board.isDone(1, i) && board.isDone(2, i)) {
                char smallCubeWinner = board.getSmallCube(0, i).getWinner();
                if (smallCubeWinner == board.getSmallCube(1, i).getWinner() && smallCubeWinner == board.getSmallCube(2, i).getWinner()) {
                    winner = smallCubeWinner;
                    displayBigCube();
                    return;
                }
            }
        }
        if (board.isDone(0, 0) && board.isDone(1, 1) && board.isDone(2, 2)) {
            char smallCubeWinner = board.getSmallCube(0, 0).getWinner();
            if (smallCubeWinner == board.getSmallCube(1, 1).getWinner() && smallCubeWinner == board.getSmallCube(2, 2).getWinner()) {
                winner = smallCubeWinner;
                displayBigCube();
                return;
            }
        }
        if (board.isDone(0, 2) && board.isDone(1, 1) && board.isDone(2, 0)) {
            char smallCubeWinner = board.getSmallCube(0, 2).getWinner();
            if (smallCubeWinner == board.getSmallCube(1, 1).getWinner() && smallCubeWinner == board.getSmallCube(2, 0).getWinner()) {
                winner = smallCubeWinner;
                displayBigCube();
                return;
            }
        }
    }

public:
    GameManager() : winner(' ') {}

    void displayBigCube() const {
        board.displayBoard();
    }

    void play(Player* player1, Player* player2) {
        int currentSmallRow = -1, currentSmallCol = -1;

        Player* currentPlayer = player1;  // Start with Player 1

        while (winner == ' ' && !board.isAllDone()) {
            displayBigCube();  // Display the board at the start of the turn

            cout << currentPlayer->getName() << "'s turn.\n";

            if (currentSmallRow == -1 || board.isDone(currentSmallRow, currentSmallCol)) {
                do {
                    cout << "Choose a small board (row and column of the 3x3 grid): ";
                    cin >> currentSmallRow >> currentSmallCol;
                } while (board.isDone(currentSmallRow, currentSmallCol));
            }

            cout << "Playing on small board (" << currentSmallRow << ", " << currentSmallCol << ").\n";

            int row, col;
            currentPlayer->getMove(row, col);  // Get move from the current player

            // Update the small cube with the current player's move
            if (!board.getSmallCube(currentSmallRow, currentSmallCol).updateBoard(row, col, currentPlayer->getSymbol())) {
                continue;  // Invalid move, retry
            }

            // Show the small cube's board after the move
            board.getSmallCube(currentSmallRow, currentSmallCol).displayBoard();

            // Check if the small cube is done and update the big cube
            if (board.getSmallCube(currentSmallRow, currentSmallCol).gameIsOver()) {
                board.markDone(currentSmallRow, currentSmallCol);
                checkBigCubeWinner();
            }

            // Alternate players after each valid move
            if (currentPlayer == player1) {
                currentPlayer = player2;
            }
            else {
                currentPlayer = player1;
            }
        }

        // Announce winner or draw
        if (winner != ' ') {
            cout << "Congratulations! " << (winner == player1->getSymbol() ? player1->getName() : player2->getName()) << " wins!\n";
        }
        else {
            cout << "It's a draw!\n";
        }
    }

};

int main() {
    int choice;
    Player* players[2];
    GameManager game;  // Our GameManager class
    string playerXName, playerOName;

    cout << "Welcome to FCAI Big Cube Game. :)\n";

    // Set up player 1 (X)
    cout << "Enter Player X name: ";
    cin >> playerXName;
    cout << "Choose Player X type:\n";
    cout << "1. Human\n";
    cout << "2. Random Computer\n";
    cin >> choice;

    switch (choice) {
    case 1:
        players[0] = new Human(playerXName, 'X');
        break;
    case 2:
        players[0] = new RandomPlayer(playerXName, 'X', 3);
        break;
    default:
        cout << "Invalid choice for Player X. Exiting the game.\n";
        return 1;
    }

    // Set up player 2 (O)
    cout << "Enter Player O name: ";
    cin >> playerOName;
    cout << "Choose Player O type:\n";
    cout << "1. Human\n";
    cout << "2. Random Computer\n";
    cin >> choice;

    switch (choice) {
    case 1:
        players[1] = new Human(playerOName, 'O');
        break;
    case 2:
        players[1] = new RandomPlayer(playerOName, 'O', 3);
        break;
    default:
        cout << "Invalid choice for Player O. Exiting the game.\n";
        return 1;
    }

    // Play the game
    game.play(players[0], players[1]);

    // Clean up memory
    delete players[0];
    delete players[1];

    return 0;
}


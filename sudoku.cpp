#include<algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>


const int BOARD_SIZE = 9;
const int SUBGRID_SIZE = 3;
const int EMPTY_CELL = 0;
const int MAX_WRONG_TURNS = 3;

class SudokuGame {
private:
    std::vector<std::vector<int>> board;
     int wrongTurns=0;
     std::chrono::steady_clock::time_point startTime;

public:
    SudokuGame() {
        // Initialize the board with empty cells
        board.resize(BOARD_SIZE, std::vector<int>(BOARD_SIZE, EMPTY_CELL));
    }

    void generateBoard() {
        std::random_device rd;
        std::mt19937 gen(rd());

        // Fill the main diagonal subgrids with random valid numbers
        for (int i = 0; i < BOARD_SIZE; i += SUBGRID_SIZE) {
            fillSubgrid(i, i, gen);
        }

        // Solve the complete puzzle starting from the top-left cell
        solve(0, 0, gen);

        // Remove some cells to create a puzzle
        removeCells(40, gen);
    }

    bool isBoardFilled() const {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == EMPTY_CELL) {
                    return false;
                }
            }
        }
        return true;
    }
    void fillSubgrid(int row, int col, std::mt19937& gen) {
        std::vector<int> validNums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::shuffle(validNums.begin(), validNums.end(), gen);

        int numIndex = 0;
        for (int i = row; i < row + SUBGRID_SIZE; i++) {
            for (int j = col; j < col + SUBGRID_SIZE; j++) {
                board[i][j] = validNums[numIndex];
                numIndex++;
            }
        }
    }

    bool solve(int row, int col, std::mt19937& gen) {
        // Check if we have reached the end of the board
        if (row == BOARD_SIZE) {
            row = 0;
            if (++col == BOARD_SIZE) {
                return true;
            }
        }

        // Skip cells that are already filled
        if (board[row][col] != EMPTY_CELL) {
            return solve(row + 1, col, gen);
        }

        // Generate random numbers to fill the current cell
        std::vector<int> validNums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::shuffle(validNums.begin(), validNums.end(), gen);

        for (int num : validNums) {
            if (isValidMove(row, col, num)) {
                board[row][col] = num;
                if (solve(row + 1, col, gen)) {
                    return true;
                }
                board[row][col] = EMPTY_CELL; // Backtrack
            }
        }

        return false;
    }

    bool isValidMove(int row, int col, int num) {
        // Check if the number exists in the row or column
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[row][i] == num || board[i][col] == num) {
                return false;
            }
        }

        // Check if the number exists in the subgrid
        int subgridRow = row - row % SUBGRID_SIZE;
        int subgridCol = col - col % SUBGRID_SIZE;
        for (int i = 0; i < SUBGRID_SIZE; i++) {
            for (int j = 0; j < SUBGRID_SIZE; j++) {
                if (board[subgridRow + i][subgridCol + j] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    void removeCells(int numToRemove, std::mt19937& gen) {
        std::uniform_int_distribution<> dis(0, BOARD_SIZE - 1);
        int cellsRemoved = 0;

        while (cellsRemoved < numToRemove) {
            int row = dis(gen);
            int col = dis(gen);

            if (board[row][col] != EMPTY_CELL) {
                board[row][col] = EMPTY_CELL;
                cellsRemoved++;
            }
        }
    }

    void displayBoard() const {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (i % SUBGRID_SIZE == 0 && i != 0) {
                std::cout << "---------------------" << std::endl;
            }
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (j % SUBGRID_SIZE == 0 && j != 0) {
                    std::cout << "| ";
                }
                if (board[i][j] == EMPTY_CELL) {
                    std::cout << "  ";
                } else {
                    std::cout << board[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    void playGame() {
        int row, col, num;
          startTime = std::chrono::steady_clock::now();

        while (true) {
            std::cout << "Enter the row (1-9), column (1-9), and number (1-9) separated by spaces (or enter 0 0 0 to quit): ";
            std::cin >> row >> col >> num;

            if (row == 0 && col == 0 && num == 0) {
                std::cout << "Thanks for playing! Goodbye!" << std::endl;
                break;
            }

            if (row < 1 || row > BOARD_SIZE || col < 1 || col > BOARD_SIZE || num < 1 || num > BOARD_SIZE) {
                std::cout << "Invalid input. Please try again." << std::endl;
                continue;
            }

            if (board[row - 1][col - 1] != EMPTY_CELL) {
                std::cout << "Cell already filled. Please select an empty cell." << std::endl;
                continue;
            }

           if (!isValidMove(row - 1, col - 1, num)) {
                std::cout << "Invalid move. Please try again." << std::endl;
                wrongTurns++;
                if (wrongTurns > MAX_WRONG_TURNS) {
                    std::cout << "You reached the maximum wrong turns. Game over!" << std::endl;
                    break;
                }
                continue;
            }

            board[row - 1][col - 1] = num;
            displayBoard();
            
            if (isBoardFilled()) {
                std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsedTime = endTime - startTime;
                std::cout << "Congratulations! You solved the puzzle in " << elapsedTime.count() << " seconds." << std::endl;
                break;
            }
        }
    }
};

int main() {
    SudokuGame game;
    game.generateBoard();
    game.displayBoard();
    game.playGame();

    return 0;
}

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <queue>

using namespace std;

enum difficulty{
    easy = 10,
    medium = 15,
    hard = 20
};

// Board cell. Each cell has a value which determines whether it contains a
// bomb (-1) or how many bombs it neighbours. Each cell also knows it's 
// location on the games board.
class cell
{
    private:
        int row, col;
        int nearby_bombs;
        bool revealed;

    public:
        void set_nearby_bombs(int bombs) {
            nearby_bombs = bombs;
        }
        void reveal(void) {
            revealed = true;
        }
        int get_nearby_bombs(void) {
            return nearby_bombs;
        }
        bool get_revealed(void) {
            return revealed;
        }
        void inc_nearby_bombs(void) {
            nearby_bombs++;
        }

        int get_row(void) {
            return row;
        }
        int get_col(void) {
            return col;
        }
        cell(int r, int c): row(r), col(c) {
            revealed = false; nearby_bombs = 0;
        }
        cell() {}
        ~cell() {}
};

// The games board controls the game. It has the ability to set up the board by
// first assigning the size of the board according to the difficulty of the game
// and by placing the bombs on the board at random. It also runs the game and displays
// the board.
class game_board 
{
    private:
        int row, col, length, revealed_cells, number_of_bombs, number_of_cells;
        const bool debug;
        std::vector<std::vector<cell>> board;
    
    public:

        void user_choose_cell(void) {
            // Ask user to select a cell.
            cout<<"Please choose a cell using row col:"<<endl;
            cin>>row>>col;

            // Check cell is within board
            if (row < 0 || row >= length || col < 0 || col >= length) {
                cout<<"Chosen cell does not exist!"<<endl;
                user_choose_cell();
            }
        }

        void check_cell(void) {
            // Check cell
            switch(board[row][col].get_nearby_bombs())
            {
                // If cell has a bomb, game over.
                case -1:
                    game_over();
                    break;

                // If cell is not near any bombs, find and reveal
                // all nearby non-bomb cells.
                case 0:
                    find_nearby_zero_cells();
                    break;
                
                // If cell has a number, reveal number.
                default:
                    board[row][col].reveal();
                    revealed_cells++;
            }
            // Display the board.
            display_board();
        }

        void display_board(void) {
            // Outputs formatted board to the terminal.
            cout<<"    |";
            for (int j=0; j< length; j++) cout<<" -- |";
            cout<<endl;
            
            for(int i=0; i<length; i++) {
                (i > 9) ? cout<<" "<<i<<" |" : cout<<"  "<<i<<" |";
                for(int j=0; j<length; j++) {
                    if (board[i][j].get_revealed()) {
                        (board[i][j].get_nearby_bombs()>9) ? cout<<" "<<board[i][j].get_nearby_bombs()<<" |" : cout<<"  "<<board[i][j].get_nearby_bombs()<<" |";
                    }
                    else cout<<"  ? |";
                }
                cout<<endl;
                cout<<"    |";
                for (int j=0; j< length; j++) cout<<" -- |";
                cout<<endl;
            }
            cout<<endl; cout<<"    |";
            for (int j=0; j<length; j++) (j > 9) ? cout<<" "<<j<<" |" : cout<<"  "<<j<<" |";
            cout<<endl;
        }

        void display_bombs(void) {
            // Outputs full games board.
                        cout<<"    |";
            for (int j=0; j< length; j++) cout<<" -- |";
            cout<<endl;
            
            for(int i=0; i<length; i++) {
                (i > 9) ? cout<<" "<<i<<" |" : cout<<"  "<<i<<" |";
                for(int j=0; j<length; j++) {
                    if (board[i][j].get_nearby_bombs()==-1) cout<<"  B |";
                    else (board[i][j].get_nearby_bombs()>9) ? cout<<" "<<board[i][j].get_nearby_bombs()<<" |" : cout<<"  "<<board[i][j].get_nearby_bombs()<<" |";
                }
                cout<<endl;
                cout<<"    |";
                for (int j=0; j< length; j++) cout<<" -- |";
                cout<<endl;
            }
            cout<<endl; cout<<"    |";
            for (int j=0; j<length; j++) (j > 9) ? cout<<" "<<j<<" |" : cout<<"  "<<j<<" |";
            cout<<endl;

        }

        void find_nearby_zero_cells(void) {
            // Checks nearby cells to see which have 0s and which have numbers.
            // Reveals connecting 0 and number cells.
            queue<cell> cells_to_check;
            cell current_cell;
            cells_to_check.push(board[row][col]);

            while(!cells_to_check.empty()) {
                // Check cell from front of queue.
                current_cell = cells_to_check.front();
                cells_to_check.pop();

                // Loop over neightbouring cells.
                for(int i=current_cell.get_row()-1; i<current_cell.get_row() + 2; i++){
                    for(int j=current_cell.get_col()-1; j < current_cell.get_col() + 2; j++) {

                        // Check the neighbouring cell is on the board.
                        if(i >= 0 && i < length && j >= 0 && j < length) {

                            // Check that the cell is not a bomb and not revealed
                            if (!board[i][j].get_revealed() && board[i][j].get_nearby_bombs() !=-1){

                                // Reveal cell
                                board[i][j].reveal();
                                revealed_cells++;
                                
                                // If the cell is 0, add to the queue of cells to check.
                                if (board[i][j].get_nearby_bombs()==0) cells_to_check.push(board[i][j]);

                            }
                        }
                    }
                }
            }

        }

        void run_game(void) {
            // Runs game.

            // Game ends when only bomb cells not revealed.
            while (number_of_cells - revealed_cells > number_of_bombs) {
                // User selects a cell.
                user_choose_cell();

                // Determine course of action depending on value of cell.
                check_cell();
            }
        }

        void game_over() {
            // Alert the user to game over and display revealed board.
            cout<<"You hit a bomb! Game Over!"<<endl;
            display_bombs();
            exit(EXIT_FAILURE);
        }

        game_board(bool db, difficulty dc): debug(db), length(dc)
        {    
            // Set board length and number of bombs
            switch(dc) {
                case easy:
                    number_of_bombs = 15;
                    break;
                case medium:
                    number_of_bombs = 50;
                    break;
                case hard:
                    number_of_bombs = 125;
                    break;
                default:
                    cout<<"Unknown difficulty."<<endl;
                    exit(EXIT_FAILURE);
            }

            // Set required variables
            revealed_cells = 0; number_of_cells = length*length; number_of_bombs = 15;

            // Set up random number generator
            unsigned seed;

            if (debug) seed = 14;
            else seed = chrono::system_clock::now().time_since_epoch().count();
            std::mt19937 rng{seed};
            std::uniform_int_distribution dist(0, length-1);

            // Set up board cells
            std::vector<cell> board_row;
            for(int r=0; r<length; r++) {
                for(int c=0; c<length; c++) {
                    board_row.push_back(cell(r, c));
                }
                board.push_back(board_row);
                board_row.clear();
            }

            // Add bombs
            int bombs = 0;

            while (bombs < number_of_bombs) {
                // Choose cell at random
                row = dist(rng); col = dist(rng);

                // If not already a bomb, place bomb
                if (board[row][col].get_nearby_bombs() != -1) {
                
                    board[row][col].set_nearby_bombs(-1);

                    // Add one to all nearby cells
                    for (int i = row - 1; i <= row + 1; i++) {
                        for (int j = col -1; j <= col + 1; j++) {
                            if (i >= 0 && i < length && j >= 0 && j < length) {
                                if (board[i][j].get_nearby_bombs() != -1) board[i][j].inc_nearby_bombs();
                            }
                        }
                    }
                    bombs++;
                }
            }
            display_board();
        }

};

int main() {

    int diff; difficulty mode;

    // Get user to enter difficulty.
    cout<<"Please enter a difficult: easy (1), medium (2) or hard (3)"<<endl;
    cin>>diff;

    // Determine board size from difficulty.
    switch(diff) 
    {
        case 1:
            mode = easy;
            break;
        case 2:
            mode = medium;
            break;
        case 3:
            mode = hard;
            break;
        default:
            cout<<"Entered difficult not recognised. Difficulty will be set to easy."<<endl;
            mode = easy;
    }

    // Construct game board
    game_board minesweeper(0, mode);

    // Run game
    minesweeper.run_game();

    // User wins - display board.
    cout<<"Congratulations! You won!"<<endl;
    minesweeper.display_bombs();
    
}
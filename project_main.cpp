#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <unistd.h>

using namespace std;

// train and play 4x4 tic-tac-toe
class ttt {
    public:

    // init
    ttt(int e = 10) {
        players.push_back('x');
        players.push_back('o');
        epsilon = e;
        eval = new unordered_map<string, pair<int, int>>;
    }

    ~ttt() {
        delete eval;
    }

    // play a game against itself using a policy based on score of positions on eval
    // once it's complete, update eval based on final result
    // set display true to show the game on terminal
    char simulate_game(bool display) {
        vector<pair<char, string>> game;          // holds game move history
        string current_game = "................"; // initial position 
        char winner = 'c';                        // continue game
        int turn = 0;

        while (winner == 'c') {
            char to_move = players[turn%2];
            current_game = best_move(current_game, to_move);
            ++turn;
            game.push_back(make_pair(to_move, current_game));
            winner = find_winner(current_game);

            if (display) {
                display_position(current_game);
            } 
        }

        // update position scores based on winner 
        update_eval(game, winner);
        
        return winner;
    }

    // return best move given a position and player to play
    string best_move(string position, char to_play, bool explore = true) {
        vector<string> lm; 
        legal_moves(lm, position, to_play);

        string bm;
        double max_score = -1;

        // selects a random move epsilon % of the time 
        if (get_random(100) < epsilon && explore) {
            string random_move = lm[get_random(lm.size())];

            if (eval->find(random_move) == eval->end()) {
                eval->insert(make_pair(random_move, make_pair(1, 1)));
            }

            return random_move;
        }

        // select best move 100 - epsilon % of the time 
        for (auto &p : lm) {
            double temp_score;

            // find score of position
            auto it = eval->find(p);

            if (it == eval->end()) {
                // if not in the map, initialize with score 1 (encourages 
                // selection of new moves)
                eval->insert(make_pair(p, make_pair(1, 1)));
                temp_score = 1;
            } else {
                temp_score = (double)it->second.first / (double)it->second.second;
            }

            if (temp_score > max_score) {
                // update to a better move
                bm = p;
                max_score = temp_score;
            } else if (temp_score == max_score) {
                // break ties randomly
                if (get_random(100) < 50) {
                    bm = p;
                }
            }
        }

        return bm;
    }

    // returns the winner of the position ('x' or 'o'), '.' for a draw or 'c' if 
    // the game continues
    char find_winner(string p) {
        // check rows 
        if (p[0] == p[1] && p[1] == p[2] && p[2] == p[3] && p[0] != '.') {
            return p[0];
        }
        if (p[4] == p[5] && p[5] == p[6] && p[6] == p[7] && p[4] != '.') {
            return p[4];
        }
        if (p[8] == p[9] && p[9] == p[10] && p[10] == p[11] && p[8] != '.') {
            return p[8];
        }
        if (p[12] == p[13] && p[13] == p[14] && p[14] == p[15] && p[12] != '.') {
            return p[12];
        }

        // check columns
        if (p[0] == p[4] && p[4] == p[8] && p[8] == p[12] && p[0] != '.') {
            return p[0];
        }
        if (p[1] == p[5] && p[5] == p[9] && p[9] == p[13] && p[1] != '.') {
            return p[4];
        }
        if (p[2] == p[6] && p[6] == p[10] && p[10] == p[14] && p[2] != '.') {
            return p[2];
        }
        if (p[3] == p[7] && p[7] == p[11] && p[11] == p[15] && p[3] != '.') {
            return p[3];
        }

        // check diagonals 
        if (p[0] == p[5] && p[5] == p[10] && p[10] == p[15] && p[0] != '.') {
            return p[0];
        }
        if (p[3] == p[6] && p[6] == p[9] && p[9] == p[12] && p[3] != '.') {
            return p[3];
        }

        // check if game can continue i.e. at least one empty space 
        for (auto &c : p) {
            if (c == '.') {
                return 'c';
            }
        }

        // game is a draw 
        return '.';
    } 

    // display position on terminal 
    void display_position(string position) {
        cout << "   " << position.substr(0, 4)  << endl;
        cout << "   " << position.substr(4, 4)  << endl;
        cout << "   " << position.substr(8, 4)  << endl;
        cout << "   " << position.substr(12, 4) << endl;
        cout << endl;

        sleep(1);
    }

    private:

    // return a random integer between 0 and n-1 
    int get_random(int n) {
        srand((unsigned)time(NULL)); // random seed 
        return rand() % n;
    }

    // set vector lm with all legal moves in a given position
    void legal_moves(vector<string> & lm, string position, char to_play) {
        for (int i = 0; i < position.length(); ++i) {
            if (position[i] == '.') {
                // if empty spot 
                string temp = position;
                temp[i] = to_play;
                lm.push_back(temp);
            }
        }
    }

    // after the game finishes, given list of moves and winner, update the eval map 
    void update_eval(vector<pair<char, string>> & game, char winner) {
        // follow modified version of  algorithm "on-policy first-visit 
        // Monte Carlo Control" from book Reinforcement Learning - Sutton and Barto 
        // Second Ed. page 101

        // simply updates # of wins and # of times played on each position 
        // played 
        for (auto &g : game) {
            auto it = eval->find(g.second); // assumes the position is in eval

            // losing or drawing results in 0
            if (g.first == winner) {
                it->second.first += 1;
            }

            it->second.second += 1;
        }

    }

    // data

    // state-action evaluation: stores the resulting position after playing a 
    // move (action) as well as number of times played and game result 
    // positions are stored as 16 char strings representing the 16 positions in 
    // a 4x4 tic-tac-toe. each char is {x, o, .}
    
    // maps state to [# of wins, # of times played]
    // e.g. eval[..x.............] = (1, 2) if such position has been played 2
    // time and won one game
    unordered_map<string, pair<int, int>> * eval;

    vector<char> players; // players 'x' or 'o'

    int epsilon;       // how ofter it explores e.g. 10 means it selects a 
                       // random move 10% of the time and the known best move 
                       // 90% of the time 

};


void print_menu() {
    cout << "   menu:" << endl;
    cout << "       input [char] and press enter"            << endl;
    cout << "       [w] watch a game (computer vs computer)" << endl;
    cout << "       [p] play a game against the computer"    << endl;
    cout << "       [q] quit"                                << endl;
}

void print_instructions() {
    cout << "   to play a move input row col e.g. 0 0 to play on top left corner" << endl;
    cout << endl;
}

// given a move and position return true if legal
bool is_legal_move(int row, int col, string p) {
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        return false;
    }
    // checks if place is empty
    if (p[row*4 + col] != '.') {
        return false;
    }

    return true;
}

int main() {
    int simulations;
    ttt * program = new ttt();
    
    cout << "   ||tic-tac-toe 4x4||"                 << endl;
    cout << endl;
    cout << "   number of simulations? (1000000 recomended) "; 
    cin >> simulations;
    cout << endl;

    for (int i = 0; i < simulations; ++i) {
        cout << "\r" << i+1 << '/' << simulations << flush;
        program->simulate_game(false); 
    }

    cout << endl;
    cout << endl;
    
    while (true) {
        print_menu();
        char game_result;
        string c;
        cin >> c;

        if (c == "w") {
            // watch a game 
            game_result = program->simulate_game(true);

            if (game_result == 'x') { cout << " x wins" << endl; }
            if (game_result == 'o') { cout << " o wins" << endl; }
            if (game_result == '.') { cout << " draw" << endl; }
            cout << endl;
        }

        if (c == "p") {
            print_instructions();
            string current_position = "................";
            bool turn = true; // player's turn?

            game_result = 'c';

            while (game_result == 'c') {
                program->display_position(current_position);

                if (turn) {
                    // player's turn 
                    int row, col;
                    cout << "   your move: ";
                    cin >> row >> col;

                    if (!is_legal_move(row, col, current_position)) {
                        cout << "   illegal move" << endl;
                        cout << endl;
                    } else {
                        current_position[row*4 + col] = 'x';
                        turn = !turn;
                    }
                } else {
                    // pc turn 
                    cout << "   computer move: " << endl;;
                    current_position = 
                               program->best_move(current_position, 'o', false);
                    turn = !turn;
                }
                game_result = program->find_winner(current_position);
            }

            program->display_position(current_position);

            if (game_result == 'x') { cout << " x wins" << endl; }
            if (game_result == 'o') { cout << " o wins" << endl; }
            if (game_result == '.') { cout << " draw" << endl; }
            cout << endl;
        }

        if (c == "q") {
            delete program;
            return 0;
        }
    }
}
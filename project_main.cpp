#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>

using namespace std;

class player {
    public:
    // methods 

    player() {
        players.push_back('x');
        players.push_back('o');
        epsilon = 20;
        eval = new unordered_map<string, pair<int, int>>;
    }

    ~player() {
        delete eval;
    }

    // returns the winner of the position ('x' or 'o'), '.' for a draw or 'c' if 
    // the game continues given a position
    char find_winner(string p) {
        // check rows 
        if (p[0] == p[1] && p[1] == p[2] && p[0] != '.') {
            return p[0];
        }
        if (p[3] == p[4] && p[4] == p[5] && p[3] != '.') {
            return p[3];
        }
        if (p[6] == p[7] && p[7] == p[8] && p[6] != '.') {
            return p[6];
        }

        // check columns
        if (p[0] == p[3] && p[3] == p[6] && p[0] != '.') {
            return p[0];
        }
        if (p[1] == p[4] && p[4] == p[7] && p[1] != '.') {
            return p[1];
        }
        if (p[2] == p[5] && p[5] == p[8] && p[2] != '.') {
            return p[2];
        }

        // check diagonals 
        if (p[0] == p[4] && p[4] == p[8] && p[0] != '.') {
            return p[0];
        }
        if (p[2] == p[4] && p[4] == p[6] && p[2] != '.') {
            return p[2];
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

    // returns best move given a position and player to play
    string best_move(string position, char to_play) {
        vector<string> lm; 
        legal_moves(lm, position, to_play);

        string bm;
        double max_score = -1;

        srand((unsigned)time(NULL)); // random seed 
        int rnd = rand() % 100;      // find random move between 0 and 99

        // selects a random move epsilon % of the time 
        if (rnd < epsilon) {
            string random_move = lm[rand()%lm.size()];

            if (eval->find(random_move) == eval->end()) {
                eval->insert(make_pair(random_move, make_pair(1, 1)));
            }

            return random_move;
        }

        // selects best move 100 - epsilon % of the time 
        for (auto &p : lm) {
            double temp_score;

            // find score of position
            auto it = eval->find(p);

            if (it == eval->end()) {
                // if not in the map, initialize to 1. that will encourage 
                // selection of unseen moves
                eval->insert(make_pair(p, make_pair(1, 1)));
                temp_score = 1;
            } else {
                temp_score = (double)it->second.first / (double)it->second.second;
                // cout << "temp: " << temp_score << endl;
            }

            if (temp_score > max_score) {
                // update to a better move
                bm = p;
                max_score = temp_score;
                // cout << "max: " << max_score << endl;
            }
        }

        // cout << position << " " << bm << endl;
        // cout << endl;

        return bm;
    }

    // after the game finishes, given list of moves and winner, update the eval map 
    void update_eval(vector<pair<char, string>> & game, char winner) {
        // follows algorithm "on-policy first-visit Monte Carlo Control" from 
        // book Reinforcement Learning - Sutton and Barto Second Ed. page 101

        // simply updates # of wins and # of times played on each position 
        // played 
        for (auto &g : game) {
            auto it = eval->find(g.second); // assumes the position is in eval
            // if (it == eval->end()) {
            //     cout << "hmmmmm" << endl;
            // } else {
            //     cout << "goood" << endl;
            // }
            // cout << "old: " << it->second.first << " " << it->second.second << endl;

            // losing or drawing results in 0
            if (g.first == winner) {
                it->second.first += 1;
            }

            it->second.second += 1;

            // it = eval->find(g.second); // assumes the position is in eval
            // cout << "new: " << it->second.first << " " << it->second.second << endl;
        }

    }

    // display position on terminal 
    void display_position(string position) {
        cout << position.substr(0, 3) << endl;
        cout << position.substr(3, 3) << endl;
        cout << position.substr(6, 3) << endl;
        cout << endl;
    }

    // play a game against itself using a policy based on score of positions on eval
    // once it's complete, update eval based on final result
    // set display true to show the game on terminal
    char simulate_game(bool display) {
        vector<pair<char, string>> game;   // holds game move history
        string current_game = "........."; // initial position 
        char winner = 'c';                 // continue game
        int turn = 0;

        while (winner == 'c') {
            char to_move = players[turn%2];
            // cout << current_game << " ";
            current_game = best_move(current_game, to_move);
            // cout << current_game << endl;
            ++turn;
            game.push_back(make_pair(to_move, current_game));
            winner = find_winner(current_game);

            if (display) {
                display_position(current_game);
            } 
        }
        // cout << "hereee" << endl;
        // cout << eval->size() << endl;

        // update position scores based on winner 
        // cout << winner << endl;
        update_eval(game, winner);
        // cout << "hereee" << endl;
        return winner;
    }

    // data

    // state-action evaluation: stores the resulting position after playing a 
    // move (action) as well as number of times playerd and game result 
    // positions are stored as 9 char strings representing the 9 positions in 
    // a 3x3 tic-tac-toe. each char is {x, o, .}
    
    // maps state to [# of wins, # of times played]
    // e.g. eval[..x......] = (1, 2)
    unordered_map<string, pair<int, int>> * eval;

    vector<char> players; // players 'x' or 'o'

    int epsilon;       // how ofter it explores e.g. 10 means it selects a 
                       // random move 10% of the time and the known best move 
                       // 90% of the time 

};

int main() {
    int simulations = 1000000;
    player * test_player = new player();
    int x = 0;
    int o = 0;
    int t = 0;

    for (int i = 0; i < simulations; ++i) {
        char temp = test_player->simulate_game(false);  
        if (temp == 'x') { ++x; }
        else if (temp == 'o') { ++o; }
        else if (temp == '.') { ++t; }
        // cout << "---------------------" << endl;  
    }
    // cout << "hererr" << endl;
    test_player->simulate_game(true);
    // cout << "hererrrr" << endl;
    delete test_player;
    // cout << "hererrrrrrrrrrrrrrrr" << endl;
    cout << "x: " << x << " o: " << o << " t: " << t << endl;
}
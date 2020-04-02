#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class player {
    public:
    // methods 

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

        for (auto &p : lm) {
            double temp_score;

            // find score of position
            auto it = eval.find(p);

            if (it == eval.end()) {
                // if not in the map, initialize to 1/2. that will encourage 
                // selection of unseen moves
                eval.insert(make_pair(p, make_pair(1, 2)));
                temp_score = 1/2;
            } else {
                temp_score = it->second.first / it->second.second;
            }

            if (temp_score > max_score) {
                // update to a better move
                bm = p;
                max_score = temp_score;
            }
        }

        return bm;
    }

    // after the game finishes, given list of moves and winner, update the eval map 
    void update_eval(vector<pair<char, string>> & game, char winner) {
        // follows algorithm "on-policy first-visit Monte Carlo Control" from 
        // book Reinforcement Learning - Sutton and Barto Second Ed. page 101

        // simply updates # of wins and # of times played on each position 
        // played 
        for (auto &g : game) {
            auto it = eval.find(g.second); // assumes the position is in eval

            // losing or drawing results in 0
            if (g.first == winner) {
                it->second.first += 1;
            }

            it->second.second += 1;
        }

    }

    // data

    // state-action evaluation: stores the resulting position after playing a 
    // move (action) as well as number of times playerd and game result 
    // positions are stored as 9 char strings representing the 9 positions in 
    // a 3x3 tic-tac-toe. each char is {x, o, .}
    
    // maps state to [# of wins, # of times played]
    // e.g. eval[..x......] = (1, 2)
    unordered_map<string, pair<int, int>> eval;


};

int main() {

}
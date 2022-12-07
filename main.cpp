// This Program is a console Snake-Game, use WSAD keys to move and collect Dollars!
// Author: Youssef Mohammed Morad -> www.github.com/YoussefMorad1
// Version 1.0 -> Date: 12/6/2022 -> coded a very simple version of the game
    // Version 1.1 -> Date: 12/7/2022 -> added more futures to the game and fixed some bugs
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

struct point {
    // r -> row , c -> column
    int r, c;
};

class Dollar {
private:
    point pos;
public:
    Dollar() {}

    // new update position function (currently used)
    void update_position(vector<point> validPoints);

    point get_pos() {
        return pos;
    }
};

void Dollar::update_position(vector<point> validPoints) {
    if (validPoints.empty()) {
        pos.r = -1, pos.c = -1;
        return;
    }
    // Choose a random point to put the dollar in
    srand(time(nullptr));
    int idx = rand() % validPoints.size();
    pos.r = validPoints[idx].r;
    pos.c = validPoints[idx].c;
}

class Snake {
private:
    point head_pos;
    char direction = 'u';
    vector<point> tails;
public:
    Snake() {}

    void move();

    point get_head_pos() {
        return head_pos;
    }

    char get_direction() {
        return direction;
    }

    vector<point> &get_tails() {
        return tails;
    }

    void set_direction(char x) {
        direction = x;
    }

    void set_position(point x) {
        head_pos = x;
    }

};

void Snake::move() {
    // update the position of every tail_point
    for (int i = tails.size() - 1; i >= 1; --i) {
        tails[i] = tails[i - 1];
    }
    if (!tails.empty())
        tails[0] = head_pos;

    // change Snake head_position depending on its direction
    if (direction == 'u') {
        head_pos.r--;
    } else if (direction == 'd') {
        head_pos.r++;
    } else if (direction == 'l') {
        head_pos.c--;
    } else {
        head_pos.c++;
    }
}

class Board {
private:
    int rows, cols, score;
    char **ptr; // pointer to our 2D-Board
    Snake snake;
    Dollar dollar;

    bool is_lost();

    char take_input(char x = 0);

    void reset_board();

    void print();

    void update_board_elements();

    vector<point> get_empty_points();

    void restart_game();

public:
    Board(int w = 20, int h = 20);

    void play();

    ~Board();
};

Board::Board(int w, int h)
        : rows(w), cols(h), score(0) {
    // allocate a 2D array to keep current board state in it
    ptr = new char *[rows];
    for (int i = 0; i < rows; ++i) {
        ptr[i] = new char[cols];
    }
    // reset the board
    reset_board();
    // set initial snake and dollar position
    snake.set_position({rows / 2, cols / 2});
    dollar.update_position(get_empty_points());
}

void Board::play() {
    bool first = true;
    while (!is_lost()) {
        print();
        if (first) {
            // if it's the start of the game hold the snake and wait for a key to start
            cout << "Press any key to start..";
            char x = take_input((char) getch());
            // set snake direction depending on the pressed key or keep to looking upward (if pressed key isn't a WSAD)
            if (x != '0')
                snake.set_direction(x);
            // it's not the start of the game anymore -> first = false
            first = false;
        } else {
            // get input to see if the player changed direction
            char ch = take_input();
            // if a WASD key was pressed set the snake direction
            if (ch != '0')
                snake.set_direction(ch);
        }
        // move the snake forward in his direction
        snake.move();
        // update the board (if the dollar was eaten)
        update_board_elements();
        // clean screen and repeat
        _sleep(50);
        system("cls");
    }
    // print the final board after loss and final score
    print();
    cout << "\n\nGame Over!\nSnake's Wealth: " << score << '$' << endl;

    // check if player wants to play again
    cout << "\nDo you want to play again?! press (y or n)\n";
    char x = tolower(getch());
    while (x != 'y' && x != 'n')
        x = tolower(getch());
    // if yes restart game else return
    if (x == 'y') {
        restart_game();
    } else
        return;
}

void Board::print() {
    // clean board
    reset_board();

    // draw the snake's head and tails inside my board
    ptr[snake.get_head_pos().r][snake.get_head_pos().c] = 'O';
    vector<point> &snake_tails = snake.get_tails();
    for (point &pt: snake_tails) {
        ptr[pt.r][pt.c] = 'o';
    }

    // draw the dollar
    if (dollar.get_pos().r != -1)
        ptr[dollar.get_pos().r][dollar.get_pos().c] = '$';

    // print my board with current score
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << ptr[i][j];
        }
        cout << endl;
    }
    cout << "Snake's Wealth: " << score << '$' << endl;
}

bool Board::is_lost() {
    point pt = snake.get_head_pos();
    // if the snake's head touched boundries he lost
    if (pt.r == 0 || pt.r == rows - 1) return true;
    if (pt.c == 0 || pt.c == cols - 1) return true;

    vector<point> &tails = snake.get_tails();
    // if the snake's head touch part of his tails he lost
    for (point &tail_pt: tails) {
        if (tail_pt.r == pt.r && tail_pt.c == pt.c && tails.size() >= 4)
            return true;
    }
    return false;
}

void Board::update_board_elements() {
    point sn = snake.get_head_pos(), frt = dollar.get_pos();
    vector<point> &tails = snake.get_tails();
    if (sn.r == frt.r && sn.c == frt.c) {
        // if the snake ate the dollar
        // increase score
        score++;
        // add another tail (having the position of last tail_point
        if (tails.empty())
            tails.push_back(sn);
        else
            tails.push_back(tails.back());
        // add dollar in another place
        dollar.update_position(get_empty_points());
    }
}

void Board::restart_game() {
    // clean the board
    reset_board();
    // put snake in initial starting position and direction
    snake.set_position({rows / 2, cols / 2});
    snake.set_direction('u');
    score = 0;
    // delete all snake_tails
    snake.get_tails().clear();
    // put the dollar in a new place
    dollar.update_position(get_empty_points());
    // clean screen and play
    system("cls");
    play();
}

char Board::take_input(char x) {
    // parameter x is used when a specific input is sent to the function (it happens only in the start of game)
    char c = 0, dir = snake.get_direction();
    if (x != 0) {
        c = tolower(x);
    } else if (_kbhit()) {
        // if a keyboard key was pressed save it in my char c
        c = tolower(getch());
    }
    // snake directions are : u -> up , d -> down , l -> left , r -> right
    // getchar : wsad : w -> up , s -> down , a -> left , d -> right
    if (c == 'w' && dir != 'd')
        return 'u';
    if (c == 's' && (dir != 'u' || x != 0))
        return 'd';
    if (c == 'a' && dir != 'r')
        return 'l';
    if (c == 'd' && dir != 'l')
        return 'r';
    return '0';
}

void Board::reset_board() {
    // make the board empty with walls on borders
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) ptr[i][j] = '*';
            else ptr[i][j] = ' ';
        }
    }
}

vector<point> Board::get_empty_points() {
    vector<point> v;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (ptr[i][j] == ' ')
                v.push_back({i, j});
        }
    }
    return v;
}

Board::~Board() {
    for (int i = 0; i < rows; ++i) {
        delete[] ptr[i];
    }
    delete[] ptr;
}

int main() {
    // Snake game in Board of size 20X20
    Board game(20, 20);
    game.play();
}

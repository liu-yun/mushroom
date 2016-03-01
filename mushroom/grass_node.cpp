#include "mushroom.h"

GrassNode::GrassNode(int i) : id(i) {
    srand(clock());
    visible = false;
    next = nullptr;
    if (id == -1) {
        type = NOTHING;
        return;
    }
    type = rand() % 3;
    grass_style = rand() % 3;
    score = rand() % 9 + 1;
    do {
        x = rand() % 4;
        y = rand() % 3;
    } while (grid[y][x] == 1);
    grid[y][x] = 1;
    Sleep(1);
}

int GrassNode::grid[3][4] = { 0 };
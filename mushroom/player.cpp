#include "mushroom.h"

Player::Player() {
    x = kWidth / 2 - kPlayerSize;
    y = kBottom - kPlayerSize - 1;
    speed = 2;
    SetDirection(UP);
}

void Player::Move() {
    if (x <= kPlayerSize || y <= kPlayerSize || x >= kWidth - kPlayerSize || y >= kBottom - kPlayerSize) {
        dx = -dx;
        dy = -dy;
        direction += direction > 1 ? -2 : 2;
        x = x <= kPlayerSize ? kPlayerSize + 1 : x;
        y = y <= kPlayerSize ? kPlayerSize + 1 : y;
        x = x >= kWidth - kPlayerSize ? kWidth - kPlayerSize - 1 : x;
        y = y >= kBottom - kPlayerSize ? kBottom - kPlayerSize - 1 : y;
        /*if (x <= 16)
        x = 16 + 1;
        if (y <= 16)
        y = 16 + 1;
        if (x >= 800 - 16)
        x = 800 - 16 - 1;
        if (y >= 490 - 16)
        y = 490 - 16 - 1;*/
    }
    x += speed*dx;
    y += speed*dy;
}

void Player::SetDirection(Direction d) {
    direction = d;
    switch (d) {
        case LEFT:
            dx = -1;
            dy = 0;
            break;
        case UP:
            dx = 0;
            dy = -1;
            break;
        case RIGHT:
            dx = 1;
            dy = 0;
            break;
        case DOWN:
            dx = 0;
            dy = 1;
            break;
        default:
            break;
    }
}

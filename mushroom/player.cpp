#include "mushroom.h"

Player::Player() {
    skin = 0;
    x = kWidth / 2 - kPlayerSize[skin];
    y = kBottom - kPlayerSize[skin] - 1;
    speed = 2;
    SetDirection(UP);
}

void Player::Move() {
    if (x <= kPlayerSize[skin] || y <= kPlayerSize[skin] || x >= kWidth - kPlayerSize[skin] || y >= kBottom - kPlayerSize[skin]) {
        dx = -dx;
        dy = -dy;
        direction += direction > 1 ? -2 : 2;
        x = x <= kPlayerSize[skin] ? kPlayerSize[skin] + 1 : x;
        y = y <= kPlayerSize[skin] ? kPlayerSize[skin] + 1 : y;
        x = x >= kWidth - kPlayerSize[skin] ? kWidth - kPlayerSize[skin] - 1 : x;
        y = y >= kBottom - kPlayerSize[skin] ? kBottom - kPlayerSize[skin] - 1 : y;
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

void Player::Reset() {
    x = kWidth / 2 - kPlayerSize[skin];
    y = kBottom - kPlayerSize[skin] - 1;
    SetDirection(UP);
}

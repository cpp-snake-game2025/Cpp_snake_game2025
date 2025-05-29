// snake.cpp
#include "snake.h"
#include <ncurses.h>

Snake::Snake(int y, int x) {
    body.push_back({y, x});
    body.push_back({y, x - 1});
    body.push_back({y, x - 2});
    dir = RIGHT;
    maxLengthSeen = body.size();
}

void Snake::setDirection(Direction d) {
    if ((dir == UP && d == DOWN) || (dir == DOWN && d == UP) ||
        (dir == LEFT && d == RIGHT) || (dir == RIGHT && d == LEFT))
        return;
    dir = d;
}

Direction Snake::getDirection() const { return dir; }

void Snake::move(bool grow) {
    auto hd = body.front();
    int y = hd.first, x = hd.second;
    switch (dir)
    {
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    }

    body.push_front({y, x});
    if (!grow)
        body.pop_back();
    if ((int)body.size() > maxLengthSeen)
        maxLengthSeen = body.size();
}

void Snake::teleport(int newY, int newX) {
    body.front() = {newY, newX};
}

bool Snake::shrink() {
    if (body.size() <= 3)
        return false;
    body.pop_back();
    return true;
}

bool Snake::checkCollision(int maxY, int maxX) const {
    auto hd = body.front();
    int y = hd.first, x = hd.second;
    if (y <= 0 || y >= maxY - 1 || x <= 0 || x >= maxX - 1)
        return true;
    for (size_t i = 1; i < body.size(); ++i)
        if (body[i] == hd)
            return true;
    return false;
}

std::pair<int, int> Snake::getHead() const { return body.front(); }

void Snake::draw(int oY, int oX) const {
    for (size_t i = 0; i < body.size(); ++i) {
        int y = oY + body[i].first;
        int x = oX + body[i].second;

        if (i == 0) {
            attron(COLOR_PAIR(4));                  // Snake 머리용
            mvaddch(y, x, ' ' | A_REVERSE);        
            attroff(COLOR_PAIR(4));
        } else {
            attron(COLOR_PAIR(3));                  // Snake 몸통용
            mvaddch(y, x, ' ' | A_REVERSE);
            attroff(COLOR_PAIR(3));
        }
    }
}

int Snake::getLength() const { return body.size(); }
int Snake::getMaxLength() const { return maxLengthSeen; }

const std::deque<std::pair<int, int>>& Snake::getBody() const {
    return body;
}
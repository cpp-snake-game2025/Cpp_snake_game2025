// snake.cpp
#include "snake.h"
#include <ncurses.h>

Snake::Snake(int y, int x)
{
    body.push_back({y, x});
    body.push_back({y, x - 1});
    body.push_back({y, x - 2});
    dir = RIGHT;
    maxLengthSeen = body.size();
}

void Snake::setDirection(Direction d) // 현재 뱀의 이동 방향 (dir), 새로 설정하려는 방향 (d)을 인자로 받는 함수
{
    //if ((dir == UP && d == DOWN) || (dir == DOWN && d == UP) ||
    //    (dir == LEFT && d == RIGHT) || (dir == RIGHT && d == LEFT)) 
    //    return;
    dir = d;
}

Direction Snake::getDirection() const { return dir; }

void Snake::move(bool grow)
{
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

void Snake::growAtTail() {
    if (body.size() < 2) return; // 몸이 너무 짧으면 무시

    auto tail = body.back();
    auto beforeTail = body[body.size() - 2];

    // 꼬리 방향 벡터 계산
    int dy = tail.first - beforeTail.first;
    int dx = tail.second - beforeTail.second;

    // 꼬리 뒤 위치는 꼬리 + (꼬리 - beforeTail)
    int newTailY = tail.first + dy;
    int newTailX = tail.second + dx;

    body.push_back({newTailY, newTailX});
}

void Snake::teleport(int newY, int newX)
{
    body.front() = {newY, newX};
}

bool Snake::shrink()
{
    if (body.size() <= 3)
        return false;
    body.pop_back();
    return true;
}

bool Snake::checkCollision(int maxY, int maxX) const
{
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

void Snake::draw(int oY, int oX) const
{
    for (size_t i = 0; i < body.size(); ++i)
    {
        attron(COLOR_PAIR(1));
        mvaddch(oY + body[i].first, oX + body[i].second,
                (i == 0 ? 'H' : 'o'));
        attroff(COLOR_PAIR(1));
    }
}

int Snake::getLength() const { return body.size(); }
int Snake::getMaxLength() const { return maxLengthSeen; }

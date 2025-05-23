// snake.h
#ifndef SNAKE_H
#define SNAKE_H

#include <deque>
#include <utility>

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake
{
public:
    Snake(int startY, int startX);
    void setDirection(Direction d);
    Direction getDirection() const;
    void move(bool grow = false);
    void teleport(int newY, int newX);
    bool shrink();
    bool checkCollision(int maxY, int maxX) const;
    std::pair<int, int> getHead() const;
    void draw(int offsetY, int offsetX) const;
    int getLength() const;
    int getMaxLength() const;

private:
    std::deque<std::pair<int, int>> body;
    Direction dir;
    int maxLengthSeen;
};

#endif

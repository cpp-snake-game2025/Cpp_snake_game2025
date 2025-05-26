// map.h
#ifndef MAP_H
#define MAP_H

#include <vector>

class GameMap
{
public:
    GameMap();
    std::vector<std::vector<int>> &getMap();
    int getWidth() const;
    int getHeight() const;
    void setSnake();
    void draw(int offsetY, int offsetX) const;

private:
    std::vector<std::vector<int>> map;
    int width, height;
};

#endif

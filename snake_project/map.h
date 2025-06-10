// map.h
#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

class GameMap
{
public:
    // 기존: 인자가 없으면 "map.txt"를 읽어옴
    GameMap(const std::string &filename = "map.txt");
    void draw(int offsetY, int offsetX) const;
    int getHeight() const;
    int getWidth() const;
    const std::vector<std::vector<int>> &getMap() const;

private:
    std::vector<std::vector<int>> grid;
    int height;
    int width;
};

#endif

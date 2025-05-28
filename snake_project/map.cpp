// map.cpp
#include "map.h"
#include <ncurses.h>

GameMap::GameMap()
{
    width = 21;
    height = 21;
    map.assign(height, std::vector<int>(width, 0));

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
                map[y][x] = 1; // 일반 Wall

    // Immune Wall (모서리)
    map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 2;
}

std::vector<std::vector<int>> &GameMap::getMap()
{
    return map;
}

int GameMap::getWidth() const { return width; }
int GameMap::getHeight() const { return height; }

void GameMap::setSnake()
{
    map[5][5] = 3; // Snake Head
    map[5][4] = 4; // Snake Body
    map[5][3] = 4;
}

void GameMap::draw(int offsetY, int offsetX) const
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            move(offsetY + y, offsetX + x);
            switch (map[y][x])
            {
            case 0:
                addch(' ');
                break;
            case 1:
                addch('#');
                break;
            case 2:
                addch('X');
                break;
            case 3:
                addch('H');
                break;
            case 4:
                addch('o');
                break;
            default:
                addch('?');
                break;
            }
        }
    }
}

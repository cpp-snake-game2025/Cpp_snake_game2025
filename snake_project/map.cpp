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
                attron(COLOR_PAIR(6));         // 일반 Wall 색상 (보라)
                addch(' ');                    // 공백 네모로 표시
                attroff(COLOR_PAIR(6));
                break;
            case 2:
                attron(COLOR_PAIR(5));         // Immune Wall 색상 (노랑)
                addch(' ');                    // 공백 네모로 표시
                attroff(COLOR_PAIR(5));
                break;
            case 3:
                attron(COLOR_PAIR(4));         // Snake Head (흰색)
                addch(' ');
                attroff(COLOR_PAIR(4));
                break;
            case 4:
                attron(COLOR_PAIR(3));         // Snake Body (하늘색)
                addch(' ');
                attroff(COLOR_PAIR(3));
                break;
            default:
                addch('?');
                break;
            }
        }
    }
}


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

    // Gate: 벽(경계) 위치에 배치
    map[5][0] = 7;         // 왼쪽 벽
    map[5][width - 1] = 7; // 오른쪽 벽
}

const std::vector<std::vector<int>> &GameMap::getMap() const
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
                attron(COLOR_PAIR(6));
                addch(' ' | A_REVERSE);
                attroff(COLOR_PAIR(6));
                break;
            case 2:
                attron(COLOR_PAIR(6));
                addch(' ' | A_REVERSE);
                attroff(COLOR_PAIR(6));
                break;
            case 3:
                attron(COLOR_PAIR(4));
                addch(' ' | A_REVERSE);
                attroff(COLOR_PAIR(4));
                break;
            case 4:
                attron(COLOR_PAIR(3));
                addch(' ' | A_REVERSE);
                attroff(COLOR_PAIR(3));
                break;
            case 7:
                attron(COLOR_PAIR(5));
                addch(' ' | A_REVERSE);
                attroff(COLOR_PAIR(5));
                break;
            default:
                addch('?');
                break;
            }
        }
    }
}


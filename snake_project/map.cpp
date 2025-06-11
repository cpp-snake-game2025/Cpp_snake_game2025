#include "map.h"
#include <ncurses.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream> // cerr 사용

GameMap::GameMap(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "Error: Cannot open map file '" << filename << "'\n";
        height = 0;
        width = 0;
        return;
    }

    std::string line;
    std::vector<int> row;
    while (std::getline(in, line))
    {
        row.clear();
        std::istringstream iss(line);
        int x;
        while (iss >> x)
        {
            row.push_back(x);
        }
        grid.push_back(row);
    }
    height = static_cast<int>(grid.size());
    width = static_cast<int>(grid.empty() ? 0 : grid[0].size());
}

void GameMap::draw(int offsetY, int offsetX) const
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int cell = grid[y][x];
            int screenY = y + offsetY;
            int screenX = x + offsetX;
            switch (cell)
            {
            case 0: // 빈 공간
                mvprintw(screenY, screenX, " ");
                break;
            case 1: // Wall
                attron(COLOR_PAIR(6));
                mvprintw(screenY, screenX, " ");
                attroff(COLOR_PAIR(6));
                break;
            case 2: // Immune Wall
                attron(COLOR_PAIR(5));
                mvprintw(screenY, screenX, " ");
                attroff(COLOR_PAIR(5));
                break;
            case 5: // Growth Item
                attron(COLOR_PAIR(1));
                mvprintw(screenY, screenX, " ");
                attroff(COLOR_PAIR(1));
                break;
            case 6: // Poison Item
                attron(COLOR_PAIR(2));
                mvprintw(screenY, screenX, " ");
                attroff(COLOR_PAIR(2));
                break;
            case 7: // Gate (단순히 map 상에 표시만 필요 없는 경우도 있으므로, 여기선 무시)
                break;
            default:
                mvprintw(screenY, screenX, "?");
                break;
            }
        }
    }
}

int GameMap::getHeight() const
{
    return height;
}

int GameMap::getWidth() const
{
    return width;
}

const std::vector<std::vector<int>> &GameMap::getMap() const
{
    return grid;
}

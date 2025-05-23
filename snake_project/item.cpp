// item.cpp
#include "item.h"
#include <ncurses.h>
#include <cstdlib>
#include <algorithm>
#include <ctime>

ItemManager::ItemManager(int y, int x)
    : maxY(y), maxX(x)
{
    srand(time(nullptr));
}

void ItemManager::update()
{
    time_t now = time(nullptr);
    items.erase(std::remove_if(items.begin(), items.end(),
                               [now](const Item &i)
                               { return difftime(now, i.spawnTime) > 5.0; }),
                items.end());
}

void ItemManager::placeItems(const std::vector<std::vector<int>> &map, int maxCount)
{
    while ((int)items.size() < maxCount)
    {
        int y = rand() % (maxY - 2) + 1,
            x = rand() % (maxX - 2) + 1;
        if (map[y][x] != 0)
            continue;
        bool overlap = false;
        for (auto &it : items)
            if (it.y == y && it.x == x)
            {
                overlap = true;
                break;
            }
        if (overlap)
            continue;
        ItemType t = (rand() % 2 == 0 ? GROWTH : POISON);
        items.push_back({y, x, t, time(nullptr)});
    }
}

void ItemManager::draw(int offsetY, int offsetX) const
{
    for (auto &it : items)
    {
        char ch = it.type == GROWTH ? '+' : '-';
        mvaddch(offsetY + it.y, offsetX + it.x, ch);
    }
}

ItemType ItemManager::checkItem(int y, int x) const
{
    for (auto &it : items)
        if (it.y == y && it.x == x)
            return it.type;
    return static_cast<ItemType>(0);
}

void ItemManager::removeItemAt(int y, int x)
{
    items.erase(std::remove_if(items.begin(), items.end(),
                               [=](const Item &i)
                               { return i.y == y && i.x == x; }),
                items.end());
}

int ItemManager::getItemCount() const { return items.size(); }

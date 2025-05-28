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
        int r = rand() % 3; // 0, 1, 2 중 하나
        ItemType t;
        if (r == 0)
            t = GROWTH; // 33% 확률로 성장 아이템
        else if (r == 1)
            t = POISON; // 33% 확률로 독 아이템
        else
            t = DOUBLE_EFFECT; // 33% 확률로 효과 두 배 아이템
        items.push_back({y, x, t, time(nullptr)});
    }
}

void ItemManager::draw(int offsetY, int offsetX) const
{
    for (auto &it : items)
    {
        char ch;
        switch (it.type)
        {
        case GROWTH:
            ch = '+';
            attron(COLOR_PAIR(3)); // Growth item color
            break;
        case POISON:
            ch = '-';
            attron(COLOR_PAIR(4)); // Poison item color
            break;
        case DOUBLE_EFFECT:
            ch = '*';
            attron(COLOR_PAIR(5)); // DOUBLE_EFFECT 아이템 색상
            break;
        default:
            ch = '?';
            break;
        }

        mvaddch(offsetY + it.y, offsetX + it.x, ch);
        // 아이템별로 사용한 색상쌍을 해제
        switch (it.type)
        {
        case GROWTH:
            attroff(COLOR_PAIR(3));
            break;
        case POISON:
            attroff(COLOR_PAIR(4));
            break;
        case DOUBLE_EFFECT:
            attroff(COLOR_PAIR(5));
            break;
        default:
            break;
        }
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

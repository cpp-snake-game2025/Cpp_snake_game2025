// item.h
#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <ctime>
#include "snake.h"
enum ItemType
{
    GROWTH = 5,
    POISON = 6,
    DOUBLE_EFFECT = 8 // 아이템 효과 두 배
};

class ItemManager
{
public:
    ItemManager(int maxY, int maxX);
    void update(const std::vector<std::vector<int>> &map, const Snake &snake);
    void placeItems(const std::vector<std::vector<int>> &map, const Snake &snake, int maxCount);
    void draw(int offsetY, int offsetX) const;
    ItemType checkItem(int y, int x) const;
    void removeItemAt(int y, int x);
    int getItemCount() const;
    

private:
    struct Item
    {
        int y, x;
        ItemType type;
        time_t spawnTime;
    };
    std::vector<Item> items;
    time_t lastResetTime;
    int maxY, maxX;
};

#endif

// item.h
#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <ctime>
enum ItemType
{
    GROWTH = 5,
    POISON = 6
};

class ItemManager
{
public:
    ItemManager(int maxY, int maxX);
    void update();
    void placeItems(const std::vector<std::vector<int>> &map, int maxCount = 3);
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
    int maxY, maxX;
};

#endif

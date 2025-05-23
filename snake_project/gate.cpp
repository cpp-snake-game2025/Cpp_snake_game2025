#include "gate.h"
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "snake.h"

GateManager::GateManager(int y, int x) : maxY(y), maxX(x)
{
    srand(time(nullptr));
}

void GateManager::setGates(const std::vector<std::vector<int>> &map)
{
    std::vector<std::pair<int, int>> wallList;

    for (int y = 0; y < maxY; ++y)
    {
        for (int x = 0; x < maxX; ++x)
        {
            if (map[y][x] == 1)
                wallList.emplace_back(y, x);
        }
    }

    if (wallList.size() < 2)
        return;

    int idx1 = rand() % wallList.size();
    int idx2;
    do
    {
        idx2 = rand() % wallList.size();
    } while (idx2 == idx1);

    gateA = wallList[idx1];
    gateB = wallList[idx2];
}

void GateManager::draw(int offsetY, int offsetX) const
{
    mvaddch(offsetY + gateA.first, offsetX + gateA.second, 'A');
    mvaddch(offsetY + gateB.first, offsetX + gateB.second, 'B');
}

bool GateManager::isGate(int y, int x) const
{
    return (gateA == std::make_pair(y, x) || gateB == std::make_pair(y, x));
}

GateRole GateManager::getGateRole(int y, int x) const
{
    if (gateA == std::make_pair(y, x))
        return ENTER;
    if (gateB == std::make_pair(y, x))
        return ENTER;
    return NONE;
}

std::pair<int, int> GateManager::getExitGate(int y, int x) const
{
    return (gateA == std::make_pair(y, x)) ? gateB : gateA;
}

bool GateManager::isOuterWall(int y, int x) const
{
    return (y == 0 || y == maxY - 1 || x == 0 || x == maxX - 1);
}

// 방향 우선순위: 동일 → 시계 → 반시계 → 반대
int GateManager::getExitDirection(int enterDir, int y, int x, const std::vector<std::vector<int>> &map)
{
    std::vector<std::pair<int, int>> dirs = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1} // UP, DOWN, LEFT, RIGHT
    };
    std::vector<int> order;

    if (isOuterWall(y, x))
    {
        if (y == 0)
            return DOWN;
        if (y == maxY - 1)
            return UP;
        if (x == 0)
            return RIGHT;
        if (x == maxX - 1)
            return LEFT;
    }

    // 중심부 gate인 경우
    switch (enterDir)
    {
    case UP:
        order = {UP, RIGHT, LEFT, DOWN};
        break;
    case DOWN:
        order = {DOWN, LEFT, RIGHT, UP};
        break;
    case LEFT:
        order = {LEFT, UP, DOWN, RIGHT};
        break;
    case RIGHT:
        order = {RIGHT, DOWN, UP, LEFT};
        break;
    }

    for (int d : order)
    {
        int ny = y + dirs[d].first;
        int nx = x + dirs[d].second;
        if (map[ny][nx] == 0)
            return d;
    }

    return enterDir; // fallback
}

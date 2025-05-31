#include "gate.h"
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include "snake.h"

GateManager::GateManager(int y, int x)
    : maxY(y), maxX(x), gateA({-1, -1}), gateB({-1, -1})
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void GateManager::setGates(const std::vector<std::vector<int>> &mapData)
{
    std::vector<std::pair<int, int>> wallList;

    // 맵 전체에서 벽(값 1)을 찾아 wallList에 추가
    for (int yy = 0; yy < maxY; ++yy)
    {
        for (int xx = 0; xx < maxX; ++xx)
        {
            if (mapData[yy][xx] == 1)
            {
                wallList.emplace_back(yy, xx);
            }
        }
    }

    // wallList에서 무작위로 두 개 선택
    if (wallList.size() >= 2)
    {
        int idxA = rand() % wallList.size();
        gateA = wallList[idxA];
        int idxB;
        do
        {
            idxB = rand() % wallList.size();
        } while (idxB == idxA);
        gateB = wallList[idxB];
    }
    else
    {
        // 벽이 충분하지 않으면 gateA/B를 무효 상태로 둠
        gateA = {-1, -1};
        gateB = {-1, -1};
    }
}

bool GateManager::isGate(int y, int x) const
{
    return (y == gateA.first && x == gateA.second) ||
           (y == gateB.first && x == gateB.second);
}

GateRole GateManager::getGateRole(int y, int x) const
{
    if (y == gateA.first && x == gateA.second)
        return ENTER;
    if (y == gateB.first && x == gateB.second)
        return EXIT;
    return NONE;
}

std::pair<int, int> GateManager::getExitGate(int y, int x) const
{
    if (y == gateA.first && x == gateA.second)
        return gateB;
    else
        return gateA;
}

int GateManager::getExitDirection(int enterDir,
                                  int exitY,
                                  int exitX,
                                  const std::vector<std::vector<int>> &mapData) const
{
    // 1) 맵 경계에 있는 게이트: 무조건 안쪽
    if (exitY == 0)
        return DOWN;
    if (exitY == maxY - 1)
        return UP;
    if (exitX == 0)
        return RIGHT;
    if (exitX == maxX - 1)
        return LEFT;

    // 2) 경계가 아닌 경우: enterDir → 시계 → 반시계 → 역방향 순서
    auto canMove = [&](int dir)
    {
        int ny = exitY;
        int nx = exitX;
        switch (dir)
        {
        case UP:
            ny -= 1;
            break;
        case DOWN:
            ny += 1;
            break;
        case LEFT:
            nx -= 1;
            break;
        case RIGHT:
            nx += 1;
            break;
        }
        if (ny < 0 || ny >= maxY || nx < 0 || nx >= maxX)
            return false;
        if (mapData[ny][nx] == 2) // Immune Wall
            return false;
        return true;
    };

    if (canMove(enterDir))
        return enterDir;

    int clockDir;
    switch (enterDir)
    {
    case UP:
        clockDir = RIGHT;
        break;
    case RIGHT:
        clockDir = DOWN;
        break;
    case DOWN:
        clockDir = LEFT;
        break;
    case LEFT:
        clockDir = UP;
        break;
    default:
        clockDir = UP;
        break;
    }
    if (canMove(clockDir))
        return clockDir;

    int counterDir;
    switch (enterDir)
    {
    case UP:
        counterDir = LEFT;
        break;
    case LEFT:
        counterDir = DOWN;
        break;
    case DOWN:
        counterDir = RIGHT;
        break;
    case RIGHT:
        counterDir = UP;
        break;
    default:
        counterDir = UP;
        break;
    }
    if (canMove(counterDir))
        return counterDir;

    int opposite;
    switch (enterDir)
    {
    case UP:
        opposite = DOWN;
        break;
    case DOWN:
        opposite = UP;
        break;
    case LEFT:
        opposite = RIGHT;
        break;
    case RIGHT:
        opposite = LEFT;
        break;
    default:
        opposite = UP;
        break;
    }
    return opposite;
}

void GateManager::draw(int offsetY, int offsetX) const
{
    if (gateA.first >= 0 && gateA.second >= 0)
    {
        attron(COLOR_PAIR(8));
        mvprintw(gateA.first + offsetY, gateA.second + offsetX, " ");
        attroff(COLOR_PAIR(8));
    }
    if (gateB.first >= 0 && gateB.second >= 0)
    {
        attron(COLOR_PAIR(8));
        mvprintw(gateB.first + offsetY, gateB.second + offsetX, " ");
        attroff(COLOR_PAIR(8));
    }
}

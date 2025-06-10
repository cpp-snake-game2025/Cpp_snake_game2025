// item.cpp
#include "item.h"
#include <ncurses.h>
#include <cstdlib>
#include <algorithm>
#include <ctime>

// 생성자: 맵 크기 설정 후 랜덤 시드 초기화
ItemManager::ItemManager(int y, int x)
    : maxY(y), maxX(x)
{
    srand(time(nullptr));  // 매 실행마다 다른 랜덤값 생성
    lastResetTime = time(nullptr) - 8; // 리셋 기준 시각 설정
}

void ItemManager::update(const std::vector<std::vector<int>> &map, const Snake &snake)
{
    time_t now = time(nullptr);

    // 8초마다 전체 아이템 리셋 및 새로 배치
    if (difftime(now, lastResetTime) >= 8.0) {
        items.clear();
        lastResetTime = now;

        // 항상 3개 생성
        std::vector<ItemType> forcedItems = {GROWTH, POISON, DOUBLE_EFFECT};

        for (ItemType t : forcedItems) {
            while (true) {
                int y = rand() % (maxY - 2) + 1;
                int x = rand() % (maxX - 2) + 1;

                if (map[y][x] != 0)
                    continue;

                auto isWallOrGate = [](int v) { return v == 1 || v == 2 || v == 7; };
                if (isWallOrGate(map[y-1][x]) || isWallOrGate(map[y+1][x]) ||
                    isWallOrGate(map[y][x-1]) || isWallOrGate(map[y][x+1]))
                    continue;

                bool overlap = false;
                for (auto &it : items)
                    if (it.y == y && it.x == x) {
                        overlap = true;
                        break;
                    }
                if (overlap)
                    continue;

                const auto &body = snake.getBody();
                bool onSnake = false;
                for (const auto &part : body) {
                    if (part.first == y && part.second == x) {
                        onSnake = true;
                        break;
                    }
                }
                if (onSnake)
                    continue;

                items.push_back({y, x, t, time(nullptr)});
                break;
            }
        }
    }
}


// 아이템 배치 함수
void ItemManager::placeItems(const std::vector<std::vector<int>> &map, const Snake &snake, int maxCount)
{
    // 이미 충분한 아이템이 있으면 리턴
    if ((int)items.size() >= maxCount)
        return;

    // 필요한 아이템 수
    int needed = maxCount - items.size();

    // 강제 생성 순서: 성장, 독
    std::vector<ItemType> forcedItems;
    if (needed >= 1) forcedItems.push_back(GROWTH);
    if (needed >= 2) forcedItems.push_back(POISON);

    // 남은 한 개는 랜덤하게 설정
    if (needed >= 3) {
        int r = rand() % 3;
        ItemType randomItem = (r == 0) ? GROWTH : (r == 1) ? POISON : DOUBLE_EFFECT;
        forcedItems.push_back(randomItem);
    }

    // 아이템들 배치
    for (ItemType t : forcedItems) {
        while (true) {
            int y = rand() % (maxY - 2) + 1;
            int x = rand() % (maxX - 2) + 1;

            if (map[y][x] != 0)
                continue;

            // [1] 벽 또는 벽과 인접한 곳이면 생성 금지
            auto isWallOrGate = [](int v) { return v == 1 || v == 2 || v == 7; };
            if (isWallOrGate(map[y-1][x]) || isWallOrGate(map[y+1][x]) ||
                isWallOrGate(map[y][x-1]) || isWallOrGate(map[y][x+1]))
                continue;

            // [2] 아이템과 겹치면 안됨
            bool overlap = false;
            for (auto &it : items)
                if (it.y == y && it.x == x) {
                    overlap = true;
                    break;
                }
            if (overlap)
                continue;

            // [3] Snake 몸통과 겹치면 안됨
            const auto &body = snake.getBody();
            bool onSnake = false;
            for (const auto &part : body) {
                if (part.first == y && part.second == x) {
                    onSnake = true;
                    break;
                }
            }
            if (onSnake)
                continue;

            items.push_back({y, x, t, time(nullptr)});
            break;
        }
    }
}

// 아이템을 화면에 출력
void ItemManager::draw(int offsetY, int offsetX) const
{
    for (const auto &item : items) {
        switch (item.type) {
            case GROWTH:
                attron(COLOR_PAIR(1));
                mvprintw(offsetY + item.y, offsetX + item.x, " ");
                attroff(COLOR_PAIR(1));
                break;
            case POISON:
                attron(COLOR_PAIR(2));
                mvprintw(offsetY + item.y, offsetX + item.x, " ");
                attroff(COLOR_PAIR(2));
                break;
            case DOUBLE_EFFECT:
                attron(COLOR_PAIR(7));
                mvprintw(offsetY + item.y, offsetX + item.x, " ");
                attroff(COLOR_PAIR(7));
                break;
        }
    }
}

// 아이템 존재 여부 및 타입 확인
ItemType ItemManager::checkItem(int y, int x) const
{
    for (auto &it : items)
        if (it.y == y && it.x == x)
            return it.type;
    return static_cast<ItemType>(0); // 없으면 0 반환
}


// 해당 위치의 아이템 제거
void ItemManager::removeItemAt(int y, int x)
{
    items.erase(std::remove_if(items.begin(), items.end(),
                               [=](const Item &i)
                               { return i.y == y && i.x == x; }),
                items.end());
}

// 현재 아이템 수 반환
int ItemManager::getItemCount() const { return items.size(); }

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

    if (difftime(now, lastResetTime) >= 8.0) {
        items.clear();
        lastResetTime = now;

        // 성장, 독 아이템 하나씩 생성(고정)
        int count = 0;
        std::vector<ItemType> types = {GROWTH, POISON};

        // 마지막 하나는 랜덤하게 추가
        ItemType randomExtra = (rand() % 2 == 0) ? GROWTH : POISON;
        types.push_back(randomExtra);

        while (count < 3) {
            int y = rand() % (maxY - 2) + 1;
            int x = rand() % (maxX - 2) + 1;

            // [1] 벽 또는 벽과 인접한 곳이면 생성 금지
            auto isWallOrGate = [](int v) { return v == 1 || v == 2 || v == 7; };
            if (isWallOrGate(map[y][x]) || isWallOrGate(map[y-1][x]) || isWallOrGate(map[y+1][x]) || isWallOrGate(map[y][x-1]) || isWallOrGate(map[y][x+1]))
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

            // [3] Snake 몸체와 겹치면 안됨
            const auto &body = snake.getBody();
            bool onSnake = false;
            for (auto &part : body) {
                if (part.first == y && part.second == x) {
                    onSnake = true;
                    break;
                }
            }
            if (onSnake)
                continue;

            // 타입 정해서 추가
            ItemType t = types[count];
            items.push_back({y, x, t, now});
            count++;
        }
    }
}

// 아이템 배치 함수
void ItemManager::placeItems(const std::vector<std::vector<int>> &map, int maxCount)
{
    while ((int)items.size() < maxCount) {
        // 1~(maxY-2), 1~(maxX-2) 범위 내에서 랜덤 위치 선정
        int y = rand() % (maxY - 2) + 1;
        int x = rand() % (maxX - 2) + 1;

        // 빈 공간이 아니면 배치 불가
        if (map[y][x] != 0)
            continue;

        // 기존 아이템과 겹치는지 확인
        bool overlap = false;
        for (auto &it : items)
            if (it.y == y && it.x == x) {
                overlap = true;
                break;
            }
        if (overlap)
            continue;

        // 아이템 생성 및 추가
        ItemType t = (rand() % 2 == 0 ? GROWTH : POISON);
        items.push_back({y, x, t, time(nullptr)});
    }
}

void ItemManager::placeItems(const std::vector<std::vector<int>> &map, const Snake &snake, int maxCount)
{
    while ((int)items.size() < maxCount) {
        int y = rand() % (maxY - 2) + 1;
        int x = rand() % (maxX - 2) + 1;

        if (map[y][x] != 0)
            continue;

        bool overlap = false;
        for (auto &it : items)
            if (it.y == y && it.x == x) {
                overlap = true;
                break;
            }
        if (overlap)
            continue;

        // Snake 몸체와 겹치지 않도록 확인
        const auto& body = snake.getBody();
        bool onSnake = false;
        for (auto &part : body) {
            if (part.first == y && part.second == x) {
                onSnake = true;
                break;
            }
        }
        if (onSnake)
            continue;

        ItemType t = (rand() % 2 == 0 ? GROWTH : POISON);
        items.push_back({y, x, t, time(nullptr)});
    }
}


// 아이템을 화면에 출력
void ItemManager::draw(int offsetY, int offsetX) const
{
    for (auto &it : items) {
        char ch = it.type == GROWTH ? '+' : '-';

        // 색상쌍 적용: GROWTH는 green(1), POISON은 red(2)
        attron(COLOR_PAIR(it.type == GROWTH ? 1 : 2));
        mvaddch(offsetY + it.y, offsetX + it.x, ch);
        attroff(COLOR_PAIR(it.type == GROWTH ? 1 : 2));
    }
}

// 아이템 존재 여부 및 타입 확인
ItemType ItemManager::checkItem(int y, int x) const
{
    for (auto &it : items)
        if (it.y == y && it.x == x)
            return it.type;
    return static_cast<ItemType>(0);  // 없으면 0 반환
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
int ItemManager::getItemCount() const
{
    return items.size();
}

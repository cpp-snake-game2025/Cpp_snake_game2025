// item.h
#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <ctime>
#include "snake.h"

// 아이템 종류 정의
enum ItemType
{
    GROWTH = 5,  // 성장 아이템
    POISON = 6   // 독 아이템
};

// 아이템 생성, 관리, 삭제 클래스
class ItemManager
{
public:
    // 생성자: 맵의 최대 높이와 너비를 받아 초기화
    ItemManager(int maxY, int maxX);

    // 일정 시간 지난 아이템 제거
    void update(const std::vector<std::vector<int>> &map, const Snake &snake);

    // 맵 위에 새로운 아이템을 배치
    void placeItems(const std::vector<std::vector<int>> &map, int maxCount = 3);
    void placeItems(const std::vector<std::vector<int>> &map, const Snake &snake, int maxCount = 3);

    // 아이템을 화면에 출력 (오프셋 포함)
    void draw(int offsetY, int offsetX) const;

    // 해당 위치에 아이템이 있는지 확인하고, 있으면 종류 반환
    ItemType checkItem(int y, int x) const;

    // 해당 위치의 아이템 제거
    void removeItemAt(int y, int x);

    // 현재 존재하는 아이템 개수 반환
    int getItemCount() const;

private:
    // 개별 아이템 정보 구조체
    struct Item
    {
        int y, x;           // 좌표
        ItemType type;      // 종류 (GROWTH or POISON)
        time_t spawnTime;   // 생성된 시각 (지속 시간 계산용)
    };

    std::vector<Item> items;  // 현재 존재하는 아이템 리스트
    int maxY, maxX;           // 맵의 최대 높이와 너비
    time_t lastResetTime;     // 전체 리셋 타이머 기준
};

#endif

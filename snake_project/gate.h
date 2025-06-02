#ifndef GATE_H
#define GATE_H

#include <vector>
#include <utility>

class GateManager
{
public:
    // 맵 전체(grid)는 0: 빈칸, 1: 벽, 2: 면역벽 등으로 가정
    // setGates() 호출 시 이번 스테이지에 새로 등장할 게이트 A/B 좌표를 결정한다.
    void setGates(const std::vector<std::vector<int>> &grid);

    // (y, x)가 현재 게이트인지 확인
    bool isGate(int y, int x) const;

    // 현재 게이트를 통과했을 때 나올 반대쪽 게이트 좌표
    std::pair<int, int> getExitGate(int y, int x) const;

    // exitGate 위치에서 나올 방향 계산
    int getExitDirection(int enterDir,
                         int exitY, int exitX,
                         const std::vector<std::vector<int>> &grid) const;

    // 실제 화면에 Gate를 그린다
    void draw(int offsetY, int offsetX) const;

private:
    // 선택된 두 개의 게이트 좌표
    std::pair<int, int> gateA{-1, -1}, gateB{-1, -1};
};

#endif

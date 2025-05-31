#ifndef GATE_H
#define GATE_H

#include <utility>
#include <vector>

enum GateRole
{
    NONE,
    ENTER,
    EXIT
};

class GateManager
{
public:
    // 생성자: 맵의 높이(maxY)와 너비(maxX)를 전달받아 초기화
    GateManager(int maxY, int maxX);

    // 맵 데이터를 const 참조로 받아서 gateA, gateB를 설정
    void setGates(const std::vector<std::vector<int>> &mapData);

    // (y,x)에 게이트가 존재하는지 여부
    bool isGate(int y, int x) const;

    // (y,x)에 해당하는 게이트의 역할(ENTER/EXIT/NONE) 반환
    GateRole getGateRole(int y, int x) const;

    // (y,x)에 해당하는 게이트의 반대편 좌표 반환
    std::pair<int, int> getExitGate(int y, int x) const;

    // 출구 게이트 위치에서 나올 방향 계산
    int getExitDirection(int enterDir,
                         int exitY,
                         int exitX,
                         const std::vector<std::vector<int>> &mapData) const;

    // 화면에 게이트를 그리기 위한 메서드 (ncurses 사용)
    void draw(int offsetY, int offsetX) const;

private:
    // 중앙에 떠 있는 벽 등 구분이 필요하지 않으므로, isOuterWall 로직은 삭제
    // 대신 필요하다면 y,x 좌표만 보고 경계인지 판단하도록 구현 가능

    std::pair<int, int> gateA, gateB;
    int maxY, maxX;
};

#endif

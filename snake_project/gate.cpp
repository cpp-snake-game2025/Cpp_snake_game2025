#include "gate.h"
#include "snake.h" // Direction enum(UP, DOWN, LEFT, RIGHT) 정의를 위해 추가
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

// 중앙 3×3 벽 영역(21×21 맵 기준)의 인덱스(0-based)
// 중앙은 10, 11, 12 (즉 11번째,12번째,13번째 줄/열)
static const int CENTER_START = 10;
static const int CENTER_END = 12;

// 상하좌우 중 적어도 하나가 빈 공간(0)인지 확인
bool isValidGatePosition(const std::vector<std::vector<int>> &grid, int y, int x) {
    if (grid[y][x] != 1) return false;

    // 상하좌우 방향
    static const int dy[4] = {-1, 1, 0, 0};
    static const int dx[4] = {0, 0, -1, 1};
    int height = grid.size();
    int width = grid[0].size();

    for (int d = 0; d < 4; ++d) {
        int ny = y + dy[d];
        int nx = x + dx[d];
        if (ny >= 0 && ny < height && nx >= 0 && nx < width && grid[ny][nx] == 0) {
            return true; // 한 방향이라도 열려있으면 유효
        }
    }
    return false;
}

void GateManager::setGates(const std::vector<std::vector<int>> &grid)
{
    // srand는 프로그램 시작 시 한 번만 호출할 수 있도록 처리
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // 1) 모든 벽(grid[y][x] == 1) 좌표를 수집하되, 중앙 3×3 영역은 제외
    std::vector<std::pair<int, int>> candidates;
    int height = static_cast<int>(grid.size());
    int width = static_cast<int>(grid[0].size());

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (grid[y][x] != 1)
                continue;

            // (y,x)가 중앙 3×3 블록 내부면 제외
            if (y >= CENTER_START && y <= CENTER_END &&
                x >= CENTER_START && x <= CENTER_END)
            {
                continue;
            }

            // 꼭짓점 네 자리 제외
            if ((y == 0 && x == 0) ||
                (y == 0 && x == width - 1) ||
                (y == height - 1 && x == 0) ||
                (y == height - 1 && x == width - 1))
            {
                continue;
            }

            if (isValidGatePosition(grid, y, x)) {
                // 중앙 영역이 아니라면 후보에 추가
                candidates.emplace_back(y, x);
            }
        }
    }

    // 2) 후보 중에서 서로 다른 두 개를 무작위로 선택
    if (candidates.size() < 2)
    {
        // 충분한 벽 위치가 없으면 gateA/B 초기화
        gateA = {-1, -1};
        gateB = {-1, -1};
        return;
    }

    int idxA = std::rand() % static_cast<int>(candidates.size());
    gateA = candidates[idxA];

    int idxB;
    do
    {
        idxB = std::rand() % static_cast<int>(candidates.size());
    } while (idxB == idxA);

    gateB = candidates[idxB];
}

bool GateManager::isGate(int y, int x) const
{
    return (y == gateA.first && x == gateA.second) ||
           (y == gateB.first && x == gateB.second);
}

std::pair<int, int> GateManager::getExitGate(int y, int x) const
{
    if (y == gateA.first && x == gateA.second)
    {
        return gateB;
    }
    else
    {
        return gateA;
    }
}

// 수정 - 게이트 출구에서의 뱀 이동 방향 결정
// 항상 벽이 아닌 방향으로만 진출하도록 보장
int GateManager::getExitDirection(int enterDir,
                                  int exitY, int exitX,
                                  const std::vector<std::vector<int>> &grid) const
{
    int height = grid.size();
    int width = grid[0].size();
    static const int dy[4] = {-1, 0, 1, 0}; // UP, RIGHT, DOWN, LEFT
    static const int dx[4] = {0, 1, 0, -1};

    // 1. 외곽일 경우 → 무조건 안쪽 방향으로
    if (exitY == 0 && grid[exitY + 1][exitX] == 0) return DOWN;
    if (exitY == height - 1 && grid[exitY - 1][exitX] == 0) return UP;
    if (exitX == 0 && grid[exitY][exitX + 1] == 0) return RIGHT;
    if (exitX == width - 1 && grid[exitY][exitX - 1] == 0) return LEFT;

    // 2. 내부일 경우 → 진출 후보 우선순위 구성
    bool upOpen    = exitY > 0            && grid[exitY - 1][exitX] == 0;
    bool downOpen  = exitY < height - 1   && grid[exitY + 1][exitX] == 0;
    bool leftOpen  = exitX > 0            && grid[exitY][exitX - 1] == 0;
    bool rightOpen = exitX < width - 1    && grid[exitY][exitX + 1] == 0;

    int candidates[4];
    int cSize = 0;

    // 진입 방향 기반 우선순위 적용
    switch (enterDir)
    {
    case UP:
        if (upOpen) candidates[cSize++] = UP;
        if (leftOpen) candidates[cSize++] = LEFT;
        if (rightOpen) candidates[cSize++] = RIGHT;
        if (downOpen) candidates[cSize++] = DOWN;
        break;
    case DOWN:
        if (downOpen) candidates[cSize++] = DOWN;
        if (rightOpen) candidates[cSize++] = RIGHT;
        if (leftOpen) candidates[cSize++] = LEFT;
        if (upOpen) candidates[cSize++] = UP;
        break;
    case LEFT:
        if (leftOpen) candidates[cSize++] = LEFT;
        if (upOpen) candidates[cSize++] = UP;
        if (downOpen) candidates[cSize++] = DOWN;
        if (rightOpen) candidates[cSize++] = RIGHT;
        break;
    case RIGHT:
        if (rightOpen) candidates[cSize++] = RIGHT;
        if (downOpen) candidates[cSize++] = DOWN;
        if (upOpen) candidates[cSize++] = UP;
        if (leftOpen) candidates[cSize++] = LEFT;
        break;
    }

    // 3. 후보 중 가능한 방향 반환
    for (int i = 0; i < cSize; ++i)
        return candidates[i];

    // 4. fallback: 전방위 스캔 (이론상 도달 안 하지만 방어적 처리)
    for (int d = 0; d < 4; ++d) {
        int ny = exitY + dy[d];
        int nx = exitX + dx[d];
        if (ny >= 0 && ny < height && nx >= 0 && nx < width &&
            grid[ny][nx] == 0) {
            return d;
        }
    }

    return enterDir; // fallback
}

void GateManager::draw(int offsetY, int offsetX) const
{
    if (gateA.first >= 0)
    {
        attron(COLOR_PAIR(8));
        mvprintw(offsetY + gateA.first, offsetX + gateA.second, " ");
        if (gateB.first >= 0)
        {
            mvprintw(offsetY + gateB.first, offsetX + gateB.second, " ");
        }
        attroff(COLOR_PAIR(8));
    }
}

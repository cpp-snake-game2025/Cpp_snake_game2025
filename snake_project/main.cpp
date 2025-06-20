#include <ncurses.h> // 콘솔 UI 라이브러리
#include <unistd.h>  // usleep
#include <ctime>     // time
#include <string>
#include <vector>
#include "map.h"        // 게임 맵 클래스
#include "scoreboard.h" // 점수판 클래스
#include "snake.h"      // 뱀 클래스
#include "item.h"       // 아이템 관리 클래스
#include "gate.h"       // 게이트 관리 클래스

// ─── 메뉴 창 출력 함수 ───────────────────────────────────
void drawMenu(WINDOW *menuWin, int highlight, const std::vector<std::string> &choices)
{
    box(menuWin, 0, 0);
    for (int i = 0; i < choices.size(); ++i)
    {
        if (i == highlight)
            wattron(menuWin, A_REVERSE);
        mvwprintw(menuWin, i + 1, 2, choices[i].c_str());
        wattroff(menuWin, A_REVERSE);
    }
    wrefresh(menuWin);
}

// ─── 게임 설명 출력 함수 ─────────────────────────────────
void showGameManual()
{
    timeout(-1);
    clear();
    redrawwin(stdscr);
    refresh();

    mvprintw(1, 2, "Snake Game Manual");
    mvprintw(3, 2, "Goal: Clear all 4 stages.");
    mvprintw(4, 2, "Each stage has a unique mission.");
    mvprintw(5, 2, "Use arrow keys to move the snake.");
    mvprintw(7, 2, "Game Elements:");

    // Growth Item (Green)
    attron(COLOR_PAIR(1));
    mvprintw(9, 4, "   ");
    attroff(COLOR_PAIR(1));
    mvprintw(9, 8, ": Growth Item (+1 length)");

    // Poison Item (Red)
    attron(COLOR_PAIR(2));
    mvprintw(10, 4, "   ");
    attroff(COLOR_PAIR(2));
    mvprintw(10, 8, ": Poison Item (-1 length)");

    // Double Effect Item (Blue)
    attron(COLOR_PAIR(7));
    mvprintw(11, 4, "   ");
    attroff(COLOR_PAIR(7));
    mvprintw(11, 8, ": Growth x2 (8 sec)");

    // Wall (Magenta)
    attron(COLOR_PAIR(6));
    mvprintw(12, 4, "   ");
    attroff(COLOR_PAIR(6));
    mvprintw(12, 8, ": Wall");

    // Gate (Yellow)
    attron(COLOR_PAIR(8));
    mvprintw(13, 4, "   ");
    attroff(COLOR_PAIR(8));
    mvprintw(13, 8, ": Gate (teleport)");

    // Snake Head (White)
    attron(COLOR_PAIR(4));
    mvprintw(14, 4, "   ");
    attroff(COLOR_PAIR(4));
    mvprintw(14, 8, ": Snake Head");

    // Snake Body (Cyan)
    attron(COLOR_PAIR(3));
    mvprintw(15, 4, "   ");
    attroff(COLOR_PAIR(3));
    mvprintw(15, 8, ": Snake Body");

    mvprintw(17, 2, "Press any key to return.");
    refresh();
    getch();
    timeout(0);
}

// ─── 제작자 정보 출력 함수 ─────────────────────────────────
void showDeveloper()
{
    timeout(-1);
    clear();
    refresh();
    mvprintw(1, 2, "Snake Game Developer");
    mvprintw(3, 2, "Kookmin univ. Park Hyemin");
    mvprintw(4, 2, "Kookmin univ. Moon Geunho");
    mvprintw(5, 2, "Kookmin univ. Moon Sebin");
    mvprintw(7, 2, "Press any key to return.");
    refresh();
    getch();
    timeout(0);
}

// ─── 스테이지 알림 함수 ───────────────────────────────────
void showStageStartMessage(int stageNumber)
{
    std::string title = "Stage " + std::to_string(stageNumber) + " Start!";
    std::string subtitle = "Get Ready...";

    int winHeight = 4;
    int winWidth = 30;
    int startY = (LINES - winHeight) / 2.1;
    int startX = (COLS - winWidth) / 5;

    WINDOW *msgWin = newwin(winHeight, winWidth, startY, startX);
    box(msgWin, 0, 0);
    mvwprintw(msgWin, 1, (winWidth - title.length()) / 2, "%s", title.c_str());
    mvwprintw(msgWin, 2, (winWidth - subtitle.length()) / 2, "%s", subtitle.c_str());
    wrefresh(msgWin);

    // 3초 대기
    timeout(-1);        // 블로킹 모드
    napms(3000);        // 3000ms = 3초 대기

    delwin(msgWin);
    clear();
    refresh();
    timeout(0);         // 게임 루프용으로 복구
}

// ─── Stage 1 실행 함수 ─────────────────────────────────
// filename: "map.txt" 등 스테이지 맵 파일명
bool runStage1(const std::string &filename, ScoreBoard &score)
{
    GameMap gameMap(filename);
    if (gameMap.getHeight() <= 0 || gameMap.getWidth() <= 0)
    {
        mvprintw(1, 2, "Failed to load map '%s'.", filename.c_str());
        refresh();
        getch();
        return false;
    }

    Snake snake(5, 5);
    ItemManager itemManager(gameMap.getHeight(), gameMap.getWidth());
    GateManager gateManager;

    bool gateSpawned = false;
    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    int doubleEffectDuration = 0;
    const int delay = 250000; // 250 ms
    time_t startTime = std::time(nullptr);

    while (true)
    {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update(gameMap.getMap(), snake);
        itemManager.draw(1, 2);

        // 10초마다 게이트 생성 업데이트
        int elapsed = static_cast<int>(std::time(nullptr) - startTime);
        static time_t lastGateUpdate = std::time(nullptr);  // 최초 갱신 시간 저장

        if (elapsed >= 10 && std::difftime(std::time(nullptr), lastGateUpdate) >= 10.0)
        {
            gateManager.setGates(gameMap.getMap());
            gateSpawned = true;
            lastGateUpdate = std::time(nullptr);  // 갱신 시간 갱신
        }
        if (gateSpawned)
            gateManager.draw(1, 2);

        // ScoreBoard 업데이트
        score.setLength(snake.getLength(), snake.getMaxLength());
        score.setGrowth(growthCount);
        score.setPoison(poisonCount);
        score.setGate(gateUsed);
        score.setTime(elapsed);
        score.setEffectTime(doubleEffectDuration);

        // 점수판 & 미션 그리기
        score.draw(1, 30);
        score.drawMissions(13, 30);
        refresh();

        // Stage 1 클리어 조건: + ≥ 1, - ≥ 1, G ≥ 1
        if (growthCount >= 1 && poisonCount >= 1 && gateUsed >= 1)
            return true;

        int ch = getch();
        Direction dir = snake.getDirection();

        switch (ch)
        {
        case KEY_UP:
            if (dir == DOWN)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            if (dir == UP)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
            if (dir == RIGHT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(LEFT);
            break;
        case KEY_RIGHT:
            if (dir == LEFT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(RIGHT);
            break;
        case 'q':
            return false;
        default:
            break;
        }

        usleep(delay);
        snake.move();
        auto head = snake.getHead();

        // ─── 수정: 게이트 먼저 검사 ───
        if (gateSpawned && gateManager.isGate(head.first, head.second))
        {
            int enterDir = snake.getDirection();
            auto exitG = gateManager.getExitGate(head.first, head.second);
            int exitDir = gateManager.getExitDirection(enterDir, exitG.first, exitG.second, gameMap.getMap());

            // 방향 먼저 설정
            snake.setDirection(static_cast<Direction>(exitDir));
            snake.teleport(exitG.first, exitG.second);

            gateUsed++;
            score.increaseGate();
            continue;
        }
        // ──────────────────────────────

        // ─── 수정: 벽 충돌 검사 ───
        const auto &grid = gameMap.getMap();
        if (grid[head.first][head.second] == 1)
        {
            // 벽 충돌 → Game Over
            attron(COLOR_PAIR(9));
            mvprintw(23, 2, "Game Over: The snake hit a wall!");
            attroff(COLOR_PAIR(9));
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ────────────────────────────

        // 아이템 처리
        ItemType t = itemManager.checkItem(head.first, head.second);
        if (t == DOUBLE_EFFECT)
        {
            doubleEffectDuration = 40;
            itemManager.removeItemAt(head.first, head.second);
        }
        else if (t == GROWTH)
        {
            int inc = (doubleEffectDuration > 0) ? 2 : 1;
            growthCount += inc;
            for (int i = 0; i < inc; ++i)
                snake.growAtTail();
            itemManager.removeItemAt(head.first, head.second);
            score.increaseGrowth();
            if (inc == 2)
                score.increaseGrowth();
        }
        else if (t == POISON)
        {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink())
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake is poisoned!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            score.increasePoison();
        }

        if (doubleEffectDuration > 0)
            doubleEffectDuration--;

        // ─── 기존 벽/자기 충돌 검사 (자기 몸 충돌 포함) ───
        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth()))
        {
            bool hitBoundary = (head.first <= 0 || head.first >= gameMap.getHeight() - 1 ||
                                head.second <= 0 || head.second >= gameMap.getWidth() - 1);
            if (hitBoundary)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit the boundary!");
                attroff(COLOR_PAIR(9));
            }
            else
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
            }
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ──────────────────────────────────────────────
    }
}

// ─── Stage 2 실행 함수 ─────────────────────────────────
// filename: "map_stage2.txt" 등 스테이지2 맵 파일명
bool runStage2(const std::string &filename, ScoreBoard &score)
{
    GameMap gameMap(filename);
    if (gameMap.getHeight() <= 0 || gameMap.getWidth() <= 0)
    {
        mvprintw(1, 2, "Failed to load map '%s'.", filename.c_str());
        refresh();
        getch();
        return false;
    }

    Snake snake(5, 5);
    ItemManager itemManager(gameMap.getHeight(), gameMap.getWidth());
    GateManager gateManager;
    bool gateSpawned = false;
    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    int doubleEffectDuration = 0;
    const int delay = 250000; // 250 ms
    time_t startTime = std::time(nullptr);

    while (true)
    {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update(gameMap.getMap(), snake);
        itemManager.draw(1, 2);

        // 10초마다 게이트 생성 업데이트
        int elapsed = static_cast<int>(std::time(nullptr) - startTime);
        static time_t lastGateUpdate = std::time(nullptr);  // 최초 갱신 시간 저장

        if (elapsed >= 10 && std::difftime(std::time(nullptr), lastGateUpdate) >= 10.0)
        {
            gateManager.setGates(gameMap.getMap());
            gateSpawned = true;
            lastGateUpdate = std::time(nullptr);  // 갱신 시간 갱신
        }
        if (gateSpawned)
            gateManager.draw(1, 2);

        // ScoreBoard 업데이트
        score.setLength(snake.getLength(), snake.getMaxLength());
        score.setGrowth(growthCount);
        score.setPoison(poisonCount);
        score.setGate(gateUsed);
        score.setTime(elapsed);
        score.setEffectTime(doubleEffectDuration);

        // 점수판 & 미션 그리기 (B 포함)
        score.draw(1, 30);
        score.drawMissions(13, 30);
        refresh();

        // Stage 2 클리어 조건: + ≥ 1, - ≥ 1, G ≥ 1, 뱀 길이 ≥ 5
        if (growthCount >= 1 &&
            poisonCount >= 1 &&
            gateUsed >= 1 &&
            snake.getLength() >= 5)
        {
            return true;
        }

        int ch = getch();
        Direction dir = snake.getDirection();

        switch (ch)
        {
        case KEY_UP:
            if (dir == DOWN)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            if (dir == UP)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
            if (dir == RIGHT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(LEFT);
            break;
        case KEY_RIGHT:
            if (dir == LEFT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(RIGHT);
            break;
        case 'q':
            return false;
        default:
            break;
        }

        usleep(delay);
        snake.move();
        auto head = snake.getHead();

        // ─── 수정: 게이트 먼저 검사 ───
        if (gateSpawned && gateManager.isGate(head.first, head.second))
        {
            int enterDir = snake.getDirection();
            auto exitG = gateManager.getExitGate(head.first, head.second);
            int exitDir = gateManager.getExitDirection(enterDir, exitG.first, exitG.second, gameMap.getMap());

            // 방향 먼저 설정
            snake.setDirection(static_cast<Direction>(exitDir));
            snake.teleport(exitG.first, exitG.second);

            gateUsed++;
            score.increaseGate();
            continue;
        }
        // ──────────────────────────────

        // ─── 수정: 벽 충돌 검사 ───
        const auto &grid2 = gameMap.getMap();
        if (grid2[head.first][head.second] == 1)
        {
            // 벽 충돌 → Game Over
            attron(COLOR_PAIR(9));
            mvprintw(23, 2, "Game Over: The snake hit a wall!");
            attroff(COLOR_PAIR(9));
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ────────────────────────────

        // 아이템 처리
        ItemType t = itemManager.checkItem(head.first, head.second);
        if (t == DOUBLE_EFFECT)
        {
            doubleEffectDuration = 40;
            itemManager.removeItemAt(head.first, head.second);
        }
        else if (t == GROWTH)
        {
            int inc = (doubleEffectDuration > 0) ? 2 : 1;
            growthCount += inc;
            for (int i = 0; i < inc; ++i)
                snake.growAtTail();
            itemManager.removeItemAt(head.first, head.second);
            score.increaseGrowth();
            if (inc == 2)
                score.increaseGrowth();
        }
        else if (t == POISON)
        {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink())
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake is poisoned!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            score.increasePoison();
        }

        if (doubleEffectDuration > 0)
            doubleEffectDuration--;

        // ─── 기존 벽/자기 충돌 검사 (자기 몸 충돌 포함) ───
        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth()))
        {
            bool hitBoundary = (head.first <= 0 || head.first >= gameMap.getHeight() - 1 ||
                                head.second <= 0 || head.second >= gameMap.getWidth() - 1);
            if (hitBoundary)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit the boundary!");
                attroff(COLOR_PAIR(9));
            }
            else
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
            }
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ──────────────────────────────────────────────
    }
}

// ─── Stage 3 실행 함수 ─────────────────────────────────
// filename: "map_stage3.txt" 등 스테이지3 맵 파일명
bool runStage3(const std::string &filename, ScoreBoard &score)
{
    GameMap gameMap(filename);
    if (gameMap.getHeight() <= 0 || gameMap.getWidth() <= 0)
    {
        mvprintw(1, 2, "Failed to load map '%s'.", filename.c_str());
        refresh();
        getch();
        return false;
    }

    Snake snake(5, 5);
    ItemManager itemManager(gameMap.getHeight(), gameMap.getWidth());
    GateManager gateManager;
    bool gateSpawned = false;
    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    int doubleEffectDuration = 0;
    const int delay = 250000; // 250 ms
    time_t startTime = std::time(nullptr);

    while (true)
    {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update(gameMap.getMap(), snake);
        itemManager.draw(1, 2);

        // 10초마다 게이트 생성 업데이트
        int elapsed = static_cast<int>(std::time(nullptr) - startTime);
        static time_t lastGateUpdate = std::time(nullptr);  // 최초 갱신 시간 저장

        if (elapsed >= 10 && std::difftime(std::time(nullptr), lastGateUpdate) >= 10.0)
        {
            gateManager.setGates(gameMap.getMap());
            gateSpawned = true;
            lastGateUpdate = std::time(nullptr);  // 갱신 시간 갱신
        }
        if (gateSpawned)
            gateManager.draw(1, 2);

        // ScoreBoard 업데이트
        score.setLength(snake.getLength(), snake.getMaxLength());
        score.setGrowth(growthCount);
        score.setPoison(poisonCount);
        score.setGate(gateUsed);
        score.setTime(elapsed);
        score.setEffectTime(doubleEffectDuration);

        // 점수판 & 미션 그리기 (B 포함)
        score.draw(1, 30);
        score.drawMissions(13, 30);
        refresh();

        // Stage 3 클리어 조건: + ≥ 3, - ≥ 3, G ≥ 3, 뱀 길이 ≥ 7
        if (growthCount >= 3 &&
            poisonCount >= 3 &&
            gateUsed >= 3 &&
            snake.getLength() >= 7)
        {
            return true;
        }

        int ch = getch();
        Direction dir = snake.getDirection();

        switch (ch)
        {
        case KEY_UP:
            if (dir == DOWN)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            if (dir == UP)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
            if (dir == RIGHT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(LEFT);
            break;
        case KEY_RIGHT:
            if (dir == LEFT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(RIGHT);
            break;
        case 'q':
            return false;
        default:
            break;
        }

        usleep(delay);
        snake.move();
        auto head = snake.getHead();

        // ─── 수정: 게이트 먼저 검사 ───
        if (gateSpawned && gateManager.isGate(head.first, head.second))
        {
            int enterDir = snake.getDirection();
            auto exitG = gateManager.getExitGate(head.first, head.second);
            int exitDir = gateManager.getExitDirection(enterDir, exitG.first, exitG.second, gameMap.getMap());

            // 방향 먼저 설정
            snake.setDirection(static_cast<Direction>(exitDir));
            snake.teleport(exitG.first, exitG.second);

            gateUsed++;
            score.increaseGate();
            continue;
        }
        // ──────────────────────────────

        // ─── 수정: 벽 충돌 검사 ───
        const auto &grid2 = gameMap.getMap();
        if (grid2[head.first][head.second] == 1)
        {
            // 벽 충돌 → Game Over
            attron(COLOR_PAIR(9));
            mvprintw(23, 2, "Game Over: The snake hit a wall!");
            attroff(COLOR_PAIR(9));
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ────────────────────────────

        // 아이템 처리
        ItemType t = itemManager.checkItem(head.first, head.second);
        if (t == DOUBLE_EFFECT)
        {
            doubleEffectDuration = 40;
            itemManager.removeItemAt(head.first, head.second);
        }
        else if (t == GROWTH)
        {
            int inc = (doubleEffectDuration > 0) ? 2 : 1;
            growthCount += inc;
            for (int i = 0; i < inc; ++i)
                snake.growAtTail();
            itemManager.removeItemAt(head.first, head.second);
            score.increaseGrowth();
            if (inc == 2)
                score.increaseGrowth();
        }
        else if (t == POISON)
        {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink())
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake is poisoned!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            score.increasePoison();
        }

        if (doubleEffectDuration > 0)
            doubleEffectDuration--;

        // ─── 기존 벽/자기 충돌 검사 (자기 몸 충돌 포함) ───
        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth()))
        {
            bool hitBoundary = (head.first <= 0 || head.first >= gameMap.getHeight() - 1 ||
                                head.second <= 0 || head.second >= gameMap.getWidth() - 1);
            if (hitBoundary)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit the boundary!");
                attroff(COLOR_PAIR(9));
            }
            else
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
            }
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ──────────────────────────────────────────────
    }
}

// ─── Stage 4 실행 함수 ─────────────────────────────────
// filename: "map_stage4.txt" 등 스테이지4 맵 파일명
bool runStage4(const std::string &filename, ScoreBoard &score)
{
    GameMap gameMap(filename);
    if (gameMap.getHeight() <= 0 || gameMap.getWidth() <= 0)
    {
        mvprintw(1, 2, "Failed to load map '%s'.", filename.c_str());
        refresh();
        getch();
        return false;
    }

    Snake snake(2, 4);
    ItemManager itemManager(gameMap.getHeight(), gameMap.getWidth());
    GateManager gateManager;
    bool gateSpawned = false;
    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    int doubleEffectDuration = 0;
    const int delay = 250000; // 250 ms
    time_t startTime = std::time(nullptr);

    while (true)
    {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update(gameMap.getMap(), snake);
        itemManager.draw(1, 2);

        // 10초마다 게이트 생성 업데이트
        int elapsed = static_cast<int>(std::time(nullptr) - startTime);
        static time_t lastGateUpdate = std::time(nullptr);  // 최초 갱신 시간 저장

        if (elapsed >= 10 && std::difftime(std::time(nullptr), lastGateUpdate) >= 10.0)
        {
            gateManager.setGates(gameMap.getMap());
            gateSpawned = true;
            lastGateUpdate = std::time(nullptr);  // 갱신 시간 갱신
        }
        if (gateSpawned)
            gateManager.draw(1, 2);

        // ScoreBoard 업데이트
        score.setLength(snake.getLength(), snake.getMaxLength());
        score.setGrowth(growthCount);
        score.setPoison(poisonCount);
        score.setGate(gateUsed);
        score.setTime(elapsed);
        score.setEffectTime(doubleEffectDuration);

        // 점수판 & 미션 그리기 (B 포함)
        score.draw(1, 30);
        score.drawMissions(13, 30);
        refresh();

        // Stage 4 클리어 조건: + ≥ 5, - ≥ 5, G ≥ 3, 뱀 길이 ≥ 9
        if (growthCount >= 5 &&
            poisonCount >= 5 &&
            gateUsed >= 3 &&
            snake.getLength() >= 9)
        {
            return true;
        }

        int ch = getch();
        Direction dir = snake.getDirection();

        switch (ch)
        {
        case KEY_UP:
            if (dir == DOWN)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            if (dir == UP)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
            if (dir == RIGHT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(LEFT);
            break;
        case KEY_RIGHT:
            if (dir == LEFT)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            snake.setDirection(RIGHT);
            break;
        case 'q':
            return false;
        default:
            break;
        }

        usleep(delay);
        snake.move();
        auto head = snake.getHead();

        // ─── 수정: 게이트 먼저 검사 ───
        if (gateSpawned && gateManager.isGate(head.first, head.second))
        {
            int enterDir = snake.getDirection();
            auto exitG = gateManager.getExitGate(head.first, head.second);
            int exitDir = gateManager.getExitDirection(enterDir, exitG.first, exitG.second, gameMap.getMap());

            // 방향 먼저 설정
            snake.setDirection(static_cast<Direction>(exitDir));
            snake.teleport(exitG.first, exitG.second);

            gateUsed++;
            score.increaseGate();
            continue;
        }
        // ──────────────────────────────

        // ─── 수정: 벽 충돌 검사 ───
        const auto &grid2 = gameMap.getMap();
        if (grid2[head.first][head.second] == 1)
        {
            // 벽 충돌 → Game Over
            attron(COLOR_PAIR(9));
            mvprintw(23, 2, "Game Over: The snake hit a wall!");
            attroff(COLOR_PAIR(9));
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ────────────────────────────

        // 아이템 처리
        ItemType t = itemManager.checkItem(head.first, head.second);
        if (t == DOUBLE_EFFECT)
        {
            doubleEffectDuration = 40;
            itemManager.removeItemAt(head.first, head.second);
        }
        else if (t == GROWTH)
        {
            int inc = (doubleEffectDuration > 0) ? 2 : 1;
            growthCount += inc;
            for (int i = 0; i < inc; ++i)
                snake.growAtTail();
            itemManager.removeItemAt(head.first, head.second);
            score.increaseGrowth();
            if (inc == 2)
                score.increaseGrowth();
        }
        else if (t == POISON)
        {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink())
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake is poisoned!");
                attroff(COLOR_PAIR(9));
                refresh();
                return false;
            }
            score.increasePoison();
        }

        if (doubleEffectDuration > 0)
            doubleEffectDuration--;

        // ─── 기존 벽/자기 충돌 검사 (자기 몸 충돌 포함) ───
        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth()))
        {
            bool hitBoundary = (head.first <= 0 || head.first >= gameMap.getHeight() - 1 ||
                                head.second <= 0 || head.second >= gameMap.getWidth() - 1);
            if (hitBoundary)
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit the boundary!");
                attroff(COLOR_PAIR(9));
            }
            else
            {
                attron(COLOR_PAIR(9));
                mvprintw(23, 2, "Game Over: The snake hit its own body!");
                attroff(COLOR_PAIR(9));
            }
            refresh();
            timeout(-1);
            getch();
            timeout(0);
            return false;
        }
        // ──────────────────────────────────────────────
    }
}

// ─── 프로그램 시작 ───────────────────────────────────────
int main()
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);
    start_color();
    init_pair(1, 46, 46);                       // Growth Item (초록)
    init_pair(2, 196, 196);                     // Poison Item (빨강)
    init_pair(3, COLOR_CYAN, COLOR_CYAN);       // 뱀 몸통 (청록)
    init_pair(4, COLOR_WHITE, COLOR_WHITE);     // 뱀 머리 (흰색)
    init_pair(5, 208, 208);                     // Immune Wall (주황)
    init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA); // Wall (보라)
    init_pair(7, 45, 45);                       // Double Effect (파랑)
    init_pair(8, 226, 226);                     // Gate (노랑)
    init_pair(9, 196, COLOR_BLACK);             // Game Over 메시지 (빨강 글자)

    std::vector<std::string> menuItems = {"Game Start", "Game Manual", "Developer", "Exit"};
    int highlight = 0, choice = -1, ch;

    int height = static_cast<int>(menuItems.size()) + 2;
    int width = 30;
    int starty = static_cast<int>((LINES - height) / 2.1);
    int startx = static_cast<int>((COLS - width) / 5);

    WINDOW *menuWin = newwin(height, width, starty, startx);

    refresh();
    drawMenu(menuWin, highlight, menuItems);

    bool inMenu = true;

    while (true)
    {
        if (inMenu)
        {
            drawMenu(menuWin, highlight, menuItems);
        }

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (inMenu)
                highlight = (highlight == 0) ? static_cast<int>(menuItems.size()) - 1 : highlight - 1;
            break;
        case KEY_DOWN:
            if (inMenu)
                highlight = (highlight + 1) % static_cast<int>(menuItems.size());
            break;
        case '\n':
            choice = highlight;
            werase(menuWin);
            wrefresh(menuWin);
            delwin(menuWin);
            clear();
            refresh();
            redrawwin(stdscr);
            refresh();
            inMenu = false;

            if (choice == 0) // Game Start
            {
                ScoreBoard score;
                // ─── Stage 1 목표 설정 ───
                score.setGoals(1, 1, 1, 0);
                score.resetCurrentMissions();
                bool cleared1 = runStage1("map.txt", score);

                if (cleared1)
                {
                    showStageStartMessage(2);
                    // ─── Stage 2 목표 재설정 ───
                    score.setGoals(1, 1, 1, 5);
                    score.resetCurrentMissions();
                    bool cleared2 = runStage2("map_stage2.txt", score);

                    if (cleared2)
                    {
                        showStageStartMessage(3);
                        // ─── Stage 3 목표 재설정 ───
                        score.setGoals(3, 3, 3, 7);
                        score.resetCurrentMissions();
                        bool cleared3 = runStage3("map_stage3.txt", score);
                    
                        if (cleared3)
                        {
                            showStageStartMessage(4);
                            // ─── Stage 4 목표 재설정 ───
                            score.setGoals(5, 5, 3, 9);
                            score.resetCurrentMissions();
                            bool cleared4 = runStage4("map_stage4.txt", score);

                            if (cleared4)
                            {
                                // 모든 스테이지 클리어
                                int winHeight = 6;
                                int winWidth = 41;
                                int startY = (LINES - winHeight) / 2.1;
                                int startX = (COLS - winWidth) / 5;

                                WINDOW *msgWin = newwin(winHeight, winWidth, startY, startX);
                                box(msgWin, 0, 0);
                                mvwprintw(msgWin, 1, 8, "Congratulations!٩('o')٩");
                                mvwprintw(msgWin, 2, 6, "You completed all the stages!");
                                mvwprintw(msgWin, 4, 7, "Press any key to continue.");
                                wrefresh(msgWin);

                                timeout(-1);     // 블로킹 모드로 축하 메시지 대기
                                getch();         // 사용자 입력 대기

                                delwin(msgWin);  // 박스 제거
                                clear();         // 전체 화면 정리
                                refresh();       // 적용

                                timeout(0);      // 비블로킹 모드로 복귀

                                // 메뉴 다시 그리기
                                inMenu = true;  // 다시 메뉴 모드로 전환
                                menuWin = newwin(height, width, starty, startx);
                                highlight = 0;
                                drawMenu(menuWin, highlight, menuItems);
                            }
                        }
                    }
                }
            }
            else if (choice == 1) // Game Manual
            {
                showGameManual();
            }
            else if (choice == 2) // Developer Info
            {
                showDeveloper();
            }
            else if (choice == 3) // Exit
            {
                endwin();
                return 0;
            }

            // 메뉴로 돌아가기
            inMenu = true;
            menuWin = newwin(height, width, starty, startx);
            highlight = 0;
            drawMenu(menuWin, highlight, menuItems);
            break;
        default:
            break;
        }
    }

    endwin();
    return 0;
}

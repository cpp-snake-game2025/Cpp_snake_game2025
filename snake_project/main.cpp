// main.cpp
#include <ncurses.h>
#include <unistd.h>
#include <ctime>
#include "map.h"
#include "scoreboard.h"
#include "snake.h"
#include "item.h"

int main()
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);

    GameMap gameMap;
    ScoreBoard score;
    Snake snake(5, 5);
    ItemManager itemManager(21, 21);

    // Gate 좌표 자동 추출
    auto m = gameMap.getMap();
    std::pair<int, int> gateA{-1, -1}, gateB{-1, -1};
    for (int y = 0; y < gameMap.getHeight(); ++y)
        for (int x = 0; x < gameMap.getWidth(); ++x)
            if (m[y][x] == 7)
            {
                if (gateA.first < 0)
                    gateA = {y, x};
                else
                    gateB = {y, x};
            }

    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    const int delay = 250000;
    time_t start = time(nullptr);
    bool run = true;

    while (run)
    {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update();
        if (itemManager.getItemCount() < 3)
            itemManager.placeItems(gameMap.getMap());
        itemManager.draw(1, 2);

        int elapsed = (int)(time(nullptr) - start);
        score.setLength(snake.getLength(), snake.getMaxLength());
        score.setGrowth(growthCount);
        score.setPoison(poisonCount);
        score.setGate(gateUsed);
        score.setTime(elapsed);
        score.draw(1, 30);
        refresh();

        int ch = getch();
        switch (ch)
        {
        case KEY_UP:
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
            snake.setDirection(LEFT);
            break;
        case KEY_RIGHT:
            snake.setDirection(RIGHT);
            break;
        case 'q':
            run = false;
            continue;
        }

        usleep(delay);
        snake.move();

        auto head = snake.getHead();
        // Gate 통과 처리
        if (head == gateA || head == gateB)
        {
            auto exitG = (head == gateA ? gateB : gateA);
            snake.teleport(exitG.first, exitG.second);
            gateUsed++;
            continue;
        }

        // 아이템 처리
        ItemType it = itemManager.checkItem(head.first, head.second);
        if (it == GROWTH)
        {
            growthCount++;
            itemManager.removeItemAt(head.first, head.second);
            snake.move(true);
        }
        else if (it == POISON)
        {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink())
            {
                mvprintw(23, 2, "Game Over!");
                refresh();
                usleep(1000000);
                break;
            }
        }

        // 벽/자기 충돌 시 종료
        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth()))
        {
            mvprintw(23, 2, "Game Over!");
            refresh();
            usleep(1000000);
            break;
        }
    }

    endwin();
    return 0;
}

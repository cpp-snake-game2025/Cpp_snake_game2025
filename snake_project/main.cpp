// main.cpp
#include <ncurses.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <vector>
#include "map.h"
#include "scoreboard.h"
#include "snake.h"
#include "item.h"

// 메뉴 창 출력
void drawMenu(WINDOW* menuWin, int highlight, const std::vector<std::string>& choices) {
    box(menuWin, 0, 0);
    for (int i = 0; i < choices.size(); ++i){
        if (i == highlight)
            wattron(menuWin, A_REVERSE);
        mvwprintw(menuWin, i + 1, 2, choices[i].c_str());
        wattroff(menuWin, A_REVERSE);
    }
    wrefresh(menuWin);
}

// 게임 설명 출력
void showGameManual() {
    clear();
    mvprintw(1, 2, "Snake Game Manual");

    mvprintw(3, 2, "Goal: Clear all 4 stages.");
    mvprintw(4, 2, "Each stage has a unique mission.");
    mvprintw(5, 2, "Use arrow keys to move the snake.");

    mvprintw(7, 2, "Game Elements:");

    // Wall (Magenta)
    attron(COLOR_PAIR(6));
    mvprintw(9, 4, "   ");
    attroff(COLOR_PAIR(6));
    mvprintw(9, 8, ": Wall");

    // Growth Item (Green)
    attron(COLOR_PAIR(1));
    mvprintw(10, 4, "   ");
    attroff(COLOR_PAIR(1));
    mvprintw(10, 8, ": Growth Item (+1 length)");

    // Poison Item (Red)
    attron(COLOR_PAIR(2));
    mvprintw(11, 4, "   ");
    attroff(COLOR_PAIR(2));
    mvprintw(11, 8, ": Poison Item (-1 length)");

    // Gate (Yellow)
    attron(COLOR_PAIR(5));
    mvprintw(12, 4, "   ");
    attroff(COLOR_PAIR(5));
    mvprintw(12, 8, ": Gate (teleport)");

    // Snake Head (White)
    attron(COLOR_PAIR(4));
    mvprintw(13, 4, "   ");
    attroff(COLOR_PAIR(4));
    mvprintw(13, 8, ": Snake Head");

    // Snake Body (Cyan)
    attron(COLOR_PAIR(3));
    mvprintw(14, 4, "   ");
    attroff(COLOR_PAIR(3));
    mvprintw(14, 8, ": Snake Body");

    mvprintw(16, 2, "Press any key to return.");
    refresh();
    getch();
}

// 제작자 정보 출력
void showDeveloper() {
    clear();
    mvprintw(1, 2, "Snake Game Developer");
    mvprintw(3, 2, "Kookmin univ. Park Hyemin");
    mvprintw(4, 2, "Kookmin univ. Moon Geunho");
    mvprintw(5, 2, "Kookmin univ. Moon Sebin");
    mvprintw(7, 2, "Press any key to return.");
    refresh();
    getch();
}

// 실제 게임 실행
void runGame() {
    timeout(0);
    GameMap gameMap;
    ScoreBoard score;
    Snake snake(5, 5);
    ItemManager itemManager(21, 21);

    auto m = gameMap.getMap();
    std::pair<int, int> gateA{-1, -1}, gateB{-1, -1};
    for (int y = 0; y < gameMap.getHeight(); ++y)
        for (int x = 0; x < gameMap.getWidth(); ++x)
            if (m[y][x] == 7)
                (gateA.first < 0) ? gateA = std::make_pair(y, x) : gateB = std::make_pair(y, x);

    int growthCount = 0, poisonCount = 0, gateUsed = 0;
    const int delay = 250000;
    time_t start = time(nullptr);

    while (true) {
        clear();
        gameMap.draw(1, 2);
        snake.draw(1, 2);
        itemManager.update(gameMap.getMap(), snake);
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
        switch (ch){
        case KEY_UP:    snake.setDirection(UP); break;
        case KEY_DOWN:  snake.setDirection(DOWN); break;
        case KEY_LEFT:  snake.setDirection(LEFT); break;
        case KEY_RIGHT: snake.setDirection(RIGHT); break;
        case 'q':       return; // quit directly
        }

        usleep(delay);
        snake.move();

        auto head = snake.getHead();
        if (head == gateA || head == gateB) {
            auto exitG = (head == gateA ? gateB : gateA);
            snake.teleport(exitG.first, exitG.second);
            gateUsed++;
            continue;
        }

        ItemType it = itemManager.checkItem(head.first, head.second);
        if (it == GROWTH) {
            growthCount++;
            itemManager.removeItemAt(head.first, head.second);
            snake.move(true);
        }
        else if (it == POISON) {
            poisonCount++;
            itemManager.removeItemAt(head.first, head.second);
            if (!snake.shrink()){
                mvprintw(23, 2, "Game Over: The snake is poisoned!");
                refresh();
                getch();
                break;
            }
        }

        if (snake.checkCollision(gameMap.getHeight(), gameMap.getWidth())) {
            bool hitWall = (head.first <= 0 || head.first >= gameMap.getHeight() - 1 ||
                            head.second <= 0 || head.second >= gameMap.getWidth() - 1);
            if (hitWall)
                mvprintw(23, 2, "Game Over: The snake hit the wall!");
            else
                mvprintw(23, 2, "Game Over: The snake ate its body!");
                refresh();
                getch();
                break;
            return;
        }
    }
    return;
}

// 프로그램 시작 (메뉴 먼저)
int main() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, 46, 46);                       // 성장 아이템
    init_pair(2, 196, 196);                     // 독 아이템
    init_pair(3, COLOR_CYAN, COLOR_CYAN);       // Snake 몸통
    init_pair(4, COLOR_WHITE, COLOR_WHITE);     // Snake 머리
    init_pair(5, 226, 226);                     // Gate
    init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA); // Wall

    std::vector<std::string> menuItems = {"Game Start", "Game Manual", "Developer", "Exit"};
    int highlight = 0;
    int choice = -1;
    int ch;

    int height = menuItems.size() + 2; // 메뉴 항목 수 + 위아래 여백
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW* menuWin = newwin(height, width, starty, startx);

    refresh();
    drawMenu(menuWin, highlight, menuItems);

    while (true) {
        drawMenu(menuWin, highlight, menuItems);
        ch = getch();

        switch (ch) {
        case KEY_UP:
            highlight = (highlight == 0) ? menuItems.size() - 1 : highlight - 1;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % menuItems.size();
            break;
        case '\n':
            choice = highlight;
            werase(menuWin); wrefresh(menuWin); delwin(menuWin);
            clear(); refresh();

            if (choice == 0) {
                timeout(0);
                runGame();
            } else if (choice == 1) {
                showGameManual();
            } else if (choice == 2) {
                showDeveloper();
            } else if (choice == 3) {
                endwin();
                return 0;
            }

            // 메뉴 다시 생성 및 출력
            menuWin = newwin(height, width, starty, startx);
            highlight = 0;
            drawMenu(menuWin, highlight, menuItems);
            continue;  // while 루프 다시 실행
        }
    }

    endwin();
    return 0;
}

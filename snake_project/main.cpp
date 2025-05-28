// main.cpp
#include <ncurses.h> // 콘솔 UI 라이브러리 - 키보드 입력, 화면 그리기, 텍스트 UI 제어 기능
#include <unistd.h> // 마이크로초 단위 딜레이
#include <ctime> // 시간 관련 함수
#include "map.h" // 게임 맵 클래스
#include "scoreboard.h" // 점수판 클래스
#include "snake.h" // 뱀 클래스 
#include "item.h" // 아이템 관리 클래스
#include "gate.h" // 게이트 관리 클래스

int main()
{
    initscr(); // ncurses 화면 초기화
    noecho(); // 입력된 키를 화면에 출력하지 않음
    curs_set(0); // 커서를 보이지 않게 설정 
    keypad(stdscr, TRUE); // 키패드 입력 활성화 (화살표 등)
    timeout(0); // getch()가 블로킹되지 않도록 설정 (0초 대기) -> 시작과 함께 바로 실행될 수 있도록
    start_color(); // 색상 기능 활성화
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // 뱀 색상 페어 설정
    init_pair(2, COLOR_RED, COLOR_BLACK); // 게이트 색상 페어 설정
    init_pair(3, COLOR_GREEN, COLOR_BLACK); // 성장 아이템 색상 페어 설정
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK); // 독 아이템 색상 페어 설정
    init_pair(5, COLOR_CYAN, COLOR_BLACK); // DOUBLE_EFFECT 아이템 색상 페어 설정
    init_pair(6, COLOR_RED, COLOR_BLACK); // 게이트 색상 페어 설정

    GameMap gameMap; // 게임 맵 객체 생성 : 게임 화면의 맵(벽, 길 등)을 관리
    ScoreBoard score; // 점수판 객체 생성
    Snake snake(5, 5); // 뱀 객체 생성 : 초기 위치 (5,5)에서 시작
    ItemManager itemManager(21, 21); // 아이템 생성 및 관리 (21x21 크기의 맵에서 아이템 관리)
    GateManager gateManager(gameMap.getHeight(), gameMap.getWidth()); // 게이트 매니저 생성

    // Gate 좌표 자동 추출
    auto m = gameMap.getMap(); //2차원 벡터 형태의 맵 데이터 불러오기
    std::pair<int, int> gateA{-1, -1}, gateB{-1, -1}; // 게이트 A와 B의 좌표를 저장할 변수
    for (int y = 0; y < gameMap.getHeight(); ++y)
        for (int x = 0; x < gameMap.getWidth(); ++x) // 맵의 모든 좌표를 순회
            if (m[y][x] == 7) // 7은 게이트를 나타내는 값
            {
                if (gateA.first < 0)
                    gateA = {y, x};
                else
                    gateB = {y, x};
            }

    int growthCount = 0, poisonCount = 0, gateUsed = 0; // 성장 아이템, 독 아이템, 게이트 사용 횟수를 저장할 변수들
    const int delay = 250000; // 뱀 이동 딜레이 (마이크로초 단위, 250ms)
    time_t start = time(nullptr); // 게임 시작 시간 저장
    bool run = true; // 게임 루프를 제어할 변수
    bool gateSpawned = false; // 게이트가 이미 생성되었는지 여부를 나타내는 변수
    int doubleEffectDuration = 0; // 남은 효과 지속 시간(프레임 또는 초)

    while (run) // 게임 루프 시작
    {
        clear(); // 화면을 지우고 새로 그리기
        gameMap.draw(1, 2); // 맵을 (1, 2) 위치에 그리기, 좌측 여백 2칸, 상단 여백 1칸
        snake.draw(1, 2); // 뱀을 (1, 2) 위치에 그리기
        itemManager.update(); // 아이템 상태 업데이트
        if (itemManager.getItemCount() < 3)
            itemManager.placeItems(gameMap.getMap()); // 아이템이 3개 미만이면 새 아이템 배치
        itemManager.draw(1, 2); // 아이템을 (1, 2) 위치에 그리기
        
        int elapsed = (int)(time(nullptr) - start);

        // 10초 후 게이트 생성
        if (!gateSpawned && elapsed >= 10) {
            gateManager.setGates(gameMap.getMap());
            gateSpawned = true;
        }
        // 게이트가 생성된 경우에만 그리기
        if (gateSpawned) {
            gateManager.draw(1, 2);
        }

        score.setLength(snake.getLength(), snake.getMaxLength()); // 현재 뱀 길이와 최대 길이를 점수판에 설정
        score.setGrowth(growthCount); // 성장 아이템 횟수 설정
        score.setPoison(poisonCount); // 독 아이템 횟수 설정
        score.setGate(gateUsed); // 게이트 사용 횟수 설정
        score.setTime(elapsed); // 경과 시간을 점수판에 설정
        score.setEffectTime(doubleEffectDuration);
        score.draw(1, 30); // 점수판을 (1, 30) 위치에 그리기
        refresh(); // 화면 업데이트

        int ch = getch(); // 키 입력 대기 (비블로킹 모드), 입력이 없으면 -1 반환, 입력이 있으면 해당 키 코드 반환
        Direction currentDir = snake.getDirection();
        
        switch (ch)
        {
        case KEY_UP:
            if (currentDir == DOWN) {
                run = false; // 반대 방향 입력 시 종료
                break;
            }
            snake.setDirection(UP);
            break;
        case KEY_DOWN:
            if (currentDir == UP) {
                run = false;
                break;
            }
            snake.setDirection(DOWN);
            break;
        case KEY_LEFT:
                if (currentDir == RIGHT) {
                    run = false;
                    break;
                }
                snake.setDirection(LEFT);
                break;
        case KEY_RIGHT:
                if (currentDir == LEFT) {
                    run = false;
                    break;
                }
                snake.setDirection(RIGHT);
                break;
        case 'q':
            run = false;
            break;
        }

        if(!run)
        {
            mvprintw(23, 2, "Game Over!");
            refresh();
            usleep(1000000);
            break;
        }

        usleep(delay);
        snake.move();

        auto head = snake.getHead();

        // Gate 통과 처리
        if (gateSpawned && gateManager.isGate(head.first, head.second))
        {
            // 1. 현재 방향(진입 방향) 저장
            int enterDir = snake.getDirection();

            // 2. 출구 게이트 좌표 구하기
            auto exitG = gateManager.getExitGate(head.first, head.second);

            // 3. 출구에서 나올 방향 결정
            int exitDir = gateManager.getExitDirection(enterDir, exitG.first, exitG.second, gameMap.getMap());

            // 4. 뱀을 출구로 순간이동
            snake.teleport(exitG.first, exitG.second);

            // 5. 뱀의 방향을 출구 방향으로 바꿔줌
            snake.setDirection(static_cast<Direction>(exitDir));

            gateUsed++;
            continue;
        }

        // 아이템 처리
        ItemType it = itemManager.checkItem(head.first, head.second);
        
        if (it == DOUBLE_EFFECT)
        {
            doubleEffectDuration = 45; // 효과 지속 시간 설정 (10프레임)
            itemManager.removeItemAt(head.first, head.second);
        }
        else if (it == GROWTH)
        {
            growthCount += (doubleEffectDuration > 0) ? 2 : 1; // 효과가 두 배인 경우 성장 횟수 증가
            itemManager.removeItemAt(head.first, head.second);
            snake.growAtTail();
            if (doubleEffectDuration > 0) {
                snake.growAtTail();  // 두 배 효과면 한 번 더 추가
            }
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

        if(doubleEffectDuration > 0){
            doubleEffectDuration--;
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

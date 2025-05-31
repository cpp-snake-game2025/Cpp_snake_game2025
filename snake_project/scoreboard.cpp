#include "scoreboard.h"
#include <ncurses.h>

// 기존 draw() 함수는 그대로 두고, drawMissions()만 수정되었습니다.

void ScoreBoard::setLength(int cur, int max)
{
    curLen = cur;
    maxLen = max;
}

void ScoreBoard::setGrowth(int count)
{
    growth = count;
}

void ScoreBoard::setPoison(int count)
{
    poison = count;
}

void ScoreBoard::setGate(int count)
{
    gateCount = count;
}

void ScoreBoard::setTime(int seconds)
{
    elapsed = seconds;
}

void ScoreBoard::setEffectTime(int doubleEffectDuration)
{
    feverTime = doubleEffectDuration;
}

void ScoreBoard::draw(int y, int x) const
{
    mvprintw(y, x, "+----------------------+");
    mvprintw(y + 1, x, "|   SCORE BOARD INFO   |");
    mvprintw(y + 2, x, "+----------------------+");

    mvprintw(y + 3, x, " B: %2d / %2d", curLen, maxLen);
    mvprintw(y + 4, x, " +: %2d", growth);
    mvprintw(y + 5, x, " -: %2d", poison);
    mvprintw(y + 6, x, " G: %2d", gateCount);
    mvprintw(y + 7, x, " T: %2d sec", elapsed);

    if (feverTime > 0)
    {
        attron(COLOR_PAIR(9));
        mvprintw(y + 8, x, "+---------------+");
        if (feverTime % 2 == 0)
        {
            mvprintw(y + 9, x, "   Fever Time!  ");
        }
        mvprintw(y + 10, x, "+---------------+");
        attroff(COLOR_PAIR(9));
    }
}

// ─── 미션 관련 메서드 구현 ───

void ScoreBoard::setGoals(int g, int p, int gate, int lengthGoal)
{
    goalGrowth = g;
    goalPoison = p;
    goalGate = gate;
    goalB = lengthGoal;
}

void ScoreBoard::increaseGrowth()
{
    curGrowth++;
}

void ScoreBoard::increasePoison()
{
    curPoison++;
}

void ScoreBoard::increaseGate()
{
    curGate++;
}

void ScoreBoard::resetCurrentMissions()
{
    curGrowth = 0;
    curPoison = 0;
    curGate = 0;
    // goalB는 그대로 두고, 길이만 curLen을 비교합니다.
}

void ScoreBoard::drawMissions(int y, int x) const
{
    // ─── “MISSION” 타이틀과 테두리 ───
    mvprintw(y, x, " +-------------+ ");
    mvprintw(y + 1, x, " |   MISSION   | ");
    mvprintw(y + 2, x, " +-------------+ ");
    // ──────────────────────────────────

    int line = y + 3;

    // B는 goalB > 0일 때만 출력
    if (goalB > 0)
    {
        bool doneB = (curLen >= goalB);
        mvprintw(line, x, " B: %d [%s]", goalB, check(doneB).c_str());
        line += 1;
    }

    mvprintw(line, x, "+: %d [%s]", goalGrowth,
             check(curGrowth >= goalGrowth).c_str());
    line += 1;

    mvprintw(line, x, "-: %d [%s]", goalPoison,
             check(curPoison >= goalPoison).c_str());
    line += 1;

    mvprintw(line, x, "G: %d [%s]", goalGate,
             check(curGate >= goalGate).c_str());
}

// ──────────────────────────────────

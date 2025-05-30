// scoreboard.cpp
#include "scoreboard.h"
#include <ncurses.h>

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
    mvprintw(y, x, "[ Score ]");
    mvprintw(y + 1, x, "B: %d/%d", curLen, maxLen);
    mvprintw(y + 2, x, "+: %d", growth);
    mvprintw(y + 3, x, "-: %d", poison);
    mvprintw(y + 4, x, "G: %d", gateCount);
    mvprintw(y + 6, x, "T: %d s", elapsed);

    if(feverTime > 0)
    {
        attron(COLOR_PAIR(9));

        mvprintw(y + 8, x, "+---------------+");
        
        if(feverTime % 2 == 0)
        {
            mvprintw(y + 9, x, "   Fever Time!  ");
        }
        mvprintw(y +10, x, "+---------------+");

        attroff(COLOR_PAIR(9));
    }
}

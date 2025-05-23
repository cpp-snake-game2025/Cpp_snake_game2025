// scoreboard.h
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

class ScoreBoard
{
public:
    void setLength(int cur, int max);
    void setGrowth(int count);
    void setPoison(int count);
    void setGate(int count);
    void setTime(int seconds);
    void draw(int y, int x) const;

private:
    int curLen = 3;
    int maxLen = 3;
    int growth = 0;
    int poison = 0;
    int gateCount = 0;
    int elapsed = 0;
};

#endif

#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <string>
#include <ncurses.h>

class ScoreBoard
{
public:
    // 기존 점수판 관련 메서드
    void setLength(int cur, int max);
    void setGrowth(int count);
    void setPoison(int count);
    void setGate(int count);
    void setTime(int seconds);
    void setEffectTime(int doubleEffectDuration);
    void draw(int y, int x) const;

    // ─── 미션 관련 멤버 추가 ───
    // Stage용 목표 설정: growth, poison, gate, 그리고 B(길이 목표)
    void setGoals(int g, int p, int gate, int lengthGoal = 0);
    // 현재값 증가
    void increaseGrowth();
    void increasePoison();
    void increaseGate();
    // 미션 체크박스 초기화 (Stage 전환 시 호출)
    void resetCurrentMissions();
    // 미션 상태 출력 (B, +, -, G)
    void drawMissions(int y, int x) const;
    // ────────────────────────────

private:
    // 기존 점수판 멤버 변수
    int curLen = 3;
    int maxLen = 3;
    int growth = 0;
    int poison = 0;
    int gateCount = 0;
    int elapsed = 0;
    int feverTime = 0; // Double effect 남은 시간

    // ─── 미션 관련 멤버 변수 ───
    int goalB = 0;      // 길이 목표치 (Stage2만 사용)
    int goalGrowth = 0; // + 아이템 목표치
    int goalPoison = 0; // - 아이템 목표치
    int goalGate = 0;   // Gate 이용 횟수 목표치
    int curGrowth = 0;  // 현재 획득한 + 아이템 개수
    int curPoison = 0;  // 현재 획득한 - 아이템 개수
    int curGate = 0;    // 현재 사용한 Gate 개수
    // ──────────────────────────

    // 내부용: 체크문자 반환 (ASCII 'V' 혹은 'X')
    std::string check(bool condition) const
    {
        return condition ? "V" : "X";
    }
};

#endif

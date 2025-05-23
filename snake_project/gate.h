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
    GateManager(int maxY, int maxX);
    void setGates(const std::vector<std::vector<int>> &map);
    void draw(int offsetY, int offsetX) const;

    bool isGate(int y, int x) const;
    GateRole getGateRole(int y, int x) const;

    std::pair<int, int> getExitGate(int y, int x) const;
    int getExitDirection(int enterDir, int exitY, int exitX, const std::vector<std::vector<int>> &map);

private:
    std::pair<int, int> gateA, gateB;
    int maxY, maxX;

    bool isOuterWall(int y, int x) const;
};

#endif

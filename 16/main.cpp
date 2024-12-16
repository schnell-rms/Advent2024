#include <cassert>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <vector>


using namespace std;

const TNumber kWALL = '#';

using TMap = std::vector<std::string>;

enum EDIRECTION {
    kEAST = 0,
    kNORTH = 1,
    kSOUTH = 2,
    kWEST = 3
};

struct SPos {

    SPos(TNumber ic, TNumber ix, TNumber iy, EDIRECTION io)
    : cost(ic)
    , x(ix), y(iy)
    , orientation(io)
    {}

    TNumber cost;
    TNumber x,y;
    EDIRECTION orientation;
    
    bool operator<(const SPos& other) const {
        return other.cost < cost;
    }
    
};

const std::vector<TNumber> kDIRs = {0, 90, -90, 180};
auto orientationCost=[](TNumber currentOrientation, TNumber goToDirection) {
    TNumber diff = abs(kDIRs[goToDirection] - kDIRs[currentOrientation]);
    if (diff == 270) 
        diff = 90;

    return 1000 * (diff / 90);
};


TNumber findCheapestPath(TMap &map, TNumber startX, TNumber startY, TNumber endX, TNumber endY, EDIRECTION startDir) {

    const TNumber nbCols = map[0].size();

    std::priority_queue<SPos> q;
    std::vector<std::vector<TNumber>> cost(4);
    cost[kEAST]  = std::vector<TNumber>(map.size() * nbCols, -1);
    cost[kNORTH] = std::vector<TNumber>(map.size() * nbCols, -1);
    cost[kSOUTH] = std::vector<TNumber>(map.size() * nbCols, -1);
    cost[kWEST]  = std::vector<TNumber>(map.size() * nbCols, -1);

    q.push(SPos(0,startX, startY, startDir));
    cost[startDir][startY * nbCols + startX] = 0;

    auto visit = [&map, &cost, &q, nbCols](const SPos &pos, TNumber dx, TNumber dy, EDIRECTION direction) {
        const TNumber x = pos.x + dx;
        const TNumber y = pos.y + dy;
        if (    (x < 0) || (x >= map.size()) || 
                (y < 0) || (y >= map[0].size()) ||
                (map[x][y] == kWALL)) {
            return;
        }

        const TNumber extraCost = 1 + orientationCost(pos.orientation, direction);
        const TNumber newCost = pos.cost + extraCost;

        const TNumber costIdx = y * nbCols + x;
        if ((cost[direction][costIdx] == -1) || (newCost < cost[direction][costIdx])) {
            cost[direction][costIdx] = newCost;
            // printVectorAsMatrix(cost[direction], true, 3, nbCols);
            // waitForKey();
            q.push(SPos(newCost, x, y, direction));
        }
    };

    printVectorAsMatrix(cost[kEAST], true, 3, nbCols);

    while (!q.empty()) {
        const SPos pos = q.top();

        if ((pos.x == endX) && (pos.y == endY))  {
            printVectorAsMatrix(cost[pos.orientation], true, 3, nbCols);
            return pos.cost;
        }

        q.pop();

        visit(pos,  0, -1, kNORTH);
        visit(pos,  0, +1, kSOUTH);
        visit(pos, +1,  0, kEAST);
        visit(pos, -1,  0, kWEST);
    }

    // printVector(cost);  

    return -1;
}

int main(int argc, char *argv[]) {

    gIS_DEBUG = argc > 2;

    std::string inputFilePath;
    if (argc > 1) {
        inputFilePath = argv[1];
    } else {
        std::cout << "No input path" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream listFile(inputFilePath);
    if (!listFile.is_open()) {
        std::cout << "Could not open input file" << std::endl;
        return EXIT_FAILURE;
    }



    std::string line;
    TMap map;
    TNumber startX, endX;
    TNumber startY, endY;
    TNumber n = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto s = line.find('S');
            if (s != std::string::npos) {
                startX = s;
                startY = n;
            }

            s = line.find('E');
            if (s != std::string::npos) {
                endX = s;
                endY = n;
            }

            map.push_back(std::move(line));
            n++;
        }
    }


    const clock_t tStart = clock();
    const TNumber minCost = findCheapestPath(map, startX, startY, endX, endY, kEAST);

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First star min cost " << minCost << endl;
}

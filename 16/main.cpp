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


std::pair<TNumber, TNumber> findCheapestPath(TMap &map, TNumber startX, TNumber startY, TNumber endX, TNumber endY, EDIRECTION startDir) {

    const TNumber nbCols = map[0].size();

    const TNumber kNO_COST_YET = -1;

    std::priority_queue<SPos> q;
    std::vector<std::vector<TNumber>> cost(4);
    cost[kEAST]  = std::vector<TNumber>(map.size() * nbCols, kNO_COST_YET);
    cost[kNORTH] = std::vector<TNumber>(map.size() * nbCols, kNO_COST_YET);
    cost[kSOUTH] = std::vector<TNumber>(map.size() * nbCols, kNO_COST_YET);
    cost[kWEST]  = std::vector<TNumber>(map.size() * nbCols, kNO_COST_YET);

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
        if ((cost[direction][costIdx] == kNO_COST_YET) || (newCost < cost[direction][costIdx])) {
            cost[direction][costIdx] = newCost;
            // gIS_DEBUG && printVectorAsMatrix(cost[direction], true, 3, nbCols);
            // gIS_DEBUG && waitForKey();
            q.push(SPos(newCost, x, y, direction));
        }
    };

    gIS_DEBUG && printVectorAsMatrix(cost[kEAST], true, 3, static_cast<int>(nbCols));

    TNumber bestCost = -1;

    std::vector<SPos> cheapestArrivals;

    while (!q.empty()) {
        const SPos pos = q.top();

        if ((pos.x == endX) && (pos.y == endY))  {
            // printVectorAsMatrix(cost[pos.orientation], true, 3, nbCols);
            if (bestCost == -1) {
                bestCost = pos.cost;
            }
            if (bestCost < pos.cost)
                break;
            cheapestArrivals.push_back(pos);
        }

        q.pop();

        visit(pos,  0, -1, kNORTH);
        visit(pos,  0, +1, kSOUTH);
        visit(pos, +1,  0, kEAST);
        visit(pos, -1,  0, kWEST);
    }

    // printVector(cost);  
    // Extract the routes
    std::vector<TNumber> routes(map.size() * nbCols, 0);

    std::function<void (TNumber, TNumber, EDIRECTION, TNumber)> markRoute
         = [&](TNumber x, TNumber y, EDIRECTION orientation, TNumber currentCost) {
        if (    (x < 0) || (x >= map.size()) ||
                (y < 0) || (y >= map[0].size()) ||
                (map[x][y] == kWALL)) {
            return;
        }

        const TNumber costIdx = y * nbCols + x;
        if ((cost[orientation][costIdx] == kNO_COST_YET) || (cost[orientation][costIdx] != currentCost)) {
            // We were not here. That cost was not propagated.
            return;
        }

        routes[costIdx] = 1;
        TNumber dx = 0, dy = 0;

        // Where were we coming from to x,y? Let's go there:
        switch (orientation) {
            case kEAST:
                dx = -1;
                break;
            case kWEST:
                dx = +1;
                break;
            case kSOUTH:
                dy = -1;
                break;
            case kNORTH:
                dy = +1;
                break;
            default:
                assert(false);
        }

        // What orientation were you haivng in that position? Try them all:
        const auto px = x + dx;
        const auto py = y + dy;

        markRoute(px, py, kEAST, currentCost - 1 - orientationCost(kEAST, orientation));
        markRoute(px, py, kWEST, currentCost - 1 - orientationCost(kWEST, orientation));
        markRoute(px, py, kSOUTH, currentCost - 1 - orientationCost(kSOUTH, orientation));
        markRoute(px, py, kNORTH, currentCost - 1 - orientationCost(kNORTH, orientation));
    };

    for (auto &pos : cheapestArrivals) {
        assert(endX = pos.x);
        assert(endY = pos.y);
        assert(bestCost == pos.cost);
        markRoute(endX, endY, pos.orientation, bestCost);
    }

    gIS_DEBUG && cout << "Routes" << endl;
    gIS_DEBUG && printVectorAsMatrix(routes, true, 3, nbCols);

    // Count positions on route:
    TNumber counter = 0;
    for (auto v : routes) {
        counter += v;
    }

    return make_pair(bestCost, counter);
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
    const auto result = findCheapestPath(map, startX, startY, endX, endY, kEAST);

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First star min cost " << result.first << endl;
    cout << "Second star number of positions " << result.second << endl;
}

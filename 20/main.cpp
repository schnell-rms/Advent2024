#include <cassert>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_set>
#include <vector>


using namespace std;

const TNumber kWALL = '#';

using TMap = std::vector<std::string>;

struct SPos {

    SPos(TNumber ic, TNumber ix, TNumber iy)
    : cost(ic)
    , x(ix), y(iy)
    {}

    TNumber cost;
    TNumber x,y;

    bool operator<(const SPos& other) const {
        return other.cost < cost;
    }
    
};

TNumber findCheapestPath(TMap &map, TNumber startX, TNumber startY, TNumber endX, TNumber endY) {

    const TNumber nbCols = map[0].size();

    const TNumber kNO_COST_YET = -1;

    std::priority_queue<SPos> q;
    std::vector<TNumber> cost(map.size() * nbCols, kNO_COST_YET);

    q.push(SPos(0,startX, startY));
    cost[startY * nbCols + startX] = 0;

    auto visit = [&map, &cost, &q, nbCols](const SPos &pos, TNumber dx, TNumber dy) {
        const TNumber x = pos.x + dx;
        const TNumber y = pos.y + dy;
        if (    (x < 0) || (x >= map.size()) || 
                (y < 0) || (y >= map[0].size()) ||
                (map[y][x] == kWALL)) {
            return;
        }

        const TNumber newCost = pos.cost + 1;

        const TNumber costIdx = y * nbCols + x;
        if ((cost[costIdx] == kNO_COST_YET) || (newCost < cost[costIdx])) {
            cost[costIdx] = newCost;
            q.push(SPos(newCost, x, y));
        }
    };

    TNumber bestCost = -1;

    std::vector<SPos> cheapestArrivals;

    while (!q.empty()) {
        const SPos pos = q.top();

        if ((pos.x == endX) && (pos.y == endY))  {
            if (bestCost == -1) {
                bestCost = pos.cost;
            }
            if (bestCost < pos.cost)
                break;
            cheapestArrivals.push_back(pos);
        }

        q.pop();

        visit(pos,  0, -1);
        visit(pos,  0, +1);
        visit(pos, +1,  0);
        visit(pos, -1,  0);
    }

    gIS_DEBUG && cout << "Costs" << endl;
    gIS_DEBUG && printVectorAsMatrix(cost, true, 3, static_cast<int>(nbCols));


// Extract the route
    std::unordered_set<TNumber> routes;//(map.size() * nbCols, 0);

    std::function<void (TNumber, TNumber, TNumber)> markRoute
         = [&](TNumber x, TNumber y, TNumber currentCost) {
        if (    (x < 0) || (x >= map.size()) ||
                (y < 0) || (y >= map[0].size()) ||
                (map[y][x] == kWALL) //--> or pass through walls, let the cost check work
                ) {
            return;
        }

        const TNumber costIdx = y * nbCols + x;
        if ((cost[costIdx] == kNO_COST_YET) || (cost[costIdx] != currentCost)) {
            // We were not here. That cost was not propagated.
            return;
        }

        // routes[costIdx] = 1;
        routes.insert(costIdx);

        markRoute(x - 1, y    , currentCost - 1);
        markRoute(x + 1, y    , currentCost - 1);
        markRoute(x    , y - 1, currentCost - 1);
        markRoute(x    , y + 1, currentCost - 1);
    };

    for (auto &pos : cheapestArrivals) {
        assert(endX = pos.x);
        assert(endY = pos.y);
        assert(bestCost == pos.cost);
        markRoute(endX, endY, bestCost);
    }

    // Go along the fastes route and search for cheats:
    std::vector<std::pair<TNumber, TNumber>> jumps = {
        {0,2}, {0,-2}, {2,0}, {-2,0}, {-1,+1}, {-1,-1}, {+1,+1}, {+1,-1} 
    };

    TNumber counter = 0;
    for (auto pos : routes) {
        const TNumber costPos = cost[pos];
        for (auto jump : jumps) {
            const TNumber p = pos + jump.first + jump.second * nbCols;
            const auto it = routes.find(p);
            counter += ((routes.end() != it) && (costPos - cost[*it] >= 102));
        }
    }

    return counter;
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

    gIS_DEBUG && cout << "The map" << endl;
    gIS_DEBUG && printMatrix(map);

    const clock_t tStart = clock();
    const auto result = findCheapestPath(map, startX, startY, endX, endY);

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First star min cost " << result << endl;
}

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

TNumber kSIZE = 7;

const TNumber kFREE = 0;
const TNumber kCORRUPTED = 1;

using TMap = std::vector<TNumber>;

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

std::pair<TNumber, TNumber> findCheapestPath(TMap &map, TNumber startX, TNumber startY, TNumber endX, TNumber endY) {

    const TNumber kNO_COST_YET = -1;

    std::priority_queue<SPos> q;
    std::vector<TNumber> cost(kSIZE * kSIZE, kNO_COST_YET);

    q.push(SPos(0,startX, startY));
    cost[startY * kSIZE + startX] = 0;

    auto visit = [&map, &cost, &q](const SPos &pos, TNumber dx, TNumber dy) {
        const TNumber x = pos.x + dx;
        const TNumber y = pos.y + dy;
        if (    (x < 0) || (x >= kSIZE) || 
                (y < 0) || (y >= kSIZE) ||
                (map[y * kSIZE + x] == kCORRUPTED)) {
            return;
        }

        const TNumber newCost = pos.cost + 1;

        const TNumber costIdx = y * kSIZE + x;
        if ((cost[costIdx] == kNO_COST_YET) || (newCost < cost[costIdx])) {
            cost[costIdx] = newCost;
            // gIS_DEBUG && printVectorAsMatrix(cost, true, 3, static_cast<int>(kSIZE));
            // gIS_DEBUG && waitForKey();
            q.push(SPos(newCost, x, y));
        }
    };

    TNumber bestCost = -1;

    std::vector<SPos> cheapestArrivals;

    while (!q.empty()) {
        const SPos pos = q.top();

        if ((pos.x == endX) && (pos.y == endY))  {
            // gIS_DEBUG && printVectorAsMatrix(cost, true, 3, static_cast<int>(kSIZE));
            if ((bestCost == -1) || (bestCost > pos.cost)) {
                bestCost = pos.cost;
            }
            // if (bestCost < pos.cost)
            //     break;
            cheapestArrivals.push_back(pos);
        }

        q.pop();

        visit(pos,  0, -1);
        visit(pos,  0, +1);
        visit(pos, +1,  0);
        visit(pos, -1,  0);
    }

    gIS_DEBUG && cout << "Costs" << endl;
    gIS_DEBUG &&printVectorAsMatrix(cost, true, 3, static_cast<int>(kSIZE));

return std::make_pair(bestCost,bestCost);

    // // Extract the routes
    // std::vector<TNumber> routes(kSIZE * kSIZE, 0);

    // std::function<void (TNumber, TNumber, EDIRECTION, TNumber)> markRoute
    //      = [&](TNumber x, TNumber y, EDIRECTION orientation, TNumber currentCost) {
    //     if (    (x < 0) || (x >= kSIZE) ||
    //             (y < 0) || (y >= kSIZE) ||
    //             (map[y * kSIZE + x] == kCORRUPTED)) {
    //         return;
    //     }

    //     const TNumber costIdx = y * kSIZE + x;
    //     if ((cost[costIdx] == kNO_COST_YET) || (cost[costIdx] != currentCost)) {
    //         // We were not here. That cost was not propagated.
    //         return;
    //     }

    //     routes[costIdx] = 1;
    //     TNumber dx = 0, dy = 0;

    //     // Where were we coming from to x,y? Let's go there:
    //     switch (orientation) {
    //         case kEAST:
    //             dx = -1;
    //             break;
    //         case kWEST:
    //             dx = +1;
    //             break;
    //         case kSOUTH:
    //             dy = -1;
    //             break;
    //         case kNORTH:
    //             dy = +1;
    //             break;
    //         default:
    //             assert(false);
    //     }

    //     // What orientation were you haivng in that position? Try them all:
    //     const auto px = x + dx;
    //     const auto py = y + dy;

    //     markRoute(px, py, kEAST, currentCost - 1);
    //     markRoute(px, py, kWEST, currentCost - 1);
    //     markRoute(px, py, kSOUTH, currentCost - 1);
    //     markRoute(px, py, kNORTH, currentCost - 1);
    // };

    // for (auto &pos : cheapestArrivals) {
    //     assert(endX = pos.x);
    //     assert(endY = pos.y);
    //     assert(bestCost == pos.cost);
    //     markRoute(endX, endY, pos.orientation, bestCost);
    // }

    // gIS_DEBUG && cout << "Routes" << endl;
    // gIS_DEBUG && printVectorAsMatrix(routes, true, 3, kSIZE);

    // // Count positions on route:
    // TNumber counter = 0;
    // for (auto v : routes) {
    //     counter += v;
    // }

    // return make_pair(bestCost, counter);
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

    clock_t tStart = clock();

    std::string line;
    std::vector<std::vector<TNumber>> coords;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            coords.push_back(std::move(allNumbers(line)));
            if ((coords.back()[0] > kSIZE) || (coords.back()[1] > kSIZE))
                kSIZE = 71;
        }
    }

    const TNumber nbBytes = kSIZE == 71 ? 1024 : 12;
    std::vector<TNumber> map(kSIZE * kSIZE, kFREE);
    for (TNumber i = 0; (i<nbBytes) && (i<coords.size()); i++) {
        map[coords[i][0] + coords[i][1] * kSIZE] = kCORRUPTED;
    }


    auto r = findCheapestPath(map, 0, 0, kSIZE-1, kSIZE - 1);
    cout << r.first << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

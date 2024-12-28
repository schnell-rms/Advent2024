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

TNumber findCheapestPath( const TMap &map,
                                TNumber startX, TNumber startY,
                                TNumber endX, TNumber endY) {

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
            q.push(SPos(newCost, x, y));
        }
    };

    TNumber bestCost = -1;

    while (!q.empty()) {
        const SPos pos = q.top();

        if ((pos.x == endX) && (pos.y == endY))  {
            if (bestCost == -1) {
                bestCost = pos.cost;
            }
            if (bestCost < pos.cost)
                break;
        }

        q.pop();

        visit(pos,  0, -1);
        visit(pos,  0, +1);
        visit(pos, +1,  0);
        visit(pos, -1,  0);
    }

    return bestCost;
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

    TNumber bestCost = findCheapestPath(map, 0, 0, kSIZE-1, kSIZE - 1);
    cout << "First star: best cost " << bestCost << endl;
    cout << "Time taken first: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << "\n" << endl;

// SECOND:
    if (gIS_DEBUG) {
        // Brute force: just try all bytes:
        TNumber i = nbBytes;
        clock_t tBruteForceStart = clock();
        for (; (i<coords.size()); i++) {
            map[coords[i][0] + coords[i][1] * kSIZE] = kCORRUPTED;

            bestCost = findCheapestPath(map, 0, 0, kSIZE-1, kSIZE - 1);
            if (bestCost == -1) {
                cout << "Second star: break at byte " << coords[i][0] << ',' << coords[i][1] << endl;
                break;
            }
        }
        cout << "Time taken brute force: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << "\n" << endl;
    }

    // Binary search:
    auto prepareMap = [&](TNumber currentByteIdx, TNumber targetByteIdx) {
        if (currentByteIdx < targetByteIdx) {
            for (TNumber i = currentByteIdx; i < targetByteIdx; i++) {
                map[coords[i][0] + coords[i][1] * kSIZE] = kCORRUPTED;
            }
        } else {
            TNumber j =currentByteIdx;
            assert(j==coords.size() || map[coords[j][0] + coords[j][1] * kSIZE] == kFREE);
            for (TNumber i = currentByteIdx - 1; i >= targetByteIdx; i--) {
                map[coords[i][0] + coords[i][1] * kSIZE] = kFREE;
            }
        }
    };

    TNumber lowByte = nbBytes;
    TNumber highByte = coords.size() - 1;
    // Revert the brute force map, in case it was run:
    prepareMap(highByte, lowByte);

    clock_t tBinarySearchStart = clock();
    // Last not set idx:
    TNumber lastSetIdx = lowByte;
    while (lowByte <= highByte) {
        TNumber midByte = lowByte + (highByte - lowByte) / 2;

        prepareMap(lastSetIdx, midByte);
        lastSetIdx = midByte;

        // mid not set. All coorupted until and including mid-1
        bestCost = findCheapestPath(map, 0, 0, kSIZE-1, kSIZE - 1);
        if (bestCost == -1) {
            if (midByte == 3030) {
                int k = 0;
            }
            // Set midByte - 1 free:
            prepareMap(midByte, midByte - 1);
            if (-1 != findCheapestPath(map, 0, 0, kSIZE-1, kSIZE - 1)) {
                cout << "Second star: break at byte " << coords[midByte-1][0] << ',' << coords[midByte-1][1] << endl;
                break;
            } else {
                highByte = midByte - 1;
            }
            prepareMap(midByte - 1, midByte);
        } else {
            lowByte = midByte + 1;
        }
    }

    cout << "Time taken binary search: " << (double)(clock() - tBinarySearchStart)/CLOCKS_PER_SEC << endl;
}

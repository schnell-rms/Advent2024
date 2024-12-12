#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_set>
#include <unordered_map>

using namespace std;

// <row + direction, columns>
using TSides = unordered_map<TNumber, unordered_set<TNumber>>;

TNumber kUP     = 0;
TNumber kDOWN   = 1;
TNumber kRIGHT  = 2;
TNumber kLEFT   = 3;

TNumber key(TNumber row, TNumber direction) {
    return (row << 2) | direction;
}

void measure(std::vector<string> map, std::vector<std::vector<bool>> &traversed, TNumber i, TNumber j, char crop,
             TNumber &area, TNumber &perimeter,
             TSides &hSides, TSides &vSides, TNumber dir) {

    if ((i < 0) || (i>=map.size()) || (j < 0) || (j >= map[0].size()) || (crop != map[i][j])) {
        if ((dir == kUP) || (dir == kDOWN))
            vSides[key(i,dir)].insert(j);
        else
            hSides[key(j,dir)].insert(i);
        perimeter++;
        return;
    }

    if (traversed[i][j]) {
        return;
    }

    traversed[i][j] = true;
    area += 1;

    // Inside the map, on the same crop:
    measure(map, traversed, i - 1, j    , crop, area, perimeter, hSides, vSides, kUP);
    measure(map, traversed, i + 1, j    , crop, area, perimeter, hSides, vSides, kDOWN);
    measure(map, traversed, i    , j - 1, crop, area, perimeter, hSides, vSides, kLEFT);
    measure(map, traversed, i    , j + 1, crop, area, perimeter, hSides, vSides, kRIGHT);
}

TNumber countSides(TSides &sides) {

    TNumber counter = 0;
    for (auto &oneSide : sides) {
        // Search for neighbouting columns:
        auto &columns = oneSide.second;
        while (!columns.empty()) {
            auto it = columns.begin();
            auto initCol = *it;
            while(it != columns.end()) {
                TNumber col = *it;
                columns.erase(it);
                it = columns.find(col + 1);
            }

            it = columns.find(initCol - 1);
            while(it != columns.end()) {
                TNumber col = *it;
                columns.erase(it);
                it = columns.find(col - 1);
            }
            counter++;
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

    clock_t tStart = clock();

    std::string line;
    std::vector<std::string> map;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            map.push_back(std::move(line));
        }
    }

    std::vector<std::vector<bool>> traversed(map.size(), std::vector<bool>(map[0].size(), false));

    assert(!map.empty());
    const TNumber n = map.size();
    const TNumber m = map[0].size();
    TNumber sum = 0;
    TNumber sumSecond = 0;
    for (TNumber i=0; i<map.size(); i++) {
        for (TNumber j=0; j<map.size(); j++) {
            if (traversed[i][j]) continue;

            TNumber area = 0;
            TNumber perimeter = 0;
            TSides hSides, vSides;
            measure(map, traversed, i, j, map[i][j], area, perimeter, hSides, vSides, kDOWN);
            TNumber nbSides = countSides(hSides);
            nbSides += countSides(vSides);
            gIS_DEBUG && cout << "Crop " << map[i][j] << " area " << area << " perimeter " << perimeter << " sides " << nbSides << endl;
            sum += area * perimeter;
            sumSecond += area * nbSides;
        }
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First: Total cost is: " << sum << endl;
    cout << "Second: Total cost is: " << sumSecond << endl;
}


#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;

const char kOBSTACLE = '#';

const std::vector<std::pair<TNumber, TNumber>> kDIRECTIONS = {
    std::make_pair(-1, 0),
    std::make_pair( 0, 1),
    std::make_pair( 1, 0),
    std::make_pair( 0, -1)
};


// map of pos key , direction key
using TVisited = std::unordered_map<TNumber, TNumber>;

TNumber posKey(std::pair<TNumber, TNumber> pos) {
    return (pos.first << 16) | pos.second;
}

std::pair<TNumber, TNumber> posFromKey(TNumber posKey) {
    return std::make_pair(posKey >> 16, posKey & 0xFFFF);
}

TNumber dirKey(TNumber dirIdx) {
    return 1 << dirIdx;
}

void markVisited(TVisited &visited, std::pair<TNumber, TNumber> pos, TNumber dirIdx) {
    visited[posKey(pos)] |= dirKey(dirIdx);
}

bool isVisited(const TVisited &visited, std::pair<TNumber, TNumber> pos, TNumber dirIdx) {
    auto it = visited.find(posKey(pos));
    if (it != visited.end()) {
        return it->second & dirKey(dirIdx);
    }
    return false;
}

// Returns:
//  - true when a cycle is detected
//  - false when getting out of the map
bool patrol(std::vector<std::string> &map, TVisited &visited, std::pair<TNumber, TNumber> pos, TNumber dirIdx) {

    std::pair<TNumber,TNumber> dir = kDIRECTIONS[dirIdx];

    auto mark=[&map](const std::pair<TNumber, TNumber> &pos) {
        map[pos.first][pos.second] = 'X';
    };

    auto isInMap=[&map](const std::pair<TNumber, TNumber> &pos) {
        return  pos.first >=0 && pos.first<map.size() &&
                pos.second >=0 && pos.second<map[0].size();
    };

    auto isObstacle=[&map, &isInMap](const std::pair<TNumber, TNumber> &pos) {
        return  isInMap(pos) && map[pos.first][pos.second] == kOBSTACLE;
    };

    auto move=[&map](std::pair<TNumber, TNumber> &pos, const std::pair<TNumber,TNumber> &dir) {
        return std::make_pair(pos.first  + dir.first, pos.second + dir.second);
    };

    auto isNextObstacle=[&map, &isObstacle](const std::pair<TNumber, TNumber> &pos, const std::pair<TNumber,TNumber> &dir) {
        std::pair<TNumber, TNumber> nextPos = {pos.first  + dir.first, pos.second + dir.second};
        return isObstacle(nextPos);
    };

    auto nextDirection=[&dirIdx]() {
        dirIdx = (dirIdx + 1) % kDIRECTIONS.size();
        return kDIRECTIONS[dirIdx];
    };

    while (isInMap(pos)) {

        if (isVisited(visited, pos, dirIdx)) {
            return true;
        }

        markVisited(visited, pos, dirIdx);

        if (!isNextObstacle(pos, dir)) {
            mark(pos);
            pos = move(pos, dir);
        } else {
            // Assume we wil not change directions forever. I.e. start between obstacles...
            dir = nextDirection();
        }
    }

    return false;
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
    std::vector<std::string> map;
    std:pair<TNumber, TNumber> startPos{0,0};
    bool foundStart = false;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (!foundStart) {
                // Search just for '^', enough for the given input
                auto j = line.find('^');
                if (j!=std::string::npos) {
                    startPos.first = map.size();
                    startPos.second = j;
                    foundStart = true;
                }
            }
            map.push_back(std::move(line));
        }
    }

    TVisited visitedX;
    patrol(map, visitedX, startPos, 0);

    TNumber steps = 0;
    for (auto line : map) {
        steps += std::count(line.begin(), line.end(), 'X');
    }

    cout << "Number of positions " << steps << endl;

    TNumber nbCycles = 0;
    // Part 2: put an obstacle somewhere in the previous path ...
    for (auto posX : visitedX) {
        auto pos = posFromKey(posX.first);
        if (pos == startPos) {
            // ... but not in the first position
            continue;
        }

        map[pos.first][pos.second] = kOBSTACLE;
        TVisited visitedY;
        nbCycles += patrol(map, visitedY, startPos, 0);

        // Remove the obstacle, e.g. put back the 'X' or the '.'
        map[pos.first][pos.second] = '.';
    }

    cout << "Number of possible obstacle positions " << nbCycles << endl;

}

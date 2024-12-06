
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

void patrol(std::vector<std::string> &map, std::pair<TNumber, TNumber> pos, TNumber dirIdx) {

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
        if (!isNextObstacle(pos, dir)) {
            mark(pos);
            pos = move(pos, dir);
        } else {
            // Assume we wil not change directions forever. I.e. start between obstacles...
            dir = nextDirection();
        }
    }
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


    patrol(map, startPos, 0);

    TNumber steps = 0;
    for (auto line : map) {
        steps += std::count(line.begin(), line.end(), 'X');
    }

    cout << "Number of positions " << steps << endl;
}

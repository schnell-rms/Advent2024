#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Antenas < frequency , vector of positions>
using TAntenas = std::unordered_map<char, std::vector<std::pair<int, int>>>;

using TAntinodesPos = std::unordered_set<int>;

void addAntinode(int row, int col, int maxRow, int maxCol, TAntinodesPos &antinodes) {
    if ((row >=0) && (col >=0) && (row < maxRow) && (col < maxCol)) {
        const int key = (row << 16) | col;
        antinodes.insert(key);
    }
}

void getAntenas(const std::string &line, TAntenas &antenas, int row) {
    for (int j = 0; j<line.size(); j++) {
        // By specs , should check line[j] to be alha numeric, not different then '.'
        if ((line[j] != '.') && (line[j] != '#')) {
            antenas[line[j]].push_back(std::make_pair(row, j));
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

    clock_t tStart = clock();

    std::string line;
    std::vector<std::string> nodeNames;
    int maxRow = 0;
    int maxCol = 0;
    TAntenas antenas;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (maxCol == 0)
                maxCol = static_cast<int>(line.size());
                
            getAntenas(line, antenas, maxRow);
            maxRow++;
        }
    }

    // Find antinodes:
    // for each frequency:
    TAntinodesPos anti;
    TAntinodesPos harmonics;
    for (auto antena : antenas) {
        auto &positions = antena.second;
        for (auto i = 0; i<positions.size(); ++i) {
            for (auto j = i + 1; j<positions.size(); ++j) {
                const int dRow = positions[j].first  - positions[i].first;
                const int dCol = positions[j].second - positions[i].second;
                addAntinode(positions[i].first - dRow, positions[i].second - dCol, maxRow, maxCol, anti);
                addAntinode(positions[j].first + dRow, positions[j].second + dCol, maxRow, maxCol, anti);

                // Harmonics: by the current implementation each antena positions will added multiple times
                // (i.e. once for each other antena)
                for (int posRow = positions[i].first, posCol = positions[i].second;
                     (posRow >= 0) && (posRow < maxRow) && (posCol >= 0) && (posCol < maxCol);
                     posRow -= dRow, posCol -= dCol) {

                    addAntinode(posRow, posCol, maxRow, maxCol, harmonics);
                }

                for (int posRow = positions[j].first, posCol = positions[j].second;
                     (posRow >= 0) && (posRow < maxRow) && (posCol >= 0) && (posCol < maxCol);
                     posRow += dRow, posCol += dCol) {

                    addAntinode(posRow, posCol, maxRow, maxCol, harmonics);
                }
            }
        }
    }

    cout << "Nb antinodes is " << anti.size() << endl;
    cout << "Nb harmonics is " << harmonics.size() << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl; 
}

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <vector>

using namespace std;

const std::string kXMAS = "XMAS";

TNumber findXMAS(const std::vector<std::string>& lines,
                 TNumber i, TNumber j,
                 TNumber dirI, TNumber dirJ,
                 TNumber k) {

    i += dirI;
    j += dirJ;
    if (    (i < 0) || (lines.size() <= i)
        ||  (j < 0) || (lines[0].size() <= j)
        ||  (lines[i][j] != kXMAS[k])) {
        return 0;
    }

    if (k==kXMAS.size()-1) {
        return 1;
    }

    return findXMAS(lines, i, j, dirI, dirJ, k+1);
}

const std::vector<std::string> kMMSSkeys = {"MMSS", "MSSM", "SSMM", "SMMS"};

bool isDoubleMAS(const std::vector<std::string>& lines,
                 TNumber i, TNumber j,
                 TNumber key)
{
    assert(lines[i][j] == 'A');

    return  lines[i-1][j-1] == kMMSSkeys[key][0] &&
            lines[i-1][j+1] == kMMSSkeys[key][1] &&
            lines[i+1][j+1] == kMMSSkeys[key][2] &&
            lines[i+1][j-1] == kMMSSkeys[key][3];
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
    std::vector<std::string> lines;
    std::vector<std::string> nodeNames;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
    }

    // Part 1:
    TNumber count = 0;
    for (TNumber i=0; i<lines.size(); ++i) {
        for (TNumber j=0; j<lines[0].size(); ++j) {
            if (lines[i][j] == kXMAS[0]) {
                count += findXMAS(lines, i, j, -1, -1, 1);
                count += findXMAS(lines, i, j, -1,  0, 1);
                count += findXMAS(lines, i, j, -1,  1, 1);

                count += findXMAS(lines, i, j,  0, -1, 1);
                count += findXMAS(lines, i, j,  0,  1, 1);

                count += findXMAS(lines, i, j,  1, -1, 1);
                count += findXMAS(lines, i, j,  1,  0, 1);
                count += findXMAS(lines, i, j,  1,  1, 1);
            }
        }
    }

    cout << "Total number of XMAS: " << count << endl;

    // Part 2:
    TNumber countX = 0;
    for (TNumber i=1; i<lines.size()-1; ++i) {
        for (TNumber j=1; j<lines[0].size()-1; ++j) {
            if (lines[i][j] == 'A') {
                countX +=   isDoubleMAS(lines, i, j, 0) ||
                            isDoubleMAS(lines, i, j, 1) ||
                            isDoubleMAS(lines, i, j, 2) ||
                            isDoubleMAS(lines, i, j, 3);
            }
        }
    }

    cout << "Total number of crossed X-MAS: " << countX << endl;
}

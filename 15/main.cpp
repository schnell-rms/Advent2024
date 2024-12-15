#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;


using TMap = std::vector<std::string>;


const char kROBOT = '@';
const char kBOX = '0';
const char kWALL = '#';

// Safe to use size_t becuase of the borders on the first row and column...
bool isFree(size_t &cX, size_t &cY, 
            TMap &silo,
            TNumber dx,
            TNumber dy) {

    bool isAllowed = false;
    switch (silo[cY + dy][cX + dx])
    {
    case '.':
        isAllowed = true;
        break;
    case 'O': {
        size_t nx = cX + dx;
        size_t ny = cY + dy;
        isAllowed = isFree(nx, ny, silo, dx, dy);
    }
    default:
        break;
    }

    if (isAllowed) {
        const char me = silo[cY][cX];
        silo[cY][cX] = '.';
        cX += dx;
        cY += dy;
        assert(silo[cY][cX] = '.');
        silo[cY][cX] = me;
    }

    return isAllowed;
}

void move(  size_t &cX, size_t &cY, //current position
            TMap &silo,
            char dir) {

    TNumber dx = 0, dy = 0;
    switch (dir) {
        case '<':
            dx--;
            break;
        case '^':
            dy--;
            break;
        case 'v':
            dy++;
            break;
        case '>':
            dx++;
            break;
        default:
            assert(false);
    }

    isFree(cX, cY, silo, dx, dy);
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
    TMap silo;
    bool isMapReading = true;
    std::string moves;
    size_t robotX, robotY;
    size_t n = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isMapReading) {
                const size_t m = line.find(kROBOT);
                if (m != std::string::npos) {
                    robotX = m;
                    robotY = n;
                }
                silo.push_back(std::move(line));
            } else {
                moves += line;
            }
        } else {
            isMapReading = false;
        }
        n++;
    }

    cout << "Initial:" << endl;
    printMatrix(silo, true, 1);


    for (char dir : moves) {
        move(robotX, robotY, silo, dir);
        // cout << "Move " << dir << endl;
        // printMatrix(silo, true, 1);
    }

    cout << "Final:" << endl;
    printMatrix(silo, true, 1);

    size_t sum = 0;
    const auto m = silo[0].size();
    for (size_t i = 0; i<silo.size(); ++i) {
        for (size_t j = 0; j<m; ++j) {
            if ('O' == silo[i][j])
                sum += i*100 + j;
        }
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First sum: " << sum << endl;

}

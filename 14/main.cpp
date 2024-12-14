#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;

using TQuadrantsCount = std::unordered_map<TNumber,TNumber>;


const TNumber kY = 103;
const TNumber kX = 101;

const TNumber hY = kY / 2;
const TNumber hX = kX / 2;


TNumber getQuadrant(TNumber x, TNumber y) {
    if ((x == hX) || (y == hY)) return 4;
    
    if (x > hX) x--;
    if (y > hY) y--;

    const auto gx = x / hX;
    const auto gy = y / hY;

    assert(gx == 0 || gx == 1);
    assert(gy == 0 || gy == 1);

    return (gx << 1) + gy;
}

struct SRobot {

    SRobot(const std::string &line) {
        auto numbers = allNumbers(line);
        px = numbers[0];
        py = numbers[1];
        vx = numbers[2];
        vy = numbers[3];
    }

    TNumber quaddrant(TNumber nbSteps) {
        TNumber fx = (px + vx * nbSteps) % kX;
        fx = (fx + kX) % kX; // in case it was negative
        TNumber fy = (py + vy * nbSteps) % kY;
        fy = (fy + kY) % kY;

        return getQuadrant(fx,fy);
    }

    // Initial position
    TNumber px, py;
    // Speed
    TNumber vx, vy;
};




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
    TQuadrantsCount qcount;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            SRobot robot(line);
            qcount[robot.quaddrant(100)]++;
        }
    }

    TNumber product = 1;
    for (TNumber i=0; i<4; i++) {
        // quadrant with no robot? might be...
        product *= qcount[i];
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
    cout << "First product " << product << endl;
}

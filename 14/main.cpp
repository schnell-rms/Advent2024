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

class CRobotsMap {
public:
    CRobotsMap() : isRobot(kY * kX, false)
    {}

    void clear() {
        std::fill(isRobot.begin(), isRobot.end(), false);
    }

    void putRobot(TNumber x, TNumber y) {
        isRobot[y * kX + x] = 1;
    }

    bool hasTree() {
        TNumber nb = 0;
        TNumber maxLen = 0;
        // Search for two lines one below the other as a candidate for tree
        for (TNumber y = 0; y<kY - 1; y++) {
            const auto rowsOffset = y * kX;
            TNumber len = 0;
            for (TNumber x = 0; x<kX; x++) {
                if (isRobot[rowsOffset + x] && isRobot[rowsOffset + kX + x]) {
                    len++;
                } else {
                    if (maxLen < len) {
                        maxLen = len;
                        len = 0;
                    }
                }
            }
        }
        return maxLen > 10;
    }

    const std::vector<int> &positions() const { return isRobot; }
private:
    std::vector<int> isRobot;
};


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
        cx = px;
        cy = py;
    }

    TNumber quaddrant(TNumber nbSteps) {
        TNumber fx = (px + vx * nbSteps) % kX;
        fx = (fx + kX) % kX; // in case it was negative
        TNumber fy = (py + vy * nbSteps) % kY;
        fy = (fy + kY) % kY;

        return getQuadrant(fx,fy);
    }


    void moveOneStep() {
        cx += vx;
        cx = (cx + kX) % kX; // in case it was negative
        cy += vy;
        cy = (cy + kY) % kY;
    }

    // Initial position
    TNumber px, py;
    // Speed
    TNumber vx, vy;
    // Current position
    TNumber cx, cy;
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

    const clock_t tStart = clock();

    std::string line;
    TQuadrantsCount qcount;
    std::vector<SRobot> robots;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            SRobot robot(line);
            qcount[robot.quaddrant(100)]++;
            robots.push_back(std::move(robot));
        }
    }

    TNumber product = 1;
    for (TNumber i=0; i<4; i++) {
        // quadrant with no robot? might be...
        product *= qcount[i];
    }

    // Second star: find Xmas tree.
    TNumber steps = 1;
    CRobotsMap robotPos;

    for (auto& r:robots) {
        r.moveOneStep();
        robotPos.putRobot(r.cx, r.cy);
    }

    while (!robotPos.hasTree()) {
        robotPos.clear();
        for (auto& r:robots) {
            r.moveOneStep();
            robotPos.putRobot(r.cx, r.cy);
        }
        steps++;
    }

    const clock_t tEnd = clock();
    printVectorAsMatrix(robotPos.positions(), true, 1, kX);

    cout << "First product " << product << endl;
    cout << "Second steps " << steps << endl;
    cout << "Time taken: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << endl;
}

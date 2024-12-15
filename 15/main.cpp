#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;


using TMap = std::vector<std::string>;


const char kROBOT = '@';
const char kSMALL_BOX = 'O';
const char kWALL = '#';
const char kFREE = '.';

const char kBIG_BOX_LEFT = '[';
const char kBIG_BOX_RIGHT = ']';

// Safe to use size_t becuase of the borders on the first row and column...
bool isFree(size_t &cX, size_t &cY, 
            TMap &silo,
            TNumber dx,
            TNumber dy,
            bool reallyMove) {

    bool isAllowed = false;
    switch (silo[cY + dy][cX + dx])
    {
    case kFREE:
        isAllowed = true;
        break;
    case kSMALL_BOX: {
        size_t nx = cX + dx;
        size_t ny = cY + dy;
        isAllowed = isFree(nx, ny, silo, dx, dy, reallyMove);
    }
    default:
        break;
    }

    if (isAllowed && reallyMove) {
        const char me = silo[cY][cX];
        silo[cY][cX] = kFREE;
        cX += dx;
        cY += dy;
        assert(silo[cY][cX] = kFREE);
        silo[cY][cX] = me;
    }

    return isAllowed;
}

void move(  size_t &cX, size_t &cY, //current position
            TMap &silo,
            char dir,
            std::function<bool (size_t&, size_t&, TMap&, TNumber, TNumber, bool)> checkAndMoveBoxes) {

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

    // Horizontal moves can be made in one go without previous check:
    const bool reallyMove = (dy == 0) || checkAndMoveBoxes(cX, cY, silo, dx, dy, false);
    if (reallyMove)
        checkAndMoveBoxes(cX, cY, silo, dx, dy, true);    
}

bool isBigFree( size_t &cX, size_t &cY,
                TMap &silo,
                TNumber dx,
                TNumber dy,
                bool reallyMove) {

    const bool isRobot = silo[cY][cX] == '@';
    const bool isHorizontalMove = dy == 0;
    const bool isBoxVerticalMovement = !isRobot && !isHorizontalMove;
    // When moving a box verically, always work with its left side:
    assert(!isBoxVerticalMovement || (kBIG_BOX_LEFT == silo[cY][cX]));
    bool isAllowed = false;
    if (isRobot || isHorizontalMove) {
        // Only the current position move (i.e. not the right one too)
        // Move the robot: care for vertical moves
        // OR move box horizontally: old approach, one by one
        // Next to move:
        size_t nx = cX + dx;
        size_t ny = cY + dy;
        switch (silo[ny][nx])
        {
        case kFREE:
            isAllowed = true;
            break;
        case kBIG_BOX_RIGHT:
            if (!isHorizontalMove) {
                // Robot: Vertical move: take the LEFT side of the box:
                nx -= 1;
            }
            [[fallthrough]];
        case kBIG_BOX_LEFT: {
            // already on the LEFT side of the box.
            isAllowed = isBigFree(nx, ny, silo, dx, dy, reallyMove);
        }
        case kWALL:
            break;
        default:
            assert(false);
            break;
        }
    } else {
        // Move a box verically.
        assert(silo[cY][cX] == kBIG_BOX_LEFT);
        assert(silo[cY][cX+1] == kBIG_BOX_RIGHT);
        const bool isBlocked = (kWALL == silo[cY + dy][cX + dx]) || (kWALL == silo[cY + dy][cX + dx + 1]);
        const bool isAllFree = (kFREE == silo[cY + dy][cX + dx]) && (kFREE == silo[cY + dy][cX + dx + 1]);
        if (isAllFree) {
            isAllowed = true;
        } else if (!isBlocked) {
            // One or two BIG boxes in direction. Could be: [], ][, ]. or .[
            size_t nx = cX + dx;
            size_t ny = cY + dy;

            if (silo[cY + dy][cX + dx] == kBIG_BOX_LEFT) {
                isAllowed = isBigFree(nx, ny, silo, dx, dy, reallyMove);//[]
            } else if (silo[cY + dy][cX + dx] == kBIG_BOX_RIGHT) {
                nx--;
                isAllowed = isBigFree(nx, ny, silo, dx, dy, reallyMove);//](.[)?
            } else {
                assert(kFREE == silo[cY + dy][cX + dx]);
                isAllowed = true;
            }

            if (isAllowed && silo[cY + dy][cX + dx + 1] == kBIG_BOX_LEFT) {
                nx = cX + dx + 1;
                ny = cY + dy;
                isAllowed = isBigFree(nx, ny, silo, dx, dy, reallyMove);//(.])?[
            }
        }
    }

    if (isAllowed && reallyMove) {
        const char me = silo[cY][cX];
        silo[cY][cX] = '.';
        if (isBoxVerticalMovement) {
            assert(me == '[');
            assert(silo[cY][cX+1] == ']');
            silo[cY][cX+1] = '.';
        }
        cX += dx;
        cY += dy;
        assert(silo[cY][cX] = '.');
        silo[cY][cX] = me;
        if (isBoxVerticalMovement) {
            assert(silo[cY][cX+1] = '.');
            silo[cY][cX+1] = ']';
        }
    }

    return isAllowed;
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

    std::string line, bigLine;
    TMap silo, bigSilo;
    bool isMapReading = true;
    std::string moves;
    size_t robotX, robotY;
    size_t robotBigX, robotBigY;
    size_t n = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isMapReading) {
                const size_t m = line.find(kROBOT);
                if (m != std::string::npos) {
                    robotX = m;
                    robotY = n;
                }
                bigLine.reserve(line.size() * 2);
                for (char c : line) {
                    switch (c) {
                        case kWALL:
                            bigLine += kWALL;
                            bigLine += kWALL;
                            break;
                        case kSMALL_BOX:
                            bigLine += kBIG_BOX_LEFT;
                            bigLine += kBIG_BOX_RIGHT;
                            break;
                        case kFREE:
                            bigLine += kFREE;
                            bigLine += kFREE;
                            break;
                        case kROBOT:
                            robotBigX = m*2;
                            robotBigY = n;
                            bigLine += kROBOT;
                            bigLine += kFREE;
                            break;
                        default:
                            assert(false);
                    }
                }

                bigSilo.push_back(std::move(bigLine));
                silo.push_back(std::move(line));
            } else {
                moves += line;
            }
        } else {
            isMapReading = false;
        }
        n++;
    }

// First star:
    cout << "Initial FIRST:" << endl;
    printMatrix(silo, true, 1);

    const clock_t tFirstStart = clock();
    for (char dir : moves) {
        move(robotX, robotY, silo, dir, isFree);
        gIS_DEBUG && cout << "Move " << dir << endl;
        gIS_DEBUG && printMatrix(silo, true, 1);
    }

    size_t sum = 0;
    auto m = silo[0].size();
    for (size_t i = 0; i<silo.size(); ++i) {
        for (size_t j = 0; j<m; ++j) {
            if ('O' == silo[i][j])
                sum += i*100 + j;
        }
    }

    const clock_t tFirstEnd = clock();

    cout << "Final FIRST:" << endl;
    printMatrix(silo, true, 1);

// Second star:
    cout << "Initial SECOND:" << endl;
    printMatrix(bigSilo, true, 1);

    const clock_t tSecondStart = clock();
    for (char dir : moves) {
        move(robotBigX, robotBigY, bigSilo, dir, isBigFree);
        gIS_DEBUG && cout << "Move " << dir << endl;
        gIS_DEBUG && printMatrix(bigSilo, true, 1);
    }

    size_t sum2nd = 0;
    m = bigSilo[0].size();
    for (size_t i = 0; i<bigSilo.size(); ++i) {
        for (size_t j = 0; j<m; ++j) {
            if ('[' == bigSilo[i][j])
                sum2nd += i*100 + j;
        }
    }
    const clock_t tSecondEnd = clock();

    cout << "Final SECOND:" << endl;
    printMatrix(bigSilo, true, 1);

    cout << "First star's taken time: " << double(tFirstEnd - tFirstStart)/CLOCKS_PER_SEC << endl;
    cout << "First sum: " << sum << endl;

    cout << "Second star's taken time: " << double(tSecondEnd - tSecondStart)/CLOCKS_PER_SEC << endl;
    cout << "Second sum: " << sum2nd << endl;
}

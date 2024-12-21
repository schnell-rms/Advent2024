#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <unordered_map>
#include <string>

using namespace std;

using TKeyboard = std::unordered_map<char, std::pair<int, int>>;

TKeyboard kNumericKeys = {
    {'7',{0,0}},{'8',{0,1}},{'9',{0,2}},
    {'4',{1,0}},{'5',{1,1}},{'6',{1,2}},
    {'1',{2,0}},{'2',{2,1}},{'3',{2,2}},
    {' ',{3,0}},{'0',{3,1}},{'A',{3,2}},
};

struct SKeyboard {
    SKeyboard(const char *s): keys(s) {}
    
    bool isOut(int x, int y) const {
        return  x<0 || x > 3 ||
                y<0 || y > keys.size() / 3;
    }
    
    char key(int x, int y) const {
        return keys[x + y * 3];
    }
private:
    std::string keys;
};

SKeyboard kNumericKeyboard("789456123 0A");
SKeyboard kDirectionalKeyboard(" ^A<v>");

TKeyboard kDirectionalKeys = {
    {' ',{0,0}},{'^',{0,1}},{'A',{0,2}},
    {'<',{1,0}},{'v',{1,1}},{'>',{1,2}}
};

const TNumber kINFINITE = 1L << 60;

using TComplexity = std::unordered_map<int, TNumber>;
int ckey(char from, char to, int level) {
    return (((from << 8) | to) << 16) | level;
}

using TMoves = std::string;

using TAllPaths = std::unordered_map<int, std::vector<std::string>>;

TAllPaths gAllPathsBetweenKeys;

void allPaths(std::vector<std::string> &paths,
              TMoves current,
              const SKeyboard &keyboard,
              int posx, int posy,
              int dx, int dy)
{
    if (keyboard.isOut(posx, posy) ||
        ' ' == keyboard.key(posx, posy)) {
        return;
    }
    
    if ((dx == 0) && (dy == 0)) {
        current += 'A';
        paths.push_back(current);
        return;
    }
    
    if (dx != 0) {
        const int ddx = dx/abs(dx);
        const char dirChar = ddx > 0 ? '>' : '<';
        
        allPaths(paths, current + dirChar,
                 keyboard,
                 posx + ddx, posy,
                 dx - ddx, dy);
    }

    if (dy != 0) {
        const int ddy = dy/abs(dy);
        const char dirChar = ddy > 0 ? 'v' : '^';

        allPaths(paths, current + dirChar,
                 keyboard,
                 posx, posy + dy/abs(dy),
                 dx, dy - ddy);
    }
}

// All possible paths:
std::vector<TMoves> moveToKey(  const TKeyboard &keyboard,
                                const SKeyboard &keyvector,
                                char from, char to) {
    auto pathKey = ckey(from, to, 0);
    auto it = gAllPathsBetweenKeys.find(pathKey);
    if (it != gAllPathsBetweenKeys.end()) {
        return it->second;
    }
    
    std::pair<int, int> fkey = keyboard.at(from);
    std::pair<int, int> tkey = keyboard.at(to);
    
    std::vector<TMoves> allMoves;
    const int dx = tkey.second - fkey.second;
    const int dy = tkey.first  - fkey.first;
    
    allPaths(allMoves,
             "",
             keyvector,
             fkey.second, fkey.first,
             dx, dy);
    
    assert(!allMoves.empty());
    gAllPathsBetweenKeys[pathKey] = allMoves;
    return allMoves;
}

// 2 paths only: horizontal, then vertical or the other way
std::vector<TMoves> moveToKeyOld(const TKeyboard &keyboard,
                                 char from, char to) {
    std::pair<int, int> fkey = keyboard.at(from);
    std::pair<int, int> tkey = keyboard.at(to);

    std::pair<int, int> spacekey = keyboard.at(' ');

    std::vector<TMoves> allMoves;
    // ' ' irrelevant:
    const int dx = tkey.second - fkey.second;
    const int dy = tkey.first  - fkey.first;

    const bool isSpaceOnHorizontalPath = (fkey.first == spacekey.first) && (tkey.second == spacekey.second);

    if (!isSpaceOnHorizontalPath) {
        TMoves hmoves;
        // horizontal:
        for (int x = 0; x < abs(dx); x++) {
            hmoves.push_back(dx > 0 ? '>' : '<');
        }

        // vertical:
        for (int y = 0; y < abs(dy); y++) {
            hmoves.push_back(dy > 0 ? 'v' : '^');
        }
        
        hmoves += 'A';
        allMoves.push_back(std::move(hmoves));
    }

    const bool isSpaceOnVerticalPath = (fkey.second == spacekey.second) && (tkey.first == spacekey.first);

    if (!isSpaceOnVerticalPath) {
        TMoves vmoves;
        // vertical:
        for (int y = 0; y < abs(dy); y++) {
            vmoves.push_back(dy > 0 ? 'v' : '^');
        }

        // horizontal:
        for (int x = 0; x < abs(dx); x++) {
            vmoves.push_back(dx > 0 ? '>' : '<');
        }
        
        vmoves += 'A';
        allMoves.push_back(std::move(vmoves));
    }

    assert(!allMoves.empty());
    return allMoves;
}


TNumber findTwoKeysComplexity(  TComplexity &complexities,
                                const TKeyboard &keys,
                                const SKeyboard &keyvector,
                                char from, char to,
                                int level)
{
    const auto moveKey = ckey(from, to, level);
    auto it = complexities.find(moveKey);
    if (it != complexities.end()) {
        return it->second;
    }

//    std::vector<TMoves> m = moveToKey(keys, keyvector, from, to);
// Only 2 versions to move, is enough:
    std::vector<TMoves> m = moveToKeyOld(keys, from, to);
    TNumber minComplexity = kINFINITE;
    
    if (level <= 0) {
        for (TMoves& moves : m ) {
            assert(!moves.empty());
            size_t c = moves.size();
            if (minComplexity > c) {
                minComplexity = c;
            }
        }
    } else {
        for (TMoves& moves : m ) {
            TNumber complexity = 0;
            assert(!moves.empty());
            for (size_t i = 0; i<moves.size(); ++i) {
                auto res = findTwoKeysComplexity(complexities,
                                                 kDirectionalKeys,
                                                 kDirectionalKeyboard,
                                                 (i == 0) ? 'A' : moves[i-1], moves[i],
                                                 level-1);
                complexity += res;
            }
            if (minComplexity > complexity) {
                minComplexity = complexity;
            }
        }
    }

    assert(complexities.find(moveKey) == complexities.end());
    assert(minComplexity > 0);
    assert(minComplexity != kINFINITE);
    complexities[moveKey] = minComplexity;
    return minComplexity;
}

TComplexity findAllBasicComplexities(int intermediaryLevels){

    TComplexity allBasicComplexities;
    allBasicComplexities[ckey('A', 'A', intermediaryLevels)] = 0;
    for (auto from : kNumericKeys) {
        if (from.first == ' ')
            continue;
        for (auto to : kNumericKeys) {
            if (to.first == ' ')
                continue;

            
            findTwoKeysComplexity(allBasicComplexities,
                                  kNumericKeys,
                                  kNumericKeyboard,
                                  from.first, to.first,
                                  intermediaryLevels);
        }
    }
    
    return allBasicComplexities;
}

size_t findComplexity(const TComplexity &complexities,
                      std::string &needToPress,
                      TNumber number,
                      int intermediaryLevels) {
    needToPress = 'A' + needToPress;

    TNumber len = 0;
    for (size_t i=1; i< needToPress.size(); i++) {
        const auto moveKey = ckey(needToPress[i-1], needToPress[i], intermediaryLevels);
        const TNumber localLen = complexities.at(moveKey);
        len += localLen;
    }
    return len * number;
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
    
    const int firstNbLevels = 2;
    TComplexity complexities2 = findAllBasicComplexities(firstNbLevels);
    const int secondNbLevels = 25;
    TComplexity complexities25 = findAllBasicComplexities(secondNbLevels);

    std::string line;
    TNumber firstStar = 0;
    TNumber secondStar = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto n = allNumbers(line)[0];
            firstStar  += findComplexity(complexities2, line, n, firstNbLevels);
            secondStar  += findComplexity(complexities25, line, n, secondNbLevels);
        }
    }

    cout << "First star: " << firstStar << endl;
    cout << "2nd star: " << secondStar << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
}

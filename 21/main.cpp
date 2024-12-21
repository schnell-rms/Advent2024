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

TKeyboard kDirectionalKeys = {
    {' ',{0,0}},{'^',{0,1}},{'A',{0,2}},
    {'<',{1,0}},{'v',{1,1}},{'>',{1,2}}
};


struct SPath {
    std::string path;
    size_t complexity() {
        return path.empty() ? (1u << 30) : path.length();
    };
};

using TComplexity = std::unordered_map<int, SPath>;
int ckey(char from, char to, int level) {
    return (((from << 8) | to) << 16) | level;
}

using TMoves = std::string;

std::vector<TMoves> moveToKey(  const TKeyboard &keyboard,
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

SPath findTwoKeysComplexity(    TComplexity &complexities,
                                const TKeyboard &keys,
                                char from, char to,
                                int level)
{
    const auto moveKey = ckey(from, to, level);
    auto it = complexities.find(moveKey);
    if (it != complexities.end()) {
        return it->second;
    }

    std::vector<TMoves> m = moveToKey(keys, from, to);
    SPath minPath;

    if (level <= 0) {
        for (TMoves& moves : m ) {
            assert(!moves.empty());
            int c = static_cast<int>(moves.size());
            if (minPath.complexity() > c) {
                minPath.path = moves;
            }
        }
    } else {
        for (TMoves& moves : m ) {
            SPath s;
            assert(!moves.empty());
            if (moves.size() == 1) {
                s.path = moves;
            } else {
                for (size_t i = 0; i<moves.size(); ++i) {
                    auto res = findTwoKeysComplexity(complexities,
                                                     kDirectionalKeys,
                                                     (i == 0) ? 'A' : moves[i-1], moves[i],
                                                     level-1);
                    s.path += res.path;
                }
            }
            if (minPath.complexity() > s.path.length()) {
                minPath = s;
            }
        }
    }

    assert(complexities.find(moveKey) == complexities.end());
    assert(!minPath.path.empty());
    complexities[moveKey] = minPath;
    return minPath;
}

TComplexity findAllBasicComplexities(int intermediaryLevels){

    TComplexity allBasicComplexities;
    for (auto from : kNumericKeys) {
        if (from.first == ' ')
            continue;
        for (auto to : kNumericKeys) {
            if (to.first == ' ')
                continue;

            findTwoKeysComplexity(allBasicComplexities,
                                  kNumericKeys,
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

    int len = 0;
    for (size_t i=1; i< needToPress.size(); i++) {
        const auto moveKey = ckey(needToPress[i-1], needToPress[i], intermediaryLevels);
        const auto localLen = complexities.at(moveKey);
        len += localLen.path.size();
        cout << localLen.path << "(" << needToPress[i-1] << needToPress[i] << ")" << " ... ";
    }
    cout << " : " << len  << endl;
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
    
    const size_t intermediaryLevels = 2;
    TComplexity complexities = findAllBasicComplexities(intermediaryLevels);

    std::string line;
    size_t firstStar = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto n = allNumbers(line);
            cout << line << " n= " << n[0] <<endl;
            
            firstStar  += findComplexity(complexities, line, n[0], intermediaryLevels);//n[0]);
        }
    }

    cout << "First star: " << firstStar << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
}

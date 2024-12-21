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

using TMoves = std::string;

TMoves moveToKey(const TKeyboard &keyboard,
                 char from, char to) {
    std::pair<int, int> fkey = keyboard.at(from);
    std::pair<int, int> tkey = keyboard.at(to);

    std::pair<int, int> spacekey = keyboard.at(' ');

    TMoves moves;
    // ' ' irrelevant:
    const int dx = tkey.second - fkey.second;
    const int dy = tkey.first  - fkey.first;

    bool isHorizontalSpaceOnFromRow = fkey.first == spacekey.first;
    bool isHorizontalFirst = !isHorizontalSpaceOnFromRow;

    if (isHorizontalFirst) {
        // horizontal:
        for (int x = 0; x < abs(dx); x++) {
            moves.push_back(dx > 0 ? '>' : '<');
        }

        // vertical:
        for (int y = 0; y < abs(dy); y++) {
            moves.push_back(dy > 0 ? 'v' : '^');
        }
    } else {
        // vertical:
        for (int y = 0; y < abs(dy); y++) {
            moves.push_back(dy > 0 ? 'v' : '^');
        }

        // horizontal:
        for (int x = 0; x < abs(dx); x++) {
            moves.push_back(dx > 0 ? '>' : '<');
        }
    }   

    return moves;
}

TMoves findCommands(const TKeyboard &keyboard, const std::string &needToPress) {

    TMoves moves;
    for (size_t i = 1; i<needToPress.size(); i++) {
        moves.append(moveToKey(keyboard, needToPress[i-1], needToPress[i]));
        moves += 'A';
    }
    return moves;
}

size_t findComplexity(std::string &numericCodes) {
    numericCodes = 'A' + numericCodes;
    TMoves mdir1 = findCommands(kNumericKeys, numericCodes);

    mdir1 = 'A' + mdir1;
    TMoves mdir2 = findCommands(kDirectionalKeys, mdir1);
    
    mdir2 = 'A' + mdir2;
    TMoves mdir3 = findCommands(kDirectionalKeys, mdir2);

    numericCodes.erase(numericCodes.begin());
    numericCodes.erase(numericCodes.end());
    size_t complexity = mdir3.size() * std::stoi(numericCodes);
    cout << complexity << endl;
    return complexity;
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
    size_t firstStar = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            firstStar  += findComplexity(line);
        }
    }

    cout << "First star: " << firstStar << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

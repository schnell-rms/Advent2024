#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;

TNumber firstStar(const std::string &line) {

    bool isFile = true;

    std::vector<TNumber> blocks;
    std::unordered_map<TNumber, TNumber> freeSpace;

    TNumber fileId = 0;

    for (auto i = 0; i<line.size(); ++i) {
        TNumber n = line[i] - '0'; 
        if (isFile) {
            for (auto j=0; j<n; ++j) {
                blocks.push_back(fileId);
            }
            fileId++;
        } else {
            for (auto j=0; j<n; ++j) {
                blocks.push_back(-1);
            }   
        }
        isFile = !isFile;
    }

    // printVector(blocks);

    TNumber sum = 0;

    TNumber lastFileBlock = blocks.size() - 1;
    while (blocks[lastFileBlock] == -1) {
        lastFileBlock--;
    }

    TNumber currentBlock = 0;
    while (currentBlock <= lastFileBlock) {
        if (blocks[currentBlock] >= 0) {
            sum += blocks[currentBlock] * currentBlock;
        } else {
            sum += blocks[lastFileBlock] * currentBlock;
            lastFileBlock--;
            while (blocks[lastFileBlock] == -1) {
               lastFileBlock--;
            }
        }

        currentBlock++;
    }

    return sum;
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
    getline(listFile, line);

    const auto sum1 = firstStar(line);
    cout << "First star " << sum1 << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;  
}

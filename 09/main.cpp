#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <map>

using namespace std;

struct SDescriptor {
    TNumber blockIdx;//first block
    TNumber fileId;//-1 for free
    TNumber length;

    bool isFile() const { return fileId >= 0; }

    bool hasSpace(TNumber spaceNeeded) const { return (fileId == -1) && (length >= spaceNeeded); }

    // Called for free spaces:
    void removeSpace(TNumber nbBlocksToRemove) {
        blockIdx += nbBlocksToRemove;
        length -= nbBlocksToRemove;
    }

    TNumber begin() const { return blockIdx; }
    TNumber end() const { return blockIdx + length; }
};

std::pair<TNumber, TNumber> bothStars(const std::string &line) {

    bool isFile = true;
    std::vector<SDescriptor> blockDescriptors;
    TNumber fileId = 0;
    TNumber nbBlocks = 0;
    for (auto i = 0; i<line.size(); ++i) {
        TNumber n = line[i] - '0'; 
        blockDescriptors.push_back({nbBlocks, isFile ? fileId : -1, n});
        nbBlocks += n;
        fileId += isFile;
        isFile = !isFile;
    }

// FIRST
    TNumber sumFirst = 0;
    TNumber lastFileDescriptor = blockDescriptors.size() - 1;
    while ((lastFileDescriptor>0) && !blockDescriptors[lastFileDescriptor].isFile())
        lastFileDescriptor--;
    
    assert(lastFileDescriptor > 0);

    TNumber lastFileBlock = blockDescriptors[lastFileDescriptor].blockIdx + blockDescriptors[lastFileDescriptor].length - 1;

    for (TNumber idx = 0; (idx < blockDescriptors.size()) && (lastFileBlock >= blockDescriptors[idx].begin()); idx++) {
        // Traverse the descriptor
        for (TNumber i = blockDescriptors[idx].begin(); (i< blockDescriptors[idx].end()) && (lastFileBlock >= i); ++i) {
            if (blockDescriptors[idx].isFile()) {
                sumFirst += i * blockDescriptors[idx].fileId;
            } else {
                // Get the block from the end, not used yet:
                TNumber fileId = blockDescriptors[lastFileDescriptor].fileId;
                sumFirst += i * fileId;
                // Get the next:
                lastFileBlock--;
                if (lastFileBlock < blockDescriptors[lastFileDescriptor].blockIdx) {
                    lastFileDescriptor--;
                    while ((lastFileDescriptor>0) && !blockDescriptors[lastFileDescriptor].isFile())
                        lastFileDescriptor--;
                    
                    assert(lastFileDescriptor > 0);
                    lastFileBlock = blockDescriptors[lastFileDescriptor].blockIdx + blockDescriptors[lastFileDescriptor].length - 1;
                }
            }
        }
    }

// SECOND
   TNumber sumSecond = 0;
    for (TNumber idx = blockDescriptors.size() - 1; idx >= 0; idx--) {
        // Jump over free spaces
        if (!blockDescriptors[idx].isFile()) continue;

        TNumber &length = blockDescriptors[idx].length;
        TNumber blockStartIdx = blockDescriptors[idx].blockIdx;

        // Search a free space that could host this file:
        for (auto freeSpaceIdx = 0; freeSpaceIdx<idx; freeSpaceIdx++) {
            if (blockDescriptors[freeSpaceIdx].hasSpace(length)) {
                blockStartIdx = blockDescriptors[freeSpaceIdx].blockIdx;
                blockDescriptors[freeSpaceIdx].removeSpace(length);
                break;
            }
        }

        for (TNumber i = 0; i<length; ++i) {
            sumSecond += (blockStartIdx + i) * blockDescriptors[idx].fileId;
        }
    }

    return std::make_pair(sumFirst, sumSecond);
}

TNumber firstStar(const std::string &line) {

    bool isFile = true;

    std::vector<TNumber> blocks;

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
    getline(listFile, line);

    // const auto sumFirst = firstStar(line);
    // cout << "First star " << sumFirst << endl;
    
    const auto sums = bothStars(line);
    cout << "First star " << sums.first << endl;
    cout << "Second star " << sums.second << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;  
}

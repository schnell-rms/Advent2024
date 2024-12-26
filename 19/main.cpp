#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

bool isPossible(std::string_view target, const std::vector<std::string> &patterns) {

    if (target.empty()) {
        return true;
    }

    for (auto &p : patterns) {
        if (target.starts_with(p) && isPossible(target.substr(p.size()), patterns)) {
            return true;
        }
    }

    return false;
}

// from what index, how many ways
using TMemory = std::unordered_map<TNumber, TNumber>;

TNumber countPossibilities( std::string_view target,
                            TNumber index,
                            const std::vector<std::string> &patterns,
                            TMemory &memory) {

    if (target.empty()) {
        return 1;
    }

    auto it = memory.find(index);
    if (it != memory.end()) {
        return it->second;
    }

    TNumber counter = 0;
    for (auto &p : patterns) {
        if (target.starts_with(p)) {
            counter += countPossibilities(  target.substr(p.size()),
                                            index + p.size(),
                                            patterns,
                                            memory);
        }
    }

    memory[index] = counter;
    return counter;
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
    std::vector<std::string> patterns;

    if (getline(listFile, line)) {
        patterns = allRegex(line, "([a-z]+)");
    }

    std::vector<std::string> targets;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            targets.push_back(std::move(line));
        }
    }

    // Both stars:
    TNumber first = 0;
    TNumber second = 0;
    TNumber counter;
    TMemory memory;
    for (auto target:targets) {
        // first += isPossible(target, patterns);
        memory.clear();
        counter = countPossibilities(target, 0, patterns, memory);
        first += counter != 0;
        second += counter;
    }

    cout << "First " << first << endl;
    cout << "Second " << second << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
}

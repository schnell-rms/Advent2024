#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;

bool isInOrder( const std::vector<std::vector<TNumber>> &rules,
                std::vector<TNumber> &order)
{
    std::unordered_map<TNumber, TNumber> orderMap;
    for (TNumber idx = 0; idx < order.size(); idx++) {
        orderMap[order[idx]] = idx;
    }

    for (auto rule: rules) {
        auto itBefore = orderMap.find(rule[0]);
        auto itAfter = orderMap.find(rule[1]);
        if (itBefore != orderMap.end() && itAfter != orderMap.end()) {
            if (itAfter->second < itBefore->second) {
                return false;
            }
        }
    }

    return true;
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


    std::string line;
    bool isRule = true;
    std::vector<std::vector<TNumber>> rules;
    TNumber sum = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isRule) {
                rules.push_back(allNumbers(line));
            } else {
                auto order = allNumbers(line);
                if (isInOrder(rules, order)) {
                    sum += order[order.size()/2];
                }
            }
        } else {
            isRule = false;
        }
    }

    cout << "Sum is " << sum << endl;
}

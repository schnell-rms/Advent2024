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

    bool isInOrder = true;

    for (auto rule: rules) {
        auto itBefore = orderMap.find(rule[0]);
        auto itAfter = orderMap.find(rule[1]);
        if (itBefore != orderMap.end() && itAfter != orderMap.end()) {
            if (itAfter->second < itBefore->second) {
                isInOrder = false;
                break;
            }
        }
    }

    if (!isInOrder) {
        std::unordered_map<TNumber, std::vector<TNumber>> rulesMap;
        for (auto rule: rules) {
            rulesMap[rule[0]].push_back(rule[1]);
        }

        auto orderRule = [&rulesMap](TNumber nBefore, TNumber nAfter) {
            // Is there a rule defining pages to be after nAfter?
            auto ruleIt = rulesMap.find(nAfter);
            if (ruleIt == rulesMap.end())
                return true; // no rule, everything in order

            // Does that rule require nBefore to be after nAfter?
            auto it = std::find(ruleIt->second.begin(), ruleIt->second.end(), nBefore);
            return it == ruleIt->second.end(); // nBefore not found, no such rule, return true.
        };

        std::sort(order.begin(), order.end(), orderRule);
    }

    return isInOrder;
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
    TNumber sumInOrder = 0;
    TNumber sumNotInOrder = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isRule) {
                rules.push_back(allNumbers(line));
            } else {
                auto order = allNumbers(line);
                if (isInOrder(rules, order)) {
                    sumInOrder += order[order.size()/2];
                } else {
                    sumNotInOrder += order[order.size()/2];
                }
            }
        } else {
            isRule = false;
        }
    }

    cout << "Sum in order is " << sumInOrder << endl;
    cout << "Sum NOT in order is " << sumNotInOrder << endl;
}

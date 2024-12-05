#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

using TRulesMap = std::unordered_map<TNumber, std::unordered_set<TNumber>>;

bool isInOrder( const std::vector<std::vector<TNumber>> &rules,
                TRulesMap &rulesMap,
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
        auto orderRule = [&rulesMap](TNumber nBefore, TNumber nAfter) {
            // Is there a rule defining pages to be after nAfter?
            auto ruleIt = rulesMap.find(nAfter);
            if (ruleIt == rulesMap.end())
                return true; // no rule, everything in order

            // Does that rule require nBefore to be after nAfter?
            auto it = ruleIt->second.find(nBefore);
            return it == ruleIt->second.end(); // nBefore not found, no such rule, return true.
        };

        std::sort(order.begin(), order.end(), orderRule);
    }

    return isInOrder;
}

// Topological sort using Kahn's algorithm
bool topologicalSort(TRulesMap &rulesMap,
                     std::vector<TNumber> &order)
{

    std::unordered_set<TNumber> orderSet;
    for (auto node : order) {
        orderSet.insert(node);
    }

    // Extract only the rules that are used by the numbers in order.
    // In the other order, not like the rules, i.e. key is the TO node, containg the FROM nodes.
    TRulesMap graph;

    std::vector<TNumber> topo;
    std::unordered_set<TNumber> nodesWithoutPrecedence; // Will be build based on those with precedence:
    std::unordered_set<TNumber> nodesWithPrecedence;

    for (auto node : order) {
        const auto it = rulesMap.find(node);
        if (it != rulesMap.end()) {
            for (auto to : it->second) {
                if (orderSet.find(to) != orderSet.end()) {
                    nodesWithPrecedence.insert(to);
                    graph[to].insert(node);
                }
            }
        }
    }

    for (auto to : order) {
        const auto it = nodesWithPrecedence.find(to);
        if (it == nodesWithPrecedence.end()) {
            nodesWithoutPrecedence.insert(to);
        }
    }

    while (!nodesWithoutPrecedence.empty()) {
        auto from = *nodesWithoutPrecedence.begin();
        nodesWithoutPrecedence.erase(nodesWithoutPrecedence.begin());

        topo.push_back(from);

        auto it = rulesMap.find(from);
        if (it != rulesMap.end()) {
            for (auto to : it->second) {
                const auto toIt = graph.find(to);
                // assert(toIt != graph.end());
                if (toIt != graph.end()) {
                    assert(toIt->second.find(from) != toIt->second.end());
                    toIt->second.erase(from);
                    if (toIt->second.empty()) {
                        nodesWithoutPrecedence.insert(to);
                        graph.erase(toIt);
                    }
                }
            }
        }
    }

    // Assert no cycles:
    assert(graph.empty());
    order.swap(topo);
    return !graph.empty();
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

    TNumber sumNotInOrder_Topological = 0;

    TRulesMap rulesMap;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isRule) {
                rules.push_back(allNumbers(line));
            } else {
                auto order = allNumbers(line);
                auto orderForTopo = order;
                if (isInOrder(rules, rulesMap, order)) {
                    sumInOrder += order[order.size()/2];
                } else {
                    sumNotInOrder += order[order.size()/2];

                    assert(isInOrder(rules, rulesMap, order));

                    topologicalSort(rulesMap, orderForTopo);
                    sumNotInOrder_Topological += orderForTopo[orderForTopo.size()/2];

                    assert(order == orderForTopo);
                }
            }
        } else {
            isRule = false;
            // Put the rules in a map
            for (auto rule: rules) {
                rulesMap[rule[0]].insert(rule[1]);
            }
        }
    }

    cout << "Sum in order is " << sumInOrder << endl;
    cout << "Sum NOT in order is " << sumNotInOrder << endl;
    cout << "Sum NOT in topological order is " << sumNotInOrder_Topological << endl;
}

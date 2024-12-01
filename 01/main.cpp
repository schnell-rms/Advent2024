#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>


using namespace std;

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

    std::vector<int> ids1, ids2;
    unordered_map<int, int> countId2;

    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto numbers = allNumbers(line);
            ids1.push_back(numbers[0]);
            ids2.push_back(numbers[1]);
            countId2[numbers[1]]++;
        }
    }

    std::sort(ids1.begin(), ids1.end());
    std::sort(ids2.begin(), ids2.end());


    int diff = 0;
    int similarity = 0;
    for (auto i = 0; i<ids1.size() ;i++) {
        diff += abs(ids1[i] - ids2[i]);
        auto it = countId2.find(ids1[i]);
        if (it != countId2.end()) {
            similarity += ids1[i] * it->second;
        }
    }
  
    cout << "Diff 1 is " << diff << endl;
    cout << "Similarity is " << similarity << endl;
}

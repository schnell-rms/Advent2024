#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

#include <cassert>

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

    auto isSafe = [](const std::vector<TNumber> &numbers) {
        assert(numbers.size() >= 2);
        // Equality goes to -1
        TNumber prevSign = (numbers[1] - numbers[0] > 0) ? 1 : -1;
        for (auto i=1; i<numbers.size(); ++i) {
            auto diff = numbers[i] - numbers[i-1];
            auto sign = (diff > 0) ? 1 : -1;
            // Equality checked by the first condition. So it can be ignored in the sign, prevsign logic.
            if ((abs(diff) < 1) || (abs(diff) > 3) || (sign != prevSign)) {
                return false;
            }
        }
        return true;
    };

    std::string line;
    std::vector<std::string> nodeNames;
    int nbSafe = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto numbers = allNumbers(line);

            nbSafe += isSafe(numbers);

        }
    }
  
    cout << "Safe lines " << nbSafe << endl;
}

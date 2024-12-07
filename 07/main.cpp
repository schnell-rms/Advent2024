#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;


const TNumber kADD          = 0;
const TNumber kMultiply     = 1;
const TNumber kConcatenate  = 2;

TNumber calculate(const std::vector<TNumber> &numbers, TNumber operators, TNumber nbOperators, TNumber nbAllowedOperators) {

    TNumber s = numbers[1];
    TNumber idx = 2;
    for (int i=0; i<nbOperators; i++) {
        TNumber op = operators % nbAllowedOperators;
        operators /= nbAllowedOperators;

        switch (op) {
            case kADD:
                s += numbers[idx++];
                break;
            case kMultiply:
                s *= numbers[idx++];
                break;
            case kConcatenate:
                {
                    auto n = numbers[idx];
                    while (n > 0) {
                        s *= 10;
                        n /= 10;
                    }
                    s += numbers[idx++];
                }
                break;
            default:
                break;
        }
    }

    return s;
}


TNumber checkEquation(const std::vector<TNumber> &numbers, bool isConcatenationAllowed) {

    TNumber sum = numbers[0];
    if (numbers.size() == 2) {
        return numbers[0] == numbers[1];
    }
    assert(numbers.size() > 2);

    TNumber nOperators = numbers.size() - 2;
    TNumber nAllowedOperations = 2 + isConcatenationAllowed;
    TNumber nCombinations = std::pow(nAllowedOperations, nOperators);

    for (TNumber i = 0; i<nCombinations; ++i) {
        const TNumber result = calculate(numbers, i, nOperators, nAllowedOperations);
        if (sum == result) {
            return sum;
        }
    }

    return 0;
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
    TNumber sum = 0;
    TNumber sumConcatenation = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const auto numbers = allNumbers(line);
            sum += checkEquation(numbers, false);
            sumConcatenation += checkEquation(numbers, true);
        }
    }
  
    cout << "Sum is " << sum << endl;
    cout << "Sum with Concatenation is " << sumConcatenation << endl;
}

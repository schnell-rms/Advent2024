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

    TNumber s = numbers[0];
    TNumber idx = numbers.size() - 1;
    assert(numbers.size() == nbOperators + 2);
    // Go backwards: fail faster:
    for (int i=0; i<nbOperators; i++) {
        // Extract the operator
        TNumber op = operators % nbAllowedOperators;
        operators /= nbAllowedOperators;

        assert(numbers[idx] != 0);
        switch (op) {
            case kADD:
                s -= numbers[idx];
                break;
            case kMultiply:
                // Check if a multiplication would do it
                if (s % numbers[idx] == 0) {
                    s /= numbers[idx];
                    break;
                }
                return 0;
            case kConcatenate:
                {
                    // Check last digits of s. Would a concatenation do it?
                    auto n = numbers[idx];
                    while (n > 0) {
                        if ( n % 10 == s % 10) {
                            s /= 10;
                            n /= 10;
                        } else {
                            break;
                        }
                    }
                    if (n != 0)
                        return 0;
                }
                break;
            default:
                break;
        }
        
        idx--;
    }

    return (numbers[1] == s) ? numbers[0] : 0;
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

    clock_t tStart = clock();

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

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
    cout << "Sum is " << sum << endl;
    cout << "Sum with Concatenation is " << sumConcatenation << endl;
}

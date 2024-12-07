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

bool isCalculationOK(const std::vector<TNumber> &numbers, TNumber targetSum, bool isConcatenationAllowed, TNumber idx) {

    if (idx == 1) {
        return targetSum == numbers[1];
    }

    // Go backwards: fail faster. Check all operators
    assert(numbers[idx] != 0);

    // Multiplication first
    if (targetSum % numbers[idx] == 0) {
        if (isCalculationOK(numbers, targetSum / numbers[idx], isConcatenationAllowed, idx - 1))
            return true;
    }

    if (isConcatenationAllowed) {
        auto s = targetSum;
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

        if ((n == 0) && (isCalculationOK(numbers, s, isConcatenationAllowed, idx - 1))) {
            return true;
        }
    }

    // Addition last:
    return isCalculationOK(numbers, targetSum - numbers[idx], isConcatenationAllowed, idx - 1);
}


TNumber checkEquation(const std::vector<TNumber> &numbers, bool isConcatenationAllowed) {
    return isCalculationOK(numbers, numbers[0], isConcatenationAllowed, numbers.size() - 1)
            ? numbers[0]
            : 0;
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
            assert(numbers.size() > 2);

            sum += checkEquation(numbers, false);
            sumConcatenation += checkEquation(numbers, true);
        }
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
    cout << "Sum is " << sum << endl;
    cout << "Sum with Concatenation is " << sumConcatenation << endl;
}

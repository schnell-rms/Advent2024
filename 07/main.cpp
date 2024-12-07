#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;


const TNumber kADD = 0x0;
const TNumber kMultiply = 0x1;
const TNumber kConcatenate = 0x2;
const TNumber kNone = 0x3;

TNumber calculate(const std::vector<TNumber> &numbers, TNumber operators, TNumber nbOperators) {

    TNumber s = numbers[1];
    TNumber idx = 2;
    for (int i=0; i<nbOperators; i++) {
        auto op = operators & 0x1;
        operators >>= 1;

        if (kADD == op) {
            s += numbers[idx++];
        } else {
            s *= numbers[idx++];
        }
    }

    return s;
}

TNumber calculate2(const std::vector<TNumber> &numbers, TNumber operators, TNumber nbOperators) {

    TNumber s = numbers[1];
    TNumber idx = 2;
    for (int i=0; i<nbOperators; i++) {
        TNumber op = operators & 0x3;
        operators >>= 2;

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
            case kNone:
                return 0;
            default:
                break;
        }
    }

    return s;
}


TNumber checkEquation(const std::vector<TNumber> &numbers) {

    TNumber sum = numbers[0];
    if (numbers.size() == 2) {
        return numbers[0] == numbers[1];
    }
    assert(numbers.size() > 2);

    TNumber nOperators = numbers.size() - 2;
    TNumber nCombinations = 1 << (nOperators << 1);

    for (TNumber i = 0; i<nCombinations; ++i) {
        if (sum == calculate2(numbers, i, nOperators)) {
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
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const auto numbers = allNumbers(line);
            sum += checkEquation(numbers);
        }
    }
  
    cout << "Sum is " << sum << endl;
}

#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <stack>

#include <cassert>

using namespace std;


std::tuple<bool, TNumber> isOkTransition (TNumber prevLevel, TNumber currentLevel) {
    TNumber diff = currentLevel - prevLevel;
    TNumber sign = (diff > 0) ? 1
                            : (diff == 0) ? 0 : -1;
    // Equality checked by the first condition. No need to check sign fo 0.
    return {(diff != 0) && (abs(diff) < 4), sign};
}

bool isValid(const std::vector<TNumber> &numbers, size_t i, std::stack<TNumber> &sequence, TNumber &prevSign) {

    if (i == numbers.size()) {
        return sequence.size() + 1 >= numbers.size();
    }

    if (sequence.size() + 1 < i) {
        // more than one failure accumulated already
        return false;
    }

    bool isPushed = true;
    if (sequence.empty()) {
        sequence.push(numbers[i]);
    } else {
        TNumber prev = sequence.top();
        auto [isOk, sign] = isOkTransition(prev, numbers[i]);

        if (isOk && (sequence.size() == 1)) {
            assert(sign != 0);
            prevSign = sign;
        }

        if (isOk && (prevSign == sign)) {
            //Ok
            sequence.push(numbers[i]);
        } else {
            // jump over the current element at position i
            isPushed = false;
        }
    }

    if (isValid(numbers, i+1, sequence, prevSign)) {
        return true;
    }

    if (isPushed) {
        sequence.pop();
        return isValid(numbers, i+1, sequence, prevSign);
    }

    return false;
};

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
    int nbSafe1 = 0;
    int nbSafe2 = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto numbers = allNumbers(line);
            nbSafe1 += isSafe(numbers);

            std::stack<TNumber> sequence;
            TNumber prevSign = 0;
            nbSafe2 += isValid(numbers, 0, sequence, prevSign);;

        }
    }
  
    cout << "Safe lines 1 " << nbSafe1 << endl;
    cout << "Safe lines 2 " << nbSafe2 << endl;
}

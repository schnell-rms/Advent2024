#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include <utils.h>
#include <string>

using namespace std;

const TNumber kMODULO = 16777216;
const TNumber kMODULO_MASK = (1 << 24) - 1;

using TBananasForKey = std::unordered_map<TNumber, TNumber>;
TNumber kSET_LEAST_SIGNIFIANT_BYTE_MASK = 0xFF;
TNumber kCLEAR_MOST_SIGNIFIANT_BYTE_MASK = 0x00FFFFFF;

// <key, sum>
std::unordered_map<TNumber, TNumber> gSUM_BY_KEY;

TNumber getNextKey(TNumber currentKey, TNumber nextDifference) {
    TNumber removedOldestDifference = currentKey & kCLEAR_MOST_SIGNIFIANT_BYTE_MASK;
    removedOldestDifference <<= 8;
    const TNumber newKey = removedOldestDifference | (nextDifference & kSET_LEAST_SIGNIFIANT_BYTE_MASK);
    return newKey;
}

TNumber nextSecret(TNumber secret) {
/*
Calculate the result of multiplying the secret number by 64. Then, mix this result into the secret number. Finally, prune the secret number.
Calculate the result of dividing the secret number by 32. Round the result down to the nearest integer. Then, mix this result into the secret number. Finally, prune the secret number.
Calculate the result of multiplying the secret number by 2048. Then, mix this result into the secret number. Finally, prune the secret number.
Each step of the above process involves mixing and pruning:

To mix a value into the secret number, calculate the bitwise XOR of the given value and the secret number. Then, the secret number becomes the result of that operation. (If the secret number is 42 and you were to mix 15 into the secret number, the secret number would become 37.)
To prune the secret number, calculate the value of the secret number modulo 16777216. Then, the secret number becomes the result of that operation. (If the secret number is 100000000 and you were to prune the secret number, the secret number would become 16113920.)

*/
    secret = (secret << 6) ^ secret;
    secret &= kMODULO_MASK;
    secret = (secret >> 5) ^ secret;
    secret &= kMODULO_MASK;    
    secret = (secret << 11) ^ secret;
    secret &= kMODULO_MASK;
    return secret;
}

TNumber secretNTimes(TNumber secret, TNumber n, TBananasForKey &nbBananas){
    TNumber key = 0;
    for (int i =0; i<n; i++) {
        const TNumber ns = nextSecret(secret);
        const TNumber diff = (ns % 10) - (secret % 10);
        key  = getNextKey(key, diff);
        if (i>=3) {
            auto it = nbBananas.find(key);
            if (it == nbBananas.end()) {
                nbBananas[key] = ns % 10;
                gSUM_BY_KEY[key] += ns % 10;
            }
        }
        secret = ns;
    }

    return secret;
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
    std::vector<TNumber> nodeNames;
    TNumber sum = 0;
    std::vector<TBananasForKey> bananasMap;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            nodeNames.push_back(allNumbers(line)[0]);
            TBananasForKey nbBananas;
            TNumber ss = secretNTimes(nodeNames.back(), 2000, nbBananas);
            bananasMap.push_back(std::move(nbBananas));
            gIS_DEBUG && cout << nodeNames.back() << ": " << ss << endl;
            sum += ss;
        }
    }

    cout << "First star " << sum << endl;
    
// SECOND STAR:
    TNumber maxSum = 0;
    for (auto it : gSUM_BY_KEY) {
        if (maxSum < it.second) {
            maxSum = it.second;
        }
    }

    cout << "Second star " << maxSum << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}


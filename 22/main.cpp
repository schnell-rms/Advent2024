#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

const TNumber kMODULO = 16777216;

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
    secret %= kMODULO;
    secret = (secret >> 5) ^ secret;
    secret %= kMODULO;    
    secret = (secret << 11) ^ secret;
    secret %= kMODULO;
    return secret;
}

TNumber secretNTimes(TNumber secret, TNumber n ){
    for (int i =0; i<n; i++) {
        secret = nextSecret(secret);
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
    while(getline(listFile, line)) {
        if (!line.empty()) {
            nodeNames.push_back(allNumbers(line)[0]);
            TNumber ss = secretNTimes(nodeNames.back(), 2000);
            cout << nodeNames.back() << ": " << ss << endl;
            sum += ss;
        }
    }

    TNumber S = 123;
    cout << sum << endl;


    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}
// 1: 8685429
// 10: 4700978
// 100: 15273692
// 2024: 8667524

// 37327623 not good
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

TNumber costToPrize(const std::vector<TNumber> &deltaA,
                    const std::vector<TNumber> &deltaB,
                    const std::vector<TNumber> &dest) {

    TNumber xA = deltaA[0], yA = deltaA[1];
    TNumber xB = deltaB[0], yB = deltaB[1];
    TNumber X = dest[0], Y = dest[1];

    TNumber Bup   = X  * yA - Y * xA;
    TNumber Bdown = xB  * yA - yB * xA;

    TNumber A = 0, B = 0;

    if ((Bdown != 0) && (Bup % Bdown == 0)) {
        B = Bup / Bdown;

        TNumber Aup = X - B * xB;
        if (Aup % xA == 0) {
            A = Aup / xA;
        } else {
            B = 0;
        }
    }

    return A * 3 + B;
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
    std::vector<std::string> nodeNames;
    TNumber nbPrizes = 0;
    TNumber totalCost = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            const auto deltaA = allNumbers(line);
            getline(listFile, line);
            const auto deltaB = allNumbers(line);
            getline(listFile, line);
            const auto posPrize = allNumbers(line);

            const auto cost = costToPrize(deltaA, deltaB, posPrize);
            nbPrizes += cost != 0;
            totalCost += cost;
        }
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
    cout << "Nb prizes is " << nbPrizes << " at cost " << totalCost << endl;
}

#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

using namespace std;

std::tuple<TNumber, TNumber> mulSum(const std::string& line) {
    static bool isActive = true;
    TNumber sumAll = 0;
    TNumber sumActive = 0;
    std::smatch sm;
    std::vector<TNumber> ret;
    std::string::const_iterator searchStart(line.cbegin() );
    while (std::regex_search(searchStart, line.cend(), sm, std::regex("mul\\((\\d|\\d{2}|\\d{3}),(\\d|\\d{2}|\\d{3})\\)|do\\(\\)|don't\\(\\)"))) {

        if (sm[0] == "do()") {
            isActive = true;
        } else if (sm[0] == "don't()") {
            isActive = false;
        } else {
            const TNumber p1 = std::stol(sm[1]);
            const TNumber p2 = std::stol(sm[2]);
            const TNumber prod = p1 * p2;
            sumAll += prod;
            if (isActive) {
                sumActive += prod;
            }
        }

        searchStart = sm.suffix().first;
    }
    return std::make_tuple(sumAll, sumActive);
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

    std::string line;
    std::vector<std::string> nodeNames;
    TNumber sum = 0;
    TNumber sum2 = 0;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            auto [sumAll, sumActive] = mulSum(line);
            sum += sumAll;
            sum2 += sumActive;
        }
    }

    cout << "Sum is " << sum << endl;
    cout << "Sum2 is " << sum2 << endl;
}

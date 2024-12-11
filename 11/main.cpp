#include <iostream>
#include <sstream>
#include <fstream>
#include <cstddef>

#include <utils.h>
#include <string>

using namespace std;

std::pair<TNumber, TNumber> splitNumber(TNumber n) {
    auto s = std::to_string(n);
    const auto sz = s.size();
    if (sz & 1) {
        return std::make_pair(0, n);     
    }

    auto s2 = s.substr(sz >> 1);
    s.resize(sz >> 1);

    return std::make_pair(std::stoi(s), std::stoi(s2));
}

std::vector<TNumber> expandOne(TNumber n) {

    if (n == 0) return { 1 };

    auto p = splitNumber(n);
    if (p.first != 0) return { p.first, p.second };

    return { n * 2024 };
}


std::vector<TNumber> expand(std::vector<TNumber> numbers) {

    std::vector<TNumber> ret;
    for (auto n : numbers) {
        auto e = expandOne(n);
        ret.insert(ret.end(), e.begin(), e.end());
    }
    return ret;
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
    getline(listFile, line);
    std::vector<TNumber> numbers = allNumbers(line);


    //Brute force:
    // printVector(numbers);
    const int nbSteps = 25;
    for (auto i=0; i<nbSteps; i++) {
        auto e = expand(numbers);
        numbers.swap(e);
        // printVector(numbers);
    }

    cout << "First star " << numbers.size() << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>

#include <utils.h>
#include <string>

using namespace std;


using THistory = std::unordered_map<TNumber, TNumber>;

TNumber key(TNumber n, TNumber nbSteps) {
    return (n << 7) | nbSteps;
}

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

TNumber count(TNumber n, TNumber nbSteps, THistory &history) {

    if (nbSteps == 0) return 1;

    // Search in history first
    auto it = history.find(key(n,nbSteps));
    if (it != history.end()) {
        return it->second;
    }

    // Not found in history
    TNumber ret;
    if (n == 0) {
        ret = count(1, nbSteps - 1, history);
    } else {
        auto p = splitNumber(n);
        if (p.first != 0) {
            ret =   count(p.first , nbSteps -1, history) +
                    count(p.second, nbSteps -1, history);
        } else {
            ret = count(n * 2024, nbSteps - 1, history);
        }
    }

    // Put in history
    history[key(n, nbSteps)] = ret;
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


    THistory history;
    TNumber counterFirst = 0;
    for (TNumber n : numbers) {
        counterFirst += count(n, 25, history);
    }

    TNumber counterSecond = 0;
    for (TNumber n : numbers) {
        counterSecond += count(n, 75, history);
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;

    cout << "First star " << counterFirst << endl;
    cout << "Second star " << counterSecond << endl;
}

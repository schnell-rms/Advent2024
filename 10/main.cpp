#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_set>

using namespace std;

using TCoordinates = std::unordered_map<TNumber, TNumber>;

TNumber key(TNumber i, TNumber j) {
    return (i<<16) | j;
}

void countPaths(const std::vector<std::string> &map, TNumber i, TNumber j, char target, TCoordinates &peaks)
{
    if ((i<0) || (j<0) || (i>=map.size()) || (j>map[i].size()) ||
         (map[i][j] != target)
       ) {
        return;
    }

    if (map[i][j] == '9') {
        peaks[key(i,j)] += 1;
        return;
    }

    countPaths(map,i - 1,j    , target + 1, peaks);
    countPaths(map,i + 1,j    , target + 1, peaks);
    countPaths(map,i    ,j - 1, target + 1, peaks);
    countPaths(map,i    ,j + 1, target + 1, peaks);
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
    std::vector<std::string> map;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            map.push_back(std::move(line));
        }
    }

    TNumber sum = 0;
    TNumber sumRatings = 0;

    for (auto i=0; i<map.size(); ++i) {
        for (auto j=0; j<map[0].size(); ++j) {        
            if (map[i][j] == '0') {
                TCoordinates peaks;
                countPaths(map, i, j, '0', peaks);
                sum += peaks.size();
                for (auto p:peaks) {
                    sumRatings += p.second;
                }
            }
        }
    }

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
    cout << "First is " << sum << endl;
    cout << "Second is " << sumRatings << endl;
}

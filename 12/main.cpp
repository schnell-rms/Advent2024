#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

void measure(std::vector<string> map, std::vector<std::vector<bool>> &traversed, TNumber i, TNumber j, char crop, TNumber &area, TNumber &perimeter) {

    if ((i < 0) || (i>=map.size()) || (j < 0) || (j >= map[0].size()) || (crop != map[i][j])) return;

    if (traversed[i][j]) {
        perimeter--;
        return;
    }

    traversed[i][j] = true;
    area += 1;
    perimeter += 3;

    // Inside the map, on the same crop:
    measure(map, traversed, i - 1, j    , crop, area, perimeter);
    measure(map, traversed, i + 1, j    , crop, area, perimeter);
    measure(map, traversed, i    , j - 1, crop, area, perimeter);
    measure(map, traversed, i    , j + 1, crop, area, perimeter);
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

    std::vector<std::vector<bool>> traversed(map.size(), std::vector<bool>(map[0].size(), false));

    assert(!map.empty());
    const TNumber n = map.size();
    const TNumber m = map[0].size();
    TNumber sum = 0;
    for (TNumber i=0; i<map.size(); i++) {
        for (TNumber j=0; j<map.size(); j++) {
            if (traversed[i][j]) continue;

            TNumber area = 0;
            TNumber perimeter = 1;
            measure(map, traversed, i, j, map[i][j], area, perimeter);
            cout << "Crop " << map[i][j] << " area " << area << " perimeter " << perimeter << endl;
            sum += area * perimeter;
        }
    }

    cout << "First: Total cost is: " << sum << endl;


    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <optional>

using namespace std;


using TKey = std::vector<TNumber>;

bool isFit(const TKey &key, const TKey &lock) {
    assert(key.size() == lock.size());
    bool is = true;
    for (size_t i = 0; i<key.size(); i++) {
        if ((key[i] + lock[i]) > 5) {
            is = false;
            break;
        }
    }
    return is;
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
    std::vector<TKey> keys;
    std::vector<TKey> locks;
    std::optional<bool> isKey; 
    TKey clear(5,0);
    TKey current = clear;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (!isKey.has_value()) {
                isKey = line[0] == '.';
                continue;
            }

            for (size_t i=0; i<line.size(); i++) {
                if (line[i] == '#') {
                    current[i]++;
                }
            }
        } else {
            if (isKey.value()) {
                for (size_t i=0; i<current.size(); i++) {
                    current[i]--;
                }
                keys.push_back(current);
            } else {
                locks.push_back(current);
            }
            current = clear;
            isKey.reset();
        }
    }
    
    if (isKey.has_value()) {
        if (isKey.value()) {
            for (size_t i=0; i<current.size(); i++) {
                current[i]--;
            }
            keys.push_back(current);
        } else {
            locks.push_back(current);
        }
    }

    size_t counter = 0;
    for (size_t k=0; k<keys.size(); k++) {
        for (size_t l=0; l<locks.size(); l++) {
            counter += isFit(keys[k], locks[l]);
        }
    }
    
    cout << "First " << counter << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

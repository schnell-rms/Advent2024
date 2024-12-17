#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>

using namespace std;

enum EInstructions {
    eADV = 0,
    eBXL = 1,
    eBST = 2,
    eJNZ = 3,
    eBXC = 4,
    eOUT = 5,
    eBDV = 6,
    eCDV = 7
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

    clock_t tStart = clock();

    std::string line;
    

    std::vector<TNumber> combos{0,1,2,3};
    
    for (auto i=0; i<3; i++) {
        getline(listFile, line);
        combos.push_back(allNumbers(line)[0]);
    }
    
    TNumber &regA = combos[4];
    TNumber &regB = combos[5];
    TNumber &regC = combos[6];

    combos.push_back(7);

    std::vector<TNumber> program;
    while (getline(listFile, line)) {
        if (!line.empty()) {
            program = allNumbers(line);
            break;
        }
    }

    auto runInstruction=[&](TNumber &i) -> std::string {
        assert(i < program.size() - 1);
        const TNumber combo = combos[program[i+1]];
        const TNumber literal = program[i+1];
        std::string out;

        switch (program[i]) {
            case eADV:
                assert(combo != 7);
                regA = regA >> combo;
                break;
            case eBXL:
                regB = regB ^ literal;
                break;
            case eBST:
                regB = combo % 8;
                break;
            case eJNZ:
                if (0 != regA) {
                    i = literal - 2;
                }
                break;
            case eBXC:
                regB = regB ^ regC;
                break;
            case eOUT:
                out = std::to_string(combo % 8);
                out += ',';
                break;
            case eBDV:
                assert(combo != 7);
                regB = regA >> combo;
                break;
            case eCDV:
                // assert(combo != 7);
                regC = regA >> combo;
                break;
            default:
                assert(false);
        }

        return out;
    };

    // Run program:
    std::string output;
    TNumber i=0;
    while (i < program.size()) {
        output += runInstruction(i);
        i += 2;
    }

    if (!output.empty()) // Remove the last comma
        output.resize(output.size() - 1);

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First star's output: " << output << endl;
}

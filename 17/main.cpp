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

TNumber outputOneCycle(TNumber &A) {
// My input: 2,4,1,3,7,5,4,1,1,3,0,3,5,5,3,0
// Aka 2,4,   1,3,   7,5,  4,1,   1,3,    0,3,   5,5,  3,0
/*
+SO:
+ 2,4:  regB = regA % 8; aka regB = regA & 7
+ 1,3:  regB = regB ^ 3; 
+ 7,5:  regC = regA >> regB;
+ 4,1:  regB = regB ^ regC;
+ 1,3:  regB = regB ^ 3
+ 0,3:  regA = regA >> 3
+ 5,5:  OUT: regB % 8 , aka OUT: regB & 7
+ 3,0:  JUMP to 01 when regA != 0
+*/

    TNumber B = A % 8;
    B = B ^ 3;
    TNumber C = A >> B;
    B = B ^ C;
    B = B ^ 3;
    A = A >> 3;
    return B % 8;
}


TNumber findA(  TNumber A,
                const std::vector<TNumber> &program,
                const std::vector<TNumber> &expected,
                TNumber expectedIdx)
{
    if (expectedIdx < 0) {
        return A;
    }

    auto desired = expected[expectedIdx];
    A <<= 3;
    for (TNumber a=0; a<8; a++) {
        auto backA = A + a;
        if ((backA != 0) && (desired == outputOneCycle(backA))) {
            auto ret = findA(A + a, program, expected, expectedIdx - 1);
            if (ret != -1) {
                return ret;
            }
        }
    }
    return -1;
}


void checkA(TNumber A,
            const std::vector<TNumber> &program)
{
    auto initialA = A;
    std::vector<TNumber> hardoded;
    while (initialA != 0) {
        cout << "For A: " << initialA;
        TNumber out = outputOneCycle(initialA);
        cout << ". Output: " <<  out << endl;
        hardoded.push_back(out);
    }
    cout << "A final: "  << initialA << ". Stop!" << endl;

    cout << "HARDCODED input with A = " << A << endl;
    printVector(hardoded);
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
    

    std::vector<TNumber> combos{0,1,2,3};
    
    for (auto i=0; i<3; i++) {
        getline(listFile, line);
        combos.push_back(allNumbers(line)[0]);
    }
    
    TNumber &regA = combos[4];
    TNumber &regB = combos[5];
    TNumber &regC = combos[6];

    TNumber initialA = regA;

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

    cout << "First time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "First star's output: " << output << endl;

    cout << "Program was";
    printVector(program);


    cout << "First star's output V2: " << endl;
    tStart = clock();
    checkA(initialA, program);
    cout << "First time taken V2: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;


    tStart = clock();
    // auto expected = allNumbers(output);
    // TNumber secondStar = findA(0, program, expected, program.size() - 1);
    TNumber secondStar = findA(0, program, program, program.size() - 1);

    cout << "Second Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout << "Second star's output: " << secondStar << endl;

    checkA(secondStar, program);
}

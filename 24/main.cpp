#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <map>
#include <unordered_map>
#include <unordered_set>

#include <utils.h>
#include <string>
#include <random>
#include <optional>

using namespace std;

using TWireId = TNumber;
using TWires = std::unordered_map<TWireId, std::optional<bool>>;

using TWiresSet = std::unordered_set<TWireId>;

using TEvaluatedWires = std::unordered_set<TWireId>;

using TSwappedGates = std::unordered_set<TWireId>;
TSwappedGates gAllSwappedGates;

std::random_device rd;
std::default_random_engine gGenerator(rd());
std::uniform_int_distribution<TNumber> gDistribution(0,0xFFFFFFFFFFFFFFFF);//Could use 45 biths only: 0xFFFFFFFFFFF

TWireId getId(const std::string &inputName) {
    return (inputName[0] << 16) | (inputName[1] << 8) | (inputName[2]);
}

std::string getName(TWireId id) {
    std::string ret;
    ret += char(id >> 16);
    ret += char((id >> 8) & 0xFF) ;
    ret += char(id & 0xFF);
    assert(getId(ret) == id);
    return ret;
}

enum EOp {
    eAND,
    eOR,
    eXOR,
    eERROR
};

EOp getOp(const std::string &opName) {
    switch (opName[0])
    {
    case 'A':
        return eAND;
    case 'O':
        return eOR;
    case 'X':
        return eXOR;
    default:
        assert(false);
        return eERROR;
    }
}

 std::string opName(EOp op) {
    switch (op)
    {
    case eAND:
        return "AND";
    case eOR:
        return "OR";
    case eXOR:
        return "XOR";
    default:
        assert(false);
        return "ERROR";
    }
}


struct SGate
{
    SGate()
    : in1(0)
    , in2(0)
    , op(eERROR) 
    {}

    SGate(TWireId i1, TWireId i2, EOp e)
    : in1(i1)
    , in2(i2)
    , op(e) 
    {}

    TWireId in1 = 0;
    TWireId in2 = 0;
    EOp op;

    void print(TWireId myId, bool newLine) {
        cout << getName(in1) << ' ' << opName(op) << ' ' << getName(in2) << " -> " << getName(myId);
        newLine && cout << endl;
    }
};

using TGates = std::unordered_map<TWireId, SGate>;
TGates gAllGates;
TWires gAllWires;
TEvaluatedWires gAllEvaluatedWires; // used for cycle detection

std::optional<bool> eval(TWireId id, TWires &wires, TGates &gates, TEvaluatedWires &alreadyChecked) {
    std::string nname = getName(id);
    
    auto it_wire = wires.find(id);
    assert(it_wire != wires.end());
    if (it_wire->second.has_value()) {
        return it_wire->second.value();
    }

    std::optional<bool> out;
    if (alreadyChecked.find(id) == alreadyChecked.end()) {
        alreadyChecked.insert(id);
        
        // search the gate
        auto it_gate = gates.find(id);
        assert(it_gate != gates.end());
        
        const std::optional<bool> in1 = eval(it_gate->second.in1, wires, gates, alreadyChecked);
        const std::optional<bool> in2 = eval(it_gate->second.in2, wires, gates, alreadyChecked);
        
        if (in1.has_value() && in2.has_value()) {
            switch (it_gate->second.op) {
                case eAND:
                    out = in1.value() && in2.value();
                    break;
                case eOR:
                    out = in1.value() || in2.value();
                    break;
                case eXOR:
                    out = in1.value() ^ in2.value();
                    break;
                case eERROR:
                    assert(false);
                    break;
            }
        }
        
        wires[id] = out;
    }
    // else cycle detected
    return out;
}

using TNumberedWires = std::map<TNumber, TWireId>;
TNumberedWires gAllXs, gAllYs, gAllZs;

std::optional<TNumber> evalXYZs(const TNumberedWires &XYZs) {
    int n = 0;
    std::optional<TNumber> out = 0;
    gAllEvaluatedWires.clear();
    for (auto z : XYZs) {
        assert(z.first == n);
        std::optional<bool> bit = eval(z.second, gAllWires, gAllGates, gAllEvaluatedWires);
        if (bit.has_value()) {
            out.value() += (bit.value() ? 1L : 0L) << n;
        } else {
            out.reset();
            break;
        }
        n++;
    }

    return out;
}

std::vector<TNumber> readInput(const std::string& line) {
    const std::string name = line.substr(0,3);
    auto out = line[5] == '1';
    gAllWires[getId(name)] = out;

    TNumber n = (name[1] - '0') * 10 + (name[2] - '0');
    switch (name[0]) {
        case 'y':
            gAllYs[n] = getId(name);
            break;
        case 'x':
            gAllXs[n] = getId(name);
            break;
        default:
            assert(false);
    }

    return {};
};

std::vector<TNumber> readGate(const std::string& line) {
    std::smatch sm;
    std::vector<TNumber> ret;
    std::string::const_iterator searchStart(line.cbegin() );
    if (std::regex_search(searchStart, line.cend(), sm, std::regex("([0-9a-z]{3}) ([A-Z]{2,3}) ([0-9a-z]{3}) -> ([0-9a-z]{3})" ))) {
        const std::string in1 = sm[1];
        const auto op = getOp(sm[2]);
        const std::string in2 = sm[3];

        const std::string out = sm[4];
        if (out[0] == 'z') {
            TNumber n = (out[1] - '0') * 10 + (out[2] - '0');
            gAllZs[n] = getId(out);
        }
        const auto gateId = getId(out);

        // Store the gate:
        gAllWires[gateId] = std::optional<bool>();
        gAllGates[gateId] = SGate(getId(in1), getId(in2), op);
    } else {
        assert(false);
    }

    return ret;
};

bool printBinary(TNumber n) {
    std::string bits;

    do {
        bits += (n & 1) ? '1' : '0';
        bits += ' ';
        n >>= 1;
    }while (n != 0);
    
    std::reverse(bits.begin(), bits.end());
    cout << bits << endl;
    return true;
}

void resetWires(TWires &wires) {
    for (auto &w:wires) {
        w.second.reset();
        assert(!w.second.has_value());
        int k = 0;
    }
}

void setNumberedWires(TWires &allWires, TNumberedWires &wires, TNumber n) {
    TNumber initN = n;
    for (auto &w:wires) {
        allWires[w.second] = (n & 1) ? true : false;
        n >>= 1;
    }
    // Test:
    if (gIS_DEBUG) {
        std::optional<TNumber> checkN = evalXYZs(wires);
        assert(checkN.has_value() && checkN.value() == initN);
    }
}

void gates(TWireId id, TWiresSet &wires) {
    std::string name = getName(id);
    std::string name2 = getName(id);
    auto gate = gAllGates.find(id);
    if (gate != gAllGates.end()) {
        wires.insert(id);
        std::string name3 = getName(id);
        gates(gate->second.in1, wires);
        gates(gate->second.in2, wires);
    }
}

void getAllInputsOfZ(TNumber nz, TWiresSet &wires) {
    gates(gAllZs[nz], wires);
}

bool testSum(TNumber X, TNumber Y, bool verbose = false) {
    resetWires(gAllWires);
    setNumberedWires(gAllWires, gAllXs, X);
    setNumberedWires(gAllWires, gAllYs, Y);
    
    std::optional<TNumber> Z = evalXYZs(gAllZs);

    if (!Z.has_value() || Z.value() != X + Y) {
        verbose && cout << "vvvvvvvvvvvvvvvvvvvvvvvv" << endl;
        verbose && cout <<  "X " << X << " + Y " << Y << " ==? Z " <<
                            (Z.has_value() ? std::to_string(Z.value()) : " loop ") <<
                            endl;
     
        verbose && cout << "Should be " << X + Y << endl;

        verbose && cout << "    Z:  ";
        verbose && Z.has_value() && printBinary(Z.value());
        verbose && cout << "X + Y:  ";
        verbose && printBinary(X + Y);
        // if (Z != X + Y) {
        //     cout << " ERROR  ERROR  ERROR  ERROR " << endl;
        // }
        verbose && cout << "------------------------" << endl;
    }
    return Z.has_value() && (Z.value() == X + Y);
}

bool testGates(TNumber bit) {
    // Test the powers of 2
    for (size_t i = 0; i<=bit; i++) {
        if (!testSum(1L << i, 0, false)) {
            return false;
        }
    }
    
    // Test some random numbers:
    const TNumber mask = (1L << bit) - 1;
        
    for (size_t i = 0; i<20; i++) {
        TNumber X = gDistribution(gGenerator) & mask;
        TNumber Y = gDistribution(gGenerator) & mask;
                
        if (!testSum(X, Y, false)) {
            return false;
        }
    }
    
    return true;
}

bool isSwapped(TWireId id) {
    return gAllSwappedGates.find(id) != gAllSwappedGates.end();
}

void swap(TWireId gateA, TWireId gateB) {
    auto itA = gAllGates.find(gateA);
    assert(itA != gAllGates.end());
    auto itB = gAllGates.find(gateB);
    assert(itB != gAllGates.end());

    auto nameA = getName(gateA);
    auto nameB = getName(gateB);

    
    if (gAllSwappedGates.find(gateA) != gAllSwappedGates.end()) {
        // Remove replacement
        assert(gAllSwappedGates.find(gateB) != gAllSwappedGates.end());
        gAllSwappedGates.erase(gateA);
        gAllSwappedGates.erase(gateB);
    } else {
        // Add replacement
        assert(gAllSwappedGates.find(gateB) == gAllSwappedGates.end());
        gAllSwappedGates.insert(gateA);
        gAllSwappedGates.insert(gateB);
    }
    
    SGate aux = itA->second;
    itA->second = itB->second;
    itB->second = aux;
}

bool solve(TNumber bitId) {

    gIS_DEBUG && cout << "Solve bit " << bitId << endl;
    if (bitId >= gAllXs.size()) {
        return true;
    }

    if (testGates(bitId)) {
        return solve(bitId + 1);
    } else {
        gIS_DEBUG && cout << "Problem on bit " << bitId << endl;
        // Up to this bit, everything ok: the input gates are to be kept as they are:
        TWiresSet toKeep;
        for (size_t i=0; i<bitId; i++) {
            getAllInputsOfZ(i, toKeep);
        }

        // Get the gates of the current Z bit. Eventually will keep those that are not sure to be kept
        TWiresSet toSwap;
        getAllInputsOfZ(bitId, toSwap);
        // Remove the ones not to be kept:
        for (auto c : toKeep) {
            if (toSwap.find(c) != toSwap.end()) {
                toSwap.erase(c);
            }
        }

        // Get all other gates:
        TWiresSet allOther;
        for (auto g : gAllGates) {
            if ((toKeep.find(g.first) == toKeep.end()) &&
                (toSwap.find(g.first) == toSwap.end())) {
                    allOther.insert(g.first);
            }
        }
        
        for (auto toSwapGate : toSwap) {
            if (isSwapped(toSwapGate)) {
                continue;
            }

            for (auto otherGate : allOther) {
                assert(toSwapGate != otherGate);
                if (isSwapped(otherGate)) {
                    continue;
                }
                // Swap the gates
                swap(toSwapGate, otherGate);
                // Test the swap
                const bool allGood = testGates(bitId);
                if (allGood) {
                    gIS_DEBUG && cout   << " Stay on swap "
                                        << getName(toSwapGate) << " <==> " << getName(otherGate) << endl;
                    if (solve(bitId + 1)) {
                        return true;
                    }
                }
                allGood && gIS_DEBUG && cout    << "BACK on " << bitId << " REVERT "
                                                << getName(toSwapGate) << " <==> " << getName(otherGate) << endl;
                swap(toSwapGate, otherGate);
            }
        }
    }

    return false;
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
    bool isInput = true;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            if (isInput) {
                readInput(line);
            } else {
                readGate(line);
            }
        } else {
            isInput = false;
        }
    }

// FIRST
    std::optional<TNumber> Z = evalXYZs(gAllZs);
    assert(Z.has_value());
    cout << "First " << Z.value() << endl;
    
// SECOND
    bool isSuccess = solve(0);
    cout << "Second: " << (isSuccess ? "yes" : "failed") << endl;
    cout << "Replacements " << gAllSwappedGates.size() / 2 << endl;

    std::vector<TWireId> sortedSwaps;
    for (auto r : gAllSwappedGates) {
        sortedSwaps.push_back(r);
    }
    
    std::sort(sortedSwaps.begin(), sortedSwaps.end());
    
    std::string second;
    for (auto r : sortedSwaps) {
        second += getName(r) + ",";
    }
    second.resize(second.empty() ? 0 : (second.size() - 1));
    cout << "Gates " << second << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
}

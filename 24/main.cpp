#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <map>
#include <unordered_map>

#include <utils.h>
#include <string>
#include <optional>

using namespace std;


using TWireId = TNumber;
using TWires = std::unordered_map<TWireId, std::optional<bool>>;

TWireId getId(const std::string &inputName) {
    return (inputName[0] << 16) | (inputName[1] << 8) | (inputName[2]);
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

private:
    std::optional<bool> val;
};



using TGates = std::unordered_map<TWireId, SGate>;
TGates gGate;
TWires gWire; 

bool eval(TWireId id, TWires &wires, TGates &gates) {
    auto it_wire = wires.find(id);
    assert(it_wire != wires.end());

    if (it_wire->second.has_value()) {
        return it_wire->second.value();
    } 

    // search the gate
    auto it_gate = gates.find(id);
    assert(it_gate != gates.end());

    const bool in1 = eval(it_gate->second.in1, wires, gates);
    const bool in2 = eval(it_gate->second.in2, wires, gates);

    bool out;
    switch (it_gate->second.op) {
        case eAND:
            out = in1 && in2;
            break;
        case eOR:
            out = in1 || in2;
            break;
        case eXOR:
            out = in1 ^ in2;
            break;
        case eERROR:
            assert(false);
            break;
    }

    wires[id] = out;        
    return out;
}

std::map<TNumber, TWireId> gAllZs;

TNumber evalZs() {
    int n = 0;
    TNumber out = 0;
    for (auto z : gAllZs) {
        TNumber bit = eval(z.second, gWire, gGate);
        out += (bit << n);
        // cout << "Eval Z" << z.first << " to " << bit << endl;
        n++;
    }
    return out;
}

std::vector<TNumber> readInput(const std::string& line) {
    const std::string name = line.substr(0,3);
    auto out = line[5] == '1';
    gWire[getId(name)] = out;

//    cout << "Input " << name << " is " << out << endl;
    return {};
};

std::vector<TNumber> readGate(const std::string& line) {
    std::smatch sm;
    std::vector<TNumber> ret;
    std::string::const_iterator searchStart(line.cbegin() );
    if (std::regex_search(searchStart, line.cend(), sm, std::regex("([0-9a-z]+) ([A-Z]+) ([0-9a-z]+) -> ([0-9a-z]+)" ))) {
        
        // cout << sm[1] << " ... " << sm[2] << "..." << sm[3] << "..." << sm[4] << endl;

        const auto in1 = sm[1];
        const auto op = getOp(sm[2]);
        const auto in2 = sm[3];

        const std::string out = sm[4];
        if (out[0] == 'z') {
            TNumber n = (out[1] - '0') * 10 + (out[2] - '0');
            gAllZs[n] = getId(out);
        }
        const auto gateId = getId(out);

        gWire[gateId] = std::optional<bool>();
        gGate[gateId] = SGate(getId(in1), getId(in2), op);
    } else {
        assert(false);
    }

    return ret;
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

    TNumber first = evalZs();
    cout << "First " << first << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_set>

using namespace std;

TNumber key(const std::string &name) {
    return (name[0] << 8) | name[1];
}

struct SComp {
    std::string name;
    std::unordered_set<std::string> conns;

    void addConn(const std::string &s) {
        conns.insert(s);
    }

    TNumber nbConns() const { return conns.size(); }
    
    bool isConnectedWith(const std::string& other) const {
        return conns.find(other) != conns.end(); 
    }

    bool hasT() const {
        if (name[0] == 't') {
            return true;
        }
        for (auto c: conns) {
            if (c[0] == 't') {
                return true;
            }
        }
        return false;
    }
};

using TLAN = std::unordered_map<TNumber, SComp>;

void addConnection(TLAN &lan, SComp c, std::string c2name) {
    auto it = lan.find(key(c.name));
    if (it == lan.end()) {
        lan[key(c.name)] = c;
    } else {
        it->second.addConn(c2name);
    }
} 

bool isConnected(const TLAN &lan, std::string &c1, std::string &c2) {
    auto it = lan.find(key(c1));
    assert(it != lan.end());
    
    return it->second.isConnectedWith(c2);
}

TNumber countLans(const TLAN &lan) {
    TNumber counter = 0;
    for (auto c1 : lan) {
        for (auto c2: c1.second.conns) {
            for (auto c3: c1.second.conns) {
                if (c2 == c3) {
                    continue;
                }
                
                // not efficient here:
                const bool hasT = (c1.second.name[0] == 't') || c2[0] == 't' || c3[0] == 't';
                const bool isLan = hasT && isConnected(lan, c2, c3);
                counter += isLan;
                if (isLan) {
                    cout << c1.second.name << "-" << c2 << "-" << c3 << endl;
                }
            }
        }
    }
    
    assert(counter % 6 == 0);
    return counter / 6;
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
    TLAN lan;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            SComp c1, c2;
            c1.name += line[0];
            c1.name += line[1];

            c2.name += line[3];
            c2.name += line[4];

            c1.addConn(c2.name);
            c2.addConn(c1.name);

            addConnection(lan, c1, c2.name);
            addConnection(lan, c2, c1.name);
        }
    }

    TNumber first = countLans(lan);

    cout << "First " << first << endl;

    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  
}

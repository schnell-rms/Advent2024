#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_set>

using namespace std;

using TNodeID = TNumber;

TNumber key(const std::string &name) {
    return (name[0] << 8) | name[1];
}

std::string revKey(TNodeID nodeId) {
    std::string s;
    s += (nodeId >> 8);
    s += nodeId & 0xFF;
    return s;
}

bool hasT(TNodeID nodeId) {
    return (nodeId >> 8) == 't';
}

struct SComp {
    std::string name;
    std::unordered_set<TNodeID> conns;

    void addConn(const std::string &s) {
        conns.insert(key(s));
    }

    TNumber nbConns() const { return conns.size(); }
    
    bool isConnectedWith(const TNodeID other) const {
        return conns.find(other) != conns.end(); 
    }
};

using TLAN = std::unordered_map<TNumber, SComp>;

SComp& getNode(TLAN &lan, std::string &name) {
    auto it = lan.find(key(name));
    if (it == lan.end()) {
        SComp node;
        node.name = name;
        lan[key(name)] = node;
        
        it = lan.find(key(name));
    }
    
    return it->second;
}

bool isConnected(const TLAN &lan, TNodeID id1, TNodeID id2) {
    auto it = lan.find(id1);
    assert(it != lan.end());
    
    return it->second.isConnectedWith(id2);
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
                const bool isT = (c1.second.name[0] == 't') || hasT(c2) || hasT(c3);
                const bool isLan = isT && isConnected(lan, c2, c3);
                counter += isLan;
                gIS_DEBUG && isLan && cout << c1.second.name << "-" << c2 << "-" << c3 << endl;
            }
        }
    }
    
    assert(counter % 6 == 0);
    return counter / 6;
}

std::string getPass(const std::unordered_set<TNodeID> &pp) {
    std::vector<TNumber> vv;
    for (auto p : pp) {
        vv.push_back(p);
    }

    std::sort(vv.begin(), vv.end());
    std::string s;
    for (auto v :vv) {
        s += revKey(v) + ',';
    }
    s.resize(s.size() - 1);
    return s;
}

void bronKerbosch(
    unordered_set<TNodeID>&& R,
    unordered_set<TNodeID>&& nodeIDs,
    unordered_set<TNodeID>&& X,
    TLAN &lan,
    vector<unordered_set<TNodeID> >& cliques)
{
    if (nodeIDs.empty() && X.empty()) {
        cliques.push_back(R);
        return;
    }

    while (!nodeIDs.empty()) {
        TNodeID v = *nodeIDs.begin();
        unordered_set<TNodeID> newR = R;
        newR.insert(v);
        unordered_set<TNodeID> newP;
        for (TNodeID p : nodeIDs) {
            if (lan[v].isConnectedWith(p)) {
                newP.insert(p);
            }
        }
        unordered_set<TNodeID> newX;
        for (TNodeID x : X) {
            if (lan[v].isConnectedWith(x)) {
                newX.insert(x);
            }
        }
        bronKerbosch(std::move(newR), std::move(newP), std::move(newX),
                     lan, cliques);
        nodeIDs.erase(v);
        X.insert(v);
    }
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

    vector<vector<TNodeID> > edges;
    while(getline(listFile, line)) {
        if (!line.empty()) {
            std::string name1, name2;
            name1 += line[0];
            name1 += line[1];

            name2 += line[3];
            name2 += line[4];

            SComp& c1 = getNode(lan, name1);
            SComp& c2 = getNode(lan, name2);
            
            c1.addConn(c2.name);
            c2.addConn(c1.name);

            edges.push_back({key(c1.name), key(c2.name)});
        }
    }

    TNumber first = countLans(lan);

    cout << "First " << first << endl;

// SECOND:    

    // Copy all nodes IDs:
    unordered_set<TNodeID> nodeIDs;
    for (const auto& node : lan) {
        nodeIDs.insert(node.first);
    }

    std::vector<unordered_set<TNodeID> > allCliques;
    bronKerbosch({},std::move(nodeIDs), {}, lan, allCliques);

    TNumber second = 0;
    std::string secondOut = "";
    for (const auto& clique : allCliques) {
        if (second < clique.size()) {
            second = clique.size();
            secondOut = getPass(clique);
        }
    }

    cout << "Second " << second << "nodes. OUT: " << secondOut << endl;
    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl; 
}

#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <cstdint>
#include <random>
#include <iomanip>
#include <utility>
#include <map>

#define debug 0

extern "C" double _tanh(double); // target function

const int seed = 555;
std::default_random_engine gen{seed};
// std::default_random_engine gen{std::random_device()};

double randNum(double _min, double _max) {
    std::uniform_int_distribution<> dist_sign(0, 1);  // 0 or 1
    std::uniform_real_distribution<double> dist(_min, _max);
    double val = dist(gen);
    return val * (dist_sign(gen) == 0 ? 1.0 : -1.0);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;
}

int eraseSpecial(std::string str) {
    std::vector<char> ch = {',', '%', ':'};
    str.erase(std::remove_if(str.begin(), str.end(),
        [&](char c) {
            return std::find(ch.begin(), ch.end(), c) != ch.end();
        }),
        str.end());
    return std::stoi(str);
}

void initBB(std::string& path, std::vector<int>& BNums, std::map<int, std::vector<int>>& BB) {
    std::ifstream ifs(path);
    std::string line;

    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        bool valid = false;
        int condCount = 0;
        while (std::getline(ifs, line)) {
            if (line.find("define") != std::string::npos) { 
                valid = true;
            } else if (line.find("ret ") != std::string::npos) {
                valid = false;
            }
            if (valid && (line.find(":") != std::string::npos)) {
                auto tokens = split(line, ':');
                int BNum = std::stoi(tokens[0]);
                BNums.push_back(BNum);
            }
            if (line.find("icmp") != std::string::npos) {
                condCount += 1;
            }
        }
        ifs.close();

        std::vector<int> initvec(condCount, -1); // -1 : not related
        for (int i=0; i<BNums.size(); i++) {
            BB[BNums[i]] = initvec;
        }

    }
}

void getCondition(std::string& path, std::vector<std::pair<int, std::pair<std::string, double>>>& cond, std::map<int, std::vector<int>>& BB) {
    std::ifstream ifs(path);
    std::string line;
    int cmpIdx = 0;

    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        while (std::getline(ifs, line)) {
            int curNode, predNode;
            // override
            if (line.find("; preds =") != std::string::npos) {
                auto tokens = split(line, ';');
                curNode = eraseSpecial(tokens[0]);

                tokens = split(line, '=');
                predNode = eraseSpecial(tokens[1]);
                if (predNode == 1) continue; // skip
                if (tokens.size() > 2) continue; // TODO: phi node
                
                for (int i=0; i<cmpIdx-1; i++) {
                    BB[curNode][i] = BB[predNode][i];
                }
            }
            // get condition
            if (line.find("icmp") != std::string::npos) {
                auto tokens = split(line, ' ');
                std::string tag = tokens[5]; // ult ugt ..
                std::string _intval = tokens[8]; // intermediate

                /* TODO
                 * set variable uint32_t or int32_t by tag
                */

                uint32_t intval = static_cast<uint32_t>(std::stoul(_intval));

                double dval;
                uint64_t val = (uint64_t(intval) << 32); // Shift to high 32 bits
                std::memcpy(&dval, &val, sizeof(dval));
                // std::cout << "curNode = " << curNode << " {" << tag << ", " << dval << "}\n";
                cond.push_back({curNode, {tag, dval}});
            }
            // conditional branch
            if (line.find("br") != std::string::npos) {
                auto tokens = split(line, ' ');
                if (tokens.size() < 8) continue; // filter unconditional branch

                int truePath = eraseSpecial(tokens[6]);
                int falsePath = eraseSpecial(tokens[8]);

                BB[truePath][cmpIdx] = 1; // true
                BB[falsePath][cmpIdx] = 0; // false

                cmpIdx++;
            }
        }
    }
}

void findRange(std::map<int, std::vector<int>>& BB, std::vector<std::pair<int, std::pair<std::string, double>>>& cond, int node, std::pair<double, double>& rangeNode) {
    double _min=1e-9, _max=1e+6;

    for (int i=0; i<cond.size(); i++) {
        if (BB[node][i] != -1) {
            std::string tag = cond[i].second.first;
            double ival = cond[i].second.second;

            if (tag.find("g") != std::string::npos) {
                if (BB[node][i] == 1) _min = ival > _min ? ival : _min;
                else _max = ival < _max ? ival : _max;
            } 
            if (tag.find("l") != std::string::npos) {
                if (BB[node][i] == 1) _max = ival < _max ? ival : _max;
                else _min = ival > _min ? ival : _min;
            }
        }
    }

    rangeNode = {_min, _max};
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./run <arg1> <arg2> <arg3>\n";
        return 1;
    }

    // std::string path = "./_tanh_UnreachablePath.ll";
    std::string path = "./" + std::string(argv[3]) + ".ll";

    // 0. get basic information of basic blocks
    // std::cout << "[*] Get basic information of basic-blocks\n";
    std::vector<int> BNums;
    std::map<int, std::vector<int>> BB; // BBNum, conditions ({-1, 0, 1})
    initBB(path, BNums, BB);

    // 1. get condition for left/right path for a given .ll file
    // std::cout << "[*] Get conditional branches' condition\n";
    std::vector<std::pair<int, std::pair<std::string, double>>> cond;
    getCondition(path, cond, BB);

    #if debug
        for (int i=0; i<BNums.size(); i++) {
            std::cout << BNums[i] << ": ";
            for (int j=0; j<BB[BNums[i]].size(); j++) {
                std::cout << BB[BNums[i]][j] << ", ";
            }
            std::cout << "\n";
        }

        for (int i=0; i<cond.size(); i++) {
            std::cout << "curNode = " << cond[i].first << " {" << cond[i].second.first << ", " << cond[i].second.second << "}\n";
        }
    #endif

    // 2. detect two split path
    // std::cout << "[*] Detect parent node and its two successor path\n";
    int nodeA, nodeB;
    // same T/Fs but different 0/1 at a certain branch
    for (int i=1; i<BNums.size(); i++) {
        nodeA = BNums[i-1];
        nodeB = BNums[i];

        bool flag = false;
        for (int j=0; j<BB[BNums[i]].size(); j++) {
            if (BB[nodeA][j] != BB[nodeB][j]) {
                if (BB[nodeA][j] + BB[nodeB][j] == 1) {// 1/0 or 0/1
                    flag = true;
                    break;
                }
            }
        }
        if (flag) break;
    }

    #if debug
        std::cout << "nodeA = " << nodeA << "\n";
        std::cout << "nodeB = " << nodeB << "\n";
    #endif

    // 3. find valid input range for two path
    // std::cout << "[*] Find valid input range for two path\n";
    std::pair<double, double> rangeNodeA, rangeNodeB; // min, max
    findRange(BB, cond, nodeA, rangeNodeA);
    findRange(BB, cond, nodeB, rangeNodeB);
    
    #if debug
        std::cout << "[*] nodeA\n";
        std::cout << "_min = " << rangeNodeA.first << ", _max = " << rangeNodeA.second << "\n"; 
        std::cout << "[*] nodeB\n";
        std::cout << "_min = " << rangeNodeB.first << ", _max = " << rangeNodeB.second << "\n"; 
    #endif

    // 4. get random sample values for the input range
    int cnt = 100;
    double _min, _max;
    std::vector<double> res(cnt, 0.0);
    if (*argv[1] == 'T') {
        // select input range for True path
        _min = rangeNodeA.first;
        _max = rangeNodeA.second;
    } else if (*argv[1] == 'F') {
        // select input range for True path
        _min = rangeNodeB.first;
        _max = rangeNodeB.second;
    }

    std::string filename = std::string(argv[1])+std::string(argv[2])+".txt";
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Failed to open file.\n";
        return 0;
    } else {
        ofs << std::setprecision(17);
        for (int i=0; i<cnt; i++) {
            double x = randNum(_min, _max);
            res[i] = _tanh(x);
            ofs << res[i] << "\n";
        }
    }

    return 0;
}
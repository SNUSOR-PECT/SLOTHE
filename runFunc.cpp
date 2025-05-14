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
#include <utility>
#include <map>

extern "C" double _tanh(double); // target function

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
    std::vector<char> ch = {',', '%'};
    str.erase(std::remove_if(str.begin(), str.end(),
        [&](char c) {
            return std::find(ch.begin(), ch.end(), c) != ch.end();
        }),
        str.end());
    return std::stoi(str);
}

void initBB(std::string path, std::map<int, std::vector<int>>& BB) {
    std::ifstream ifs(path);
    std::string line;

    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        bool valid = false;
        int condCount = 0;
        std::vector<int> BNums; // size = #cond
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

void getCondition(std::string path, std::vector<std::pair<std::string, int>>& cond, std::map<int, std::vector<int>> BB) {
    std::ifstream ifs(path);
    std::string line;
    int curblocknum = 0;

    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        while (std::getline(ifs, line)) {
            // get condition
            if (line.find("icmp") != std::string::npos) {
                auto tokens = split(line, ' ');
                std::string tag = tokens[5]; // ult ugt ..
                std::string _intval = tokens[8]; // intermediate
                uint32_t intval = static_cast<uint32_t>(std::stoul(_intval));

                double dval;
                uint64_t val = (uint64_t(intval) << 32); // Shift to high 32 bits
                std::memcpy(&dval, &val, sizeof(dval));
                std::cout << tag << ", dval = " << dval << "\n";
                cond.push_back({tag, dval});
            }
            // conditional branch
            if (line.find("br") != std::string::npos) {
                auto tokens = split(line, ' ');
                if (tokens.size() < 8) continue; // filter unconditional branch

                int truePath = eraseSpecial(tokens[6]);
                int falsePath = eraseSpecial(tokens[8]);

                std::cout << truePath << "\n" << falsePath << "\n";
            }
        }
    }
}

int main(void) {
    std::string path = "./_tanh_UnreachablePath.ll";

    // 0. get the number of basic blocks
    std::map<int, std::vector<int>> BB; // BBNum, conditions {-1, 0, 1}
    initBB(path, BB);

    // 1. get condition for left/right path for a given .ll file
    std::vector<std::pair<std::string, int>> cond;
    getCondition(path, cond, BB);

    // 2. 

    // double x = 1.0;
    // double y = _tanh(x);
    // printf("tanh(%f) = %f\n", x, y);
    // printf("Answer tanh(%f) = %f\n", x, tanh(x));

    return 0;
}
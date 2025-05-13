#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <cstdint>

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

void getCondition(std::ifstream &ifs) {
    std::string line;
    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        while (std::getline(ifs, line)) {
            if (line.find("icmp") != std::string::npos) {
                auto tokens = split(line, ' ');
                std::string tag = tokens[5]; // ult ugt ..
                std::string _intval = tokens[8]; // intermediate ..
                uint32_t intval = static_cast<uint32_t>(std::stoul(_intval));

                double dval;
                uint64_t val = (uint64_t(intval) << 32); // Shift to high 32 bits
                std::memcpy(&dval, &val, sizeof(dval));
                std::cout << tag << ", dval = " << dval << "\n";
            }
        }
    }
}

int main(void) {
    // 1. get condition for left/right path for a given .ll file
    std::ifstream ifs("./_tanh_UnreachablePath.ll");
    getCondition(ifs);

    // double x = 1.0;
    // double y = _tanh(x);
    // printf("tanh(%f) = %f\n", x, y);
    // printf("Answer tanh(%f) = %f\n", x, tanh(x));

    return 0;
}
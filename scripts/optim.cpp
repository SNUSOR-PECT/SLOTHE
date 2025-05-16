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

/*
 * TODO : Automatize detection
*/

void eraseSpecial(std::string& str, char ch) {
    str.erase(std::remove(str.begin(), str.end(), ch), str.end());
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./run <arg1> \n";
        return 1;
    }
    std::string path = "./temp/" + std::string(argv[1]) + ".ll";
    std::string dst = "./temp/" + std::string(argv[2]) + ".ll";

    std::ifstream ifs(path);
    std::ofstream ofs(dst);
    std::string line;

    if (!ifs) {
        std::cerr << "Failed to open file.\n";
    } else {
        while (std::getline(ifs, line)) {
            if (line.find("%4 = ") != std::string::npos) {
                line.replace(line.find("%3"), 2, "%0");
                eraseSpecial(line, '-');
            }
            if (line.find("%8 = ") != std::string::npos) {
                line.replace(line.find("%6"), 2, "%5");
            }
            if (line.find("double %11") != std::string::npos) {
                std::string str = "double %11";
                line.replace(line.find(str), str.length(), "double %8");
            }
            ofs << line << "\n";
        }
    }

    ifs.close();
    ofs.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <path> <HEparam>\n", argv[0]);
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;
    double threshold = pow(10,ceil(log10(pow(2,-std::atoi(argv[2]))) -2));

    // std::cout << "threshold = " << threshold << "\n";

    while (std::getline(infile, line)) {
        std::size_t pos = line.find("double u =");
        if (pos != std::string::npos) {
            std::string valueStr = line.substr(pos + 11);
            std::istringstream iss(valueStr);
            double value;
            if (iss >> value) {
                if (std::abs(value) > threshold) {
                    std::cout << "1\n";
                    // std::cout << "KEEP: (" << value << " < " << threshold << ")\n";
                } else {
                    std::cout << "0\n";
                    // std::cout << "SKIP: (" << value << " < " << threshold << ")\n";
                }
            } else {
                std::cerr << "Failed to parse value in line: " << line << "\n";
            }
        }
    }

    return 0;
}
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <bitset>

int main() {
    std::cout << "Enter your input: ";
    std::string input;
    std::getline(std::cin, input); // 전체 입력 라인을 읽음

    std::istringstream iss(input);
    std::vector<std::string> parts;
    std::string part;
    while (iss >> part) {
        parts.push_back(part);
    }

    if (parts.size() != 4) {
        std::cerr << "Invalid input format!" << std::endl;
        return 1;
    }

    //check operation
    if (parts[0] == "ugt") {
        std::cout << "unsigned greater than" << std::endl;
    } else if (parts[0] == "sgt"){
        std::cout << "signed greater than" << std::endl;
    } else if (parts[0] == "ult"){
        std::cout << "unsigned less than" << std::endl;
    } else if (parts[0] == "slt"){
        std::cout << "signed less than" << std::endl;
    } else {
        std::cout << "Operation not recognized." << std::endl;
    }

    //check data format
    // if (parts[1] == "i32") {
    //     std::cout << "32bit" << std::endl;
    // } else if (parts[1] == "i64") {
    //     std::cout << "64bit" << std::endl;
    // }

    uint64_t ans;

    ans = stoull(parts[3]);

    //change lx for low word
    uint lx = 0x00000000;

    //change ans for high word
    uint hx = ans & 0x7fffffff;
    bool sign = 1;
    double temp;

    //from binary format, create exponent and mantissa
    int exponent = hx >> 20;
    uint mantissa = hx & 0x000fffff;

    //check exp & mantissa by binary
    std::cout << "exponent : "<< std::bitset<11>(exponent).to_string() << "\n";
    std::cout << "mantissa : "<< std::bitset<32>(mantissa).to_string() << "\n" << "\n";

    std::cout << "x value is : ";

    //check if Inf
    if (exponent == 2046) {
        if (lx == 0) std::cout << "Inf" << "\n";
        else std::cout << "NaN" << "\n";
    }

    //check if NaN
    else if (exponent == 2047) {
        std::cout << "NaN" << "\n";
    }

    else {
        temp = sign * (1+mantissa*pow(2,-20)) * pow(2,exponent-1023);
        std::cout << temp << std::endl;
    }


    //copy  ugt i32 %6, 2146435071      <- inf
    //      sgt i64 %2, -1              <- T : inf / F : NaN
    //      ult i32 %6, 1077280768      <- 22       0x40360000
    //      ult i32 %6, 1015021568      <- 2^-55    <<- exponent is overflowed...
    //      ugt i32 %6, 1072693247      <- 1

}

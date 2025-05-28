#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// unit : us
// PAdd CAdd PMult(+Rescale) CMult(+Rescale+Relin) Btp
typedef struct _Perf {
	int btp;
	std::vector<std::vector<int>> opTime; // HE operations
} Perf;

std::vector<int> split(std::string _in, char delimiter);
void readOpTime(std::string path, Perf& p);

std::vector<int> split(std::string _in, char delimiter) {
	std::vector<int> res;
	std::stringstream ss(_in);
	std::string tmp;

	while (std::getline(ss, tmp, delimiter)) {
		res.push_back(stoi(tmp));		
	}
	return res;
}

void readOpTime(std::string path, Perf& p) {
	std::ifstream f(path);
	if (!f.is_open()) {
		throw std::invalid_argument("cannot open "+ path + "\n");
	}

    // read btp time
    std::string val, line;
    std::getline(f, val);
    p.btp = stoi(val);

	// read measured time line by line
	while(std::getline(f, line)) {
		std::vector<int> _time;
		_time = split(line, ',');
		if (_time.size()>0)
			p.opTime.push_back(_time);
	}

}
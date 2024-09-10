#include <cmath>
#include <vector>
#include <cstdint>
#include <iostream>

#include "main.hpp"

using namespace std;

int main(void) {
    uint32_t n = 32768;
    vector<double> x(n);
    for(int i=0; i<n; i++) x[i] = -8.0 + 15.9 * static_cast<double>(i+1) / static_cast<double>(n);

    {
        double avg_err = 0.0, max_err = -1.0;
        for (int i=0; i<n; i++) {
            double e = exp_inv_9(x[i]); // f = exp(x/8)
            e = pow(e, 8); // {exp(x/8)} ^8

            double _exp = exp(x[i]); // expected val

            double err = fabs(_exp - e);
            avg_err += err;
            max_err = max_err > err ? max_err : err;
        }

        std::cout << std::scientific << "avg_err = " << avg_err / static_cast<double>(n) << ", max_err = " << max_err << "\n";
    }

    // {
    //     double avg_err = 0.0, max_err = -1.0;
    //     for (int i=0; i<n; i++) {
    //         double e = _tanh(x[i]); // using inner approximated function
    //         double _tanh = tanh(x[i]); // expected val

    //         double err = fabs(_tanh - e);
    //         avg_err += err;
    //         max_err = max_err > err ? max_err : err;
    //     }

    //     std::cout << std::scientific << "avg_err = " << avg_err / static_cast<double>(n) << ", max_err = " << max_err << "\n";
    // }

    // {
    //     double avg_err = 0.0, max_err = -1.0;
    //     for (int i=0; i<n; i++) {
    //         double e = tanh_15(x[i]); // using inner approximated function
    //         double _tanh = tanh(x[i]); // expected val

    //         double err = fabs(_tanh - e);
    //         avg_err += err;
    //         max_err = max_err > err ? max_err : err;
    //     }

    //     std::cout << std::scientific << "avg_err = " << avg_err / static_cast<double>(n) << ", max_err = " << max_err << "\n";
    // }
}
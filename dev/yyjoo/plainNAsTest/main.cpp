#include "main.hpp"

int main(void) {
    double alpha = 13;
    double epsilon = std::pow(2, -alpha);
    std::vector<double> K;
    getRelaxationFactor(f_inv, alpha, epsilon, K);

    long n = 2; //32768;
    std::vector<double> m_x(n), r_x(n);
    for(int i=0; i<n; i++) m_x[i] = 0.05 + 0.95 * static_cast<double>(i+1) / static_cast<double>(n);

    double diff = 0.0;
    for (int i=0; i<n; i++) {
        r_x[i] = Inverse(alpha, epsilon, K, m_x[i]);
        diff += std::fabs(1/m_x[i] - r_x[i]);
        std::cout << "m_x = " << m_x[i] << ", r_x = " << r_x[i] << ", (expected) = " << 1/m_x[i] << "\n";
    }
    diff /= (double)n;
    
    std::cout << "avg error = " << diff << "\n";

    return 0;

}
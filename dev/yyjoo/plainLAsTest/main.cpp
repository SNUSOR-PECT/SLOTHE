#include "main.hpp"

int main(void) {
    double alpha = 13;
    double epsilon = std::pow(2, -alpha);

    long n = 32768;
    std::vector<double> m_x(n), r_x(n);
    for(int i=0; i<n; i++) m_x[i] = -4.0 + 8.0 * static_cast<double>(i+1) / static_cast<double>(n+1);
    // for(int i=0; i<n; i++) m_x[i] = -8.0 + 16.0 * static_cast<double>(i+1) / static_cast<double>(n+1);

    double avg_err = 0.0;
    double max_err = -1.0;
    for (int i=0; i<n; i++) {
        double ans = tanh(m_x[i]);
        r_x[i] = _Tanh(m_x[i]);
        double diff = std::fabs(ans - r_x[i]);

        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
        // std::cout << "m_x = " << m_x[i] << ", r_x = " << r_x[i] << ", (expected) = " << ans << "\n";

    }

    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;

    return 0;

}
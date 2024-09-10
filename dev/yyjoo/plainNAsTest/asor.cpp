#include "asor.hpp"

void getRelaxationFactor(std::function<double(double)> f, double alpha, double epsilon, std::vector<double>& K) {
    std::vector<double> eps(1, epsilon);  // Initialize eps with epsilon
    K.clear();  // Clear K in case it's used elsewhere

    size_t i = 0;
    double k = 0.0;

    while (1 - eps[i] > pow(2, -alpha)) {
        double max_min = -1.0;
        double best_k = 0.0;

        // Loop over potential values of k_i (range might need to be adjusted)
        for (double cand_k = 0.0; cand_k <= maxIter; cand_k += 0.01) {
            double min_val = std::min(f(cand_k * eps[i]), f(cand_k));
            if (min_val > max_min) {
                max_min = min_val;
                best_k = cand_k;
            }
        }

        k = best_k;
        K.push_back(k);
        eps.push_back(f(k));  // Add the new epsilon value
        i++;
    }
}

double Inverse(double alpha, double epsilon, std::vector<double>& K, double x) {
    // Step 2: Initialize variables
    double epsilon_i = epsilon;
    size_t i = 0;  // Start with i = 0
    double a = x;
    double b = 1.0;

    // Ensure K has elements to access
    if (K.empty()) {
        std::cerr << "Error: Relaxation factors vector K is empty." << std::endl;
        return 0.0;
    }

    // Step 6: Iteratively compute the inverse
    while (1 - epsilon_i > std::pow(2, -alpha)) {
        // Check if i is within the bounds of K
        if (i >= K.size()) {
            std::cerr << "Error: Index i out of bounds for vector K." << std::endl;
            break;
        }

        // Step 7: Get k_i (i-th element of K)
        double k_i = K[i];

        // Step 8: Update b and a
        b = k_i * b * (2 - k_i * a);
        a = k_i * a * (2 - k_i * a);

        // Step 10: Update epsilon_i
        epsilon_i = k_i * (2 - k_i);

        // Step 11: Increment i
        i++;
    }

    // Step 13: Return the approximate value of 1/x (which is b)
    return b;
}
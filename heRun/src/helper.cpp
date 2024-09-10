#include "helper.hpp"

void Timer::start(void) {
    t_start = high_resolution_clock::now();
}
void Timer::end(void) {
    t_end = high_resolution_clock::now();
    t_diff = duration_cast<milliseconds>(t_end - t_start);

    std::cout << "[*] time : " << t_diff.count() << " ms" << std::endl;
}

long ShowFailure_Inverse(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    for (int i = 0; i < n; ++i) {
            if(abs(1/x[i] - output[i]) > bound) failure++;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}
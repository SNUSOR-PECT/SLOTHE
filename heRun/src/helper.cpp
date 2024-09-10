#include "helper.hpp"

void Timer::start(void) {
    t_start = high_resolution_clock::now();
    t_starts.push(t_start);
}
void Timer::end(void) {
    t_end = high_resolution_clock::now();
    t_start = t_starts.top();
    t_diff = duration_cast<milliseconds>(t_end - t_start);

    t_starts.pop();

    std::cout << "[*] time : " << t_diff.count() << " ms" << std::endl;
}

long ShowFailure_Exp(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(exp(x[i]) - output[i]) > bound) failure++;
            max_err = max_err > abs(exp(x[i]) - output[i]) ? max_err : abs(exp(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

long ShowFailure_Expm1(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(expm1(x[i]) - output[i]) > bound) failure++;
            max_err = max_err > abs(expm1(x[i]) - output[i]) ? max_err : abs(expm1(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

long ShowFailure_Inverse(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(1/x[i] - output[i]) > bound) failure++;
            max_err = max_err > abs(exp(x[i]) - output[i]) ? max_err : abs(exp(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

void decrypt_and_print_part(const Ciphertext& ctxt_in, Decryptor& decryptor, CKKSEncoder& encoder, long sparse_slots, size_t start, size_t end) {
    Plaintext ptxt;
    decryptor.decrypt(ctxt_in, ptxt);

    std::vector<double> rtn_vec;
    encoder.decode(ptxt, rtn_vec);

    std::cout << "( "; 
    std::cout << "... ";
    for (size_t i = start; i <= end; i++) std::cout << rtn_vec[i] << ", ";
    std::cout << "... ";
    for (size_t i = start; i < end; i++) std::cout << rtn_vec[sparse_slots - end + i] << ", ";
    std::cout << ")" << std::endl;
    std::cout << "scale: " << ctxt_in.scale() << std::endl;
}
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

void checkPrecision(long n, long log_modulus, SEALContext& context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks) {
    Plaintext ptxt;
    Ciphertext ctxt;

    double scale = std::pow(2.0, log_modulus);
    std::vector<double> decoded, decrypted;

    double prec = 1.0;
    double precEncode, precEncrypt;
    bool validEncode = true, validEncrypt = true;
    while (true) {
        std::vector<double> vec(n, prec);
        encoder.encode(vec, scale, ptxt);
        encryptor.encrypt(ptxt, ctxt);

        // 1. Encode -> Decode
        encoder.decode(ptxt, decoded);

        // 2. Encrypt -> Decrypt -> Decode
        decryptor.decrypt(ctxt, ptxt);
        encoder.decode(ptxt, decrypted);

        // check precision
        validEncode = (abs(prec - decoded[0]) < prec);
        validEncrypt = (abs(prec - decrypted[0]) < prec);

        if ((!validEncode && !validEncrypt)) break;
        if (validEncode) precEncode = prec;
        if (validEncrypt) precEncrypt = prec;

        prec /= 10.0;
    }

    std::cout << "\nUsing scalingfactor " << log_modulus << ",\n";
    std::cout << "Encode precision bound : " << precEncode << "\n";
    std::cout << "Encrypt precision bound : " << precEncrypt << "\n\n";
}

long ShowFailure_Tanh(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
        if(abs(tanh(x[i]) - output[i]) > bound) failure++;
        max_err = max_err > abs(tanh(x[i]) - output[i]) ? max_err : abs(tanh(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

long ShowFailure_Sigmoid(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(Sigmoid(x[i]) - output[i]) > bound) failure++;
            max_err = max_err > abs(Sigmoid(x[i]) - output[i]) ? max_err : abs(Sigmoid(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

long ShowFailure_GeLU(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(GeLU(x[i]) - output[i]) > bound) failure++;
            max_err = max_err > abs(GeLU(x[i]) - output[i]) ? max_err : abs(GeLU(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

long ShowFailure_Softplus(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    for (int i = 0; i < n; ++i) {
            if(abs(Softplus(x[i]) - output[i]) > bound) failure++;
            max_err = max_err > abs(Softplus(x[i]) - output[i]) ? max_err : abs(Softplus(x[i]) - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
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
        max_err = max_err > abs(1/x[i] - output[i]) ? max_err : abs(1/x[i] - output[i]);
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    return failure;
}

void print_part(const std::vector<double>& m_x, long sparse_slots, size_t start, size_t end) {
    std::cout << "( "; 
    std::cout << "... ";
    for (size_t i = start; i <= end; i++) std::cout << m_x[i] << ", ";
    std::cout << "... ";
    for (size_t i = start; i < end; i++) std::cout << m_x[sparse_slots - end + i] << ", ";
    std::cout << ")" << std::endl;
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
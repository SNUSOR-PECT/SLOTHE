#include "helper.hpp"

void Timer::start(void) {
    t_start = high_resolution_clock::now();
    t_starts.push(t_start);
}
void Timer::end(void) {
    t_end = high_resolution_clock::now();
    t_start = t_starts.top();
    t_diffs.push_back(duration_cast<milliseconds>(t_end - t_start)); cnt += 1;

    t_starts.pop();
}
void Timer::calAvg(void) {
    int sum = 0;
    for (int i=0; i<cnt; i++) sum += t_diffs[i].count();
    std::cout << "[*] time : " << sum / (double)cnt << " ms\n";

    t_diffs.clear();
    cnt = 0;
}

void checkPrecision(long n, long log_modulus, SEALContext& context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks) {
    Plaintext ptxt;
    Ciphertext ctxt;

    double scale = std::pow(2.0, log_modulus);
    std::vector<double> decoded, decrypted, multiplied;

    std::vector<double> ones(n, 1.0);
    double precEncode, precEncrypt, precMult;
    // double precEncodeBound, precEncryptBound, precMultBound;

    std::vector<double> vec(n, 0.123456789);
    encoder.encode(vec, scale, ptxt);
    encryptor.encrypt(ptxt, ctxt);

    // 1. Encode -> Decode
    encoder.decode(ptxt, decoded);

    // 2. Encrypt -> Decrypt -> Decode
    decryptor.decrypt(ctxt, ptxt);
    encoder.decode(ptxt, decrypted);

    // 3. Successive multiplication & rescale & relinearizes
    int totalLvl = context.get_context_data(ctxt.parms_id())->chain_index();
    Ciphertext ctxt_temp = ctxt;
    for (int i=0; i<totalLvl; i++) {
        evaluator.multiply_vector_reduced_error(ctxt_temp, ones, ctxt_temp);
        evaluator.rescale_to_next_inplace(ctxt_temp);
    }
    decryptor.decrypt(ctxt_temp, ptxt);
    encoder.decode(ptxt, multiplied);

    precEncode = abs(vec[0] - decoded[0]);
    precEncrypt = abs(vec[0] - decrypted[0]);
    precMult = abs(vec[0] - multiplied[0]);

    // print results
    // std::cout << "\nUsing scalingfactor " << log_modulus << ",\n";
    // std::cout << "Encode precision bound : " << precEncode << "\n";
    // std::cout << "Encrypt precision bound : " << precEncrypt << "\n";
    // std::cout << "Multiplication precision bound : " << precMult << "\n\n";
    std::cout << precEncode << ", " << precEncrypt << ", " << precMult << "\n";
}

void reEncrypt(long n, SEALContext& context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out) {
    std::vector<double> output(n);
    Plaintext ptxt;

    decryptor.decrypt(ctxt_in, ptxt);
    encoder.decode(ptxt, output);
    encoder.encode(output, ctxt_in.scale(), ptxt);
    encryptor.encrypt(ptxt, ctxt_out);
}

long ShowFailure_Tanh(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n) {
    long failure = 0;
    double bound = pow(2.0, static_cast<double>(-alpha));
    Plaintext pout;
    std::vector<double> output;
    decryptor.decrypt(ctxt, pout);
    encoder.decode(pout, output);

    double max_err = -1.0;
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(tanh(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(Sigmoid(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(GeLU(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(Softplus(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(exp(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(expm1(x[i]) - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
    double avg_err = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = abs(1/x[i] - output[i]);
        if(diff > bound) failure++;
        max_err = max_err > diff ? max_err : diff;
        avg_err += diff;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "failure : " << failure << std::endl;
    std::cout << "max_err : " << max_err << std::endl;
    std::cout << "avg_err : " << avg_err/(double)n << std::endl;
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
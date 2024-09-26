#include "main.hpp"

int main(void) {
    // Timer setting
    std::cout << "set chrono timer ...\n";
    Timer timer;

    ////////////////////////////////////////////
    ///////      Select NAAF to run      ///////
    ////////////////////////////////////////////

    std::cout << "\n========================= NAAF =========================\n";
    std::cout << "+---------------------------------------+" << std::endl;
    std::cout << "| sample of non-arithmetic activations  |" << std::endl;
    std::cout << "+---------------------------------------+" << std::endl;
    std::cout << "| Examples                              |" << std::endl;
    std::cout << "+-------------------+-------------------+" << std::endl;
    std::cout << "| 1. ReLU           | naafs.cpp:num     |" << std::endl;
    std::cout << "| 2. Swish          | naafs.cpp:num     |" << std::endl;
    std::cout << "| 3. Tanh           | naafs.cpp:num     |" << std::endl;
    std::cout << "| 4. Sigmoid        | naafs.cpp:num     |" << std::endl;
    std::cout << "| 5. GeLU           | naafs.cpp:num     |" << std::endl;
    std::cout << "| 6. Softplus       | naafs.cpp:num     |" << std::endl;
    std::cout << "| 7. Inv(aSOR)      | naafs.cpp:num     |" << std::endl;
    std::cout << "| 8. Inv(Numerical) | naafs.cpp:num     |" << std::endl;
    std::cout << "| 9. Exp            | naafs.cpp:num     |" << std::endl;
    std::cout << "+-------------------+-------------------+" << std::endl;

    int sel = 0; // naaf
    bool valid = true;
    do {
        std::cout << "[*] select NAAF option or exit(0) : ";
        if (!(std::cin >> sel)) valid = false;
        else if (sel < 0 || sel > 10) valid = false;
        else valid = true;

        if (!valid) {
            std::cout << "[**] valid option : type 1 ~ 9\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (!valid);
    if (sel == 0 ) return 0;

    int alpha = 0; // precision alpha
    valid = true;
    do {
        std::cout << "[*] select alpha option(0-30) or exit(-1) : ";
        if (!(std::cin >> alpha)) valid = false;
        else if (alpha < 0 || alpha > 30) valid = false;
        else valid = true;

        if (!valid) {
            std::cout << "[**] valid option : type 0 ~ 30\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (!valid);
    if (alpha == -1 ) return 0;
    double epsilon = std::pow(2, -alpha);

    long level = 0;
    std::vector<double> K;  // for inverse (To be removed)
    std::function<double(double)> f_inv = [](double z) -> double {
        return z * (2 - z);
    }; // for inverse (To be removed)
    getRelaxationFactor(f_inv, alpha, epsilon, K); // for inverse (To be removed)

    // std::cout << "set ciphertext level ...\n";
    if (sel == 1) {         // 1. ReLU 
    } else if (sel == 2) {  // 2. Swish
    } else if (sel == 3) {  // 3. Tanh
        level = K.size() * 2 + 2; // 2 for normalization
    } else if (sel == 4) {  // 4. Sigmoid
    } else if (sel == 5) {  // 5. GeLU
    } else if (sel == 6) {  // 6. Softplus
    } else if (sel == 7) {  // 7. Inverse (Inner)
        // std::function<double(double)> f_inv = [](double z) -> double {
        //     return z * (2 - z);
        // };
        // getRelaxationFactor(f_inv, alpha, epsilon, K);
        level = K.size() * 2 + 2; // 2 for normalization
    }

    int d = 12;
    level = d;
    
    for (long scalingfactor=40; scalingfactor<=55; scalingfactor+=5) {

    // std::cout << "set HE parameters ...\n";

    std::cout << "\n< scalingfactor = " << scalingfactor << " >\n";

    // SEAL setting
    // long scalingfactor = 40;
    long log_modulus = scalingfactor;
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 65536;
    long n = poly_modulus_degree / 2;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    std::vector<int> modulus_list;
    modulus_list.emplace_back(60);  for(int i=0; i<level; i++) modulus_list.emplace_back(log_modulus);   modulus_list.emplace_back(60);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus_list));
    SEALContext context(parms);

    // Key generation
    KeyGenerator keygen(context);
    PublicKey pk;
    keygen.create_public_key(pk);
    auto sk = keygen.secret_key();
    RelinKeys rlks;
    keygen.create_relin_keys(rlks);

    // Evaluator, Encryptor, Decryptor, Encoder
    CKKSEncoder encoder(context);
    Encryptor encryptor(context, pk);
    Evaluator evaluator(context, encoder);
    Decryptor decryptor(context, sk);

    // Check plaintext/ciphertext precision
    // std::cout << "check precision ...\n";
    // for (int i=0; i<5; i++)
    //     checkPrecision(n, log_modulus, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks);

    // Generate input vector
    // std::cout << "generate input vector ...\n";
    std::vector<double> m_x(n), output(n);
    for(int i=0; i<n; i++) m_x[i] = -4.0 + 8.0 * static_cast<double>(i+1) / static_cast<double>(n+1);

    // Encode & Encrypt
    // std::cout << "encode and encrypt input vector ...\n";
    Plaintext ptxt;
    double scale = std::pow(2.0, log_modulus);
    encoder.encode(m_x, scale, ptxt);

    Ciphertext ctxt, ctxt_res;
    encryptor.encrypt(ptxt, ctxt);
    // std::cout << "initial level : " << context.get_context_data(ctxt.parms_id())->chain_index() << std::endl;
    // std::cout << "Input : ";
    // decrypt_and_print_part(ctxt, decryptor, encoder, n, 0, 3);

    // std::cout << "run naaf ...\n";
    if (sel == 1) {         // 1. ReLU 

    } else if (sel == 2) {  // 2. Swish

    } else if (sel == 3) {  // 3. Tanh
        std::cout << "Approximated Tanh ..." << std::endl;

        for (int i=0; i<10; i++) {
            timer.start();
            Tanh_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
            timer.end();
        }
        timer.calAvg();

        // timer.start();
        // Tanh_LA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        // timer.end();

        for (long i=0; i<n; i++) output[i] = tanh(m_x[i]);

        // result
        // std::cout << "Expected output : \n";
        // print_part(output, n, 0, 3);
        // decrypt_and_print_part(ctxt_res, decryptor, encoder, n, 0, 3);
        ShowFailure_Tanh(decryptor, encoder, ctxt_res, m_x, alpha, n);
        std::cout << "remaining level : " << context.get_context_data(ctxt_res.parms_id())->chain_index() << std::endl;

    } else if (sel == 4) {  // 4. Sigmoid
        std::cout << "\nApproximated Sigmoid ..." << std::endl;

        timer.start();
        Sigmoid_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();

    } else if (sel == 5) {  // 5. GeLU
        std::cout << "\nApproximated GeLU ..." << std::endl;

        timer.start();
        GeLU_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();
        
    } else if (sel == 6) {  // 6. Softplus
        std::cout << "\nApproximated Softplus ..." << std::endl;

        timer.start();
        Softplus_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();
        
    } else if (sel == 7) {  // 7. Inverse (aSOR)
        for(int i=0; i<n; i++) m_x[i] = 0.05 + 0.95 * static_cast<double>(i+1) / static_cast<double>(n+1);
        for (long i=0; i<n; i++) output[i] = 1 / m_x[i];
        
        encoder.encode(m_x, ctxt.scale(), ptxt);
	    encryptor.encrypt(ptxt, ctxt);

        std::cout << "\nInput : "; decrypt_and_print_part(ctxt, decryptor, encoder, n, 0, 3);

		std::cout << "\nApproximated Inverse (Numerical) ..." << std::endl;
        timer.start();
		Inv_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();

        // result
        std::cout << "Expected output : "; print_part(output, n, 0, 3);
        std::cout << "Computed output : "; decrypt_and_print_part(ctxt_res, decryptor, encoder, n, 0, 3);
        ShowFailure_Inverse(decryptor, encoder, ctxt_res, m_x, alpha, n);
        std::cout << "remaining level : " << context.get_context_data(ctxt_res.parms_id())->chain_index() << std::endl;
    } else if (sel == 8) {  // 7. Inverse (Numerical)

        for(int i=0; i<n; i++) m_x[i] = epsilon + 1.95 * static_cast<double>(i+1) / static_cast<double>(n+1);
        for (long i=0; i<n; i++) output[i] = 1 / m_x[i];
        
        encoder.encode(m_x, ctxt.scale(), ptxt);
	    encryptor.encrypt(ptxt, ctxt);

        std::cout << "\nInput : "; decrypt_and_print_part(ctxt, decryptor, encoder, n, 0, 3);

		std::cout << "\nApproximated Inverse (Numerical) ..." << std::endl;
        timer.start();
		Inv_NA2(d, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();

        // result
        std::cout << "Expected output : "; print_part(output, n, 0, 3);
        std::cout << "Computed output : "; decrypt_and_print_part(ctxt_res, decryptor, encoder, n, 0, 3);
        ShowFailure_Inverse(decryptor, encoder, ctxt_res, m_x, alpha, n);
        std::cout << "remaining level : " << context.get_context_data(ctxt_res.parms_id())->chain_index() << std::endl;
    } else if (sel == 9) {  // 8. Exp
        std::cout << "\nApproximated Exp ..." << std::endl;

        timer.start();
        Exp_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt, ctxt_res, timer);
        timer.end();

        for (long i=0; i<n; i++) output[i] = exp(m_x[i]);

        // result
        std::cout << "Expected output : ";
        print_part(output, n, 0, 3);
        decrypt_and_print_part(ctxt_res, decryptor, encoder, n, 0, 3);
        ShowFailure_Exp(decryptor, encoder, ctxt_res, m_x, alpha, n);
        std::cout << "remaining level : " << context.get_context_data(ctxt_res.parms_id())->chain_index() << std::endl;
        
    }

    }

    return 0;
}
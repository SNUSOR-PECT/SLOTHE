#include "naafs.hpp"

// Non-Arithmetic Activation Functions
// void ReLU(double x);      // return max(x, 0)
// void Swish(double x);     // return x * sigmoid(x)

/*
 * Tanh function
 * Reference : https://github.com/samhocevar/lolremez
 * Input : x
 * Output : tanh(x)
*/
void Tanh_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;

	std::vector<double> coeffs;

	// tanh(x) for input range {-8, 8}, deg = 3
	// Estimated max error: 3.694283658858717e-1
	coeffs = { 0.0, 0.39828491779402708, -0.0049916170572429936 };
	// tanh(x) for input range {-8, 8}, deg = 5
	// Estimated max error: 2.3948370323248433e-1
	coeffs = { 0.0, 0.54432569591496227, -0.017848386208625704, 0.00018381505278499826 };
	// tanh(x) for input range {-8, 8}, deg = 9
	// Estimated max error: 1.7532633168081485e-2
	coeffs = { 0.0, 0.74192320497372455, -0.063803740971915657, 0.0026722733788654965, -4.7969048289304421e-05, 3.0452150809489182e-07 };

	evalPolyOdd(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	ctxt_out = ctxt_temp;
}
void Tanh_LA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Plaintext pOne, pTwo;
	Ciphertext cOne, cTwo;
	Ciphertext ct, cz;

	// generate plaintext
	std::vector<double> ones(n, 1.0);
	std::vector<double> twos(n, 2.0);
	encoder.encode(ones, ctxt_in.scale(), pOne);
	encoder.encode(twos, ctxt_in.scale(), pTwo);
	encryptor.encrypt(pOne, cOne);
	encryptor.encrypt(pTwo, cTwo);

	std::vector<double> decoded;
	Plaintext ptxt_decrypted; // To be removed
	Ciphertext ctxt_temp;
	decryptor.decrypt(ctxt_in, ptxt_decrypted);
	encoder.decode(ptxt_decrypted, decoded);

	// after expm1(x) + 2, always positive -> just normalize
	for (long i=0; i<n; i++) decoded[i] = expm1(2*decoded[i]) + 2;

	encoder.encode(decoded, ctxt_in.scale(), ptxt_decrypted);
	encryptor.encrypt(ptxt_decrypted, ctxt_temp);
	
	double normN = 1e+07; // for [-8, 8]
	std::vector<double> n_inv(n, 1/normN);

	// normalization
	evaluator.multiply_vector_reduced_error(ctxt_temp, n_inv, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);

	std::cout << "\n check 2 : t = t * (1/normN) \n";
	for (long i=0; i<n; i++) decoded[i] *= n_inv[i];
	print_part(decoded, n, 0, 3);
	decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// inverse
	Inv_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	std::cout << "\n check 3 : \n";
	for (long i=0; i<n; i++) decoded[i] = 1 / decoded[i];
	print_part(decoded, n, 0, 3);
	decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// normalization back and multiply -2
	for (long i=0; i<n; i++) n_inv[i] = -2.0 * n_inv[i];
	evaluator.multiply_vector_reduced_error(ctxt_temp, n_inv, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);

	std::cout << "\n check 4 : \n";
	for (long i=0; i<n; i++) decoded[i] *= (-2.0 * n_inv[i]);
	print_part(decoded, n, 0, 3);
	decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// // z = 1 + (-2/t)
	evaluator.add_reduced_error(ctxt_temp, cOne, ctxt_temp);
	
	std::cout << "\n check 5 : \n";
	for (long i=0; i<n; i++) decoded[i] += 1.0;
	print_part(decoded, n, 0, 3);
	decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);
	ctxt_out = ctxt_temp;
}

/*
 * Sigmoid function
 * Reference : https://github.com/samhocevar/lolremez
 * Input : x
 * Output : 1/ (exp(-x)+1)
*/
void Sigmoid_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;

	std::vector<double> coeffs;

	// sigmoid(x) for input range {-8, 8}, deg = 3
	// Estimated max error: 8.94603755084224e-2
	coeffs = { 0.5, 0.15583087751130215, -0.0016336772377525508};
	// sigmoid(x) for input range {-8, 8}, deg = 5
	// Estimated max error: 4.0853315110567625e-2
	coeffs = { 0.5, 0.1965964728377535, -0.0048211354734327876, 4.3828355245228892e-05};
	// sigmoid(x) for input range {-8, 8}, deg = 7
	// Estimated max error: 1.8892310117363313e-2
	coeffs = { 0.5, 0.22055725616798885, -0.008555530904085595, 0.00017437069903207758, -1.2478985560336278e-06};
	// sigmoid(x) for input range {-8, 8}, deg = 9
	// Estimated max error: 1.8892310117363313e-2
	coeffs = { 0.5, 0.23414344280109828, -0.012025640398186115, 0.00039354155141787742, -6.1706828569287849e-06, 3.6043686016119918e-08};
	// sigmoid(x) for input range {-8, 8}, deg = 11
	// Estimated max error: 4.0715956372485779e-3
	coeffs = { 0.5, 0.24161696493694845, -0.014838367247276107, 0.00066948001518286372, -1.6824455201482871e-05, 2.1221641721149113e-07, -1.045105194195202e-09};

	evalPolyOdd(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	ctxt_out = ctxt_temp;
}

/*
 * GeLU function
 * Reference : https://github.com/samhocevar/lolremez
 * Input : x
 * Output : x/2 * (1 + erf(x/sqrt(2)))
*/
void GeLU_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;

	std::vector<double> coeffs;

	// GeLU(x) for input range {-8, 8}, deg = 2
	// Estimated max error: 4.9993715738368353e-1
	coeffs = {0.49993715738368355, 0.5, 0.062499999999999924};
	// GeLU(x) for input range {-8, 8}, deg = 6
	// Estimated max error: 1.5609816295815388e-1
	coeffs = {0.15609816295815387, 0.5, 0.17542866, 1e-11, -0.0038431061, -1e-11, 3.2478059e-05};
	// GeLU(x) for input range {-8, 8}, deg = 8
	// Estimated max error: 9.5342600112376785e-2
	coeffs = {0.095342600112376788, 0.5, 0.22588139582610095, -1e-11, -0.0087719276478916801, 1e-11, 0.00017221815387257933, -1e-11, -1.1839410433831415e-06};
	// GeLU(x) for input range {-8, 8}, deg = 10
	// Estimated max error: 5.7697200707737303e-2
	coeffs = {0.057697200707737303, 0.5, 0.27025638888361592, -1e-11, -0.015662347546635213, 1e-11, 0.00051628622235233275, -1e-11, -7.902185216961197e-06, 1e-11, 4.4789071611223128e-08};

	evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	ctxt_out = ctxt_temp;
}

/*
 * Softplus function
 * Reference : https://github.com/samhocevar/lolremez
 * Input : x
 * Output : log(1 + exp(x))
*/
void Softplus_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;

	std::vector<double> coeffs;

	// Softplus(x) for input range {-8, 8}, deg = 2
	// Estimated max error: 2.6227782350570383e-1
	coeffs = {0.95542500406564912, 0.5, 0.051674816028327353};
	// Softplus(x) for input range {-8, 8}, deg = 4
	// Estimated max error: 7.8099534057619972e-2
	coeffs = {0.77124671461756533, 0.5, 0.082536603095807487, 1e-11, -0.00052034996104344949};
	// Softplus(x) for input range {-8, 8}, deg = 6
	// Estimated max error: 2.7286444052623291e-2
	coeffs = {0.72043362461256855, 0.5, 0.1013129408489427, -1e-11, -0.0013919469397213795, 1e-11, 9.6304881156135099e-06};
	// Softplus(x) for input range {-8, 8}, deg = 8
	// Estimated max error: 1.0233577491629362e-2
	coeffs = {0.70338075805157463, 0.5, 0.11216236159902405, 1e-11, -0.0023454093711466227, -1e-11, 3.5215274922081019e-05, 1e-11, -2.0959057310016689e-07};
	// Softplus(x) for input range {-8, 8}, deg = 10
	// Estimated max error: 3.9968457802034743e-3
	coeffs = {0.69714402634014883, 0.5, 0.11819128930392243, -1e-11, -0.0031936017375783091, 1e-11, 7.5258450189936426e-05, -1e-11, -9.6379894363676408e-07, 1e-11, 4.9036464829536086e-09};

	evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	ctxt_out = ctxt_temp;
}

// Inner Functions

/*
 * Exp function
 * Reference : Minimax Approximation
 * Input : x
 * Output : Exp(x)
 * TODO : PA, NA, LA selection option
*/
void Exp_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;

	// exp(x) for input range {-8, 8}, deg = 11
	// polynomial coefficients
	std::vector<double> coeffs = { 
		0.78919824009924178,
		0.80582046823738174, 0.7313696115227859, 0.23850288369342329,
		0.0009565724089118299, 0.00065985917478436564, 0.0039611179252804201,
		0.0005489786020671407, -4.6021316887290597e-05, -4.8475009048631753e-06,
		1.0919219951807688e-06, 9.7745273355354518e-08
	};

	evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	ctxt_out = ctxt_temp;
}

/*
 * Expm1 function
 * Reference : definition
 * Input : x
 * Output : Exp(x) - 1
*/
void Expm1(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Plaintext pOne;
	Ciphertext cOne, cTemp;

	// generate plaintext
	std::vector<double> ones(n, 1.0);
	encoder.encode(ones, ctxt_in.scale(), pOne);
	encryptor.encrypt(pOne, cOne);

	// compute exp(x) - 1
	Exp_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_in, cTemp, timer);
	evaluator.sub_reduced_error(cTemp, cOne, cTemp);

	ctxt_out = cTemp;
}

/*
 * Inverse function
 * Reference : https://eprint.iacr.org/2024/1366.pdf
 * Input : x
 * Output : 1/x
*/
void Inv_NA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

    Plaintext pa, pb, pOne, pTwo, pKi;
	Ciphertext cOne, cTwo, cTemp;
	Ciphertext ca, cb, cKia;

	// generate plaintext
	std::vector<double> ones(n, 1.0);
	std::vector<double> twos(n, 2.0);
	encoder.encode(ones, ctxt_in.scale(), pOne);
	encoder.encode(twos, ctxt_in.scale(), pTwo);
	encryptor.encrypt(pOne, cOne);
	encryptor.encrypt(pTwo, cTwo);

	ca = ctxt_in;
	cb = cOne;

	for (size_t i=0; i<K.size(); i++) {
		std::vector<double> Kis(n, K[i]);

		// 2 - k_i*a
		evaluator.multiply_vector_reduced_error(ca, Kis, cKia);
		evaluator.rescale_to_next_inplace(cKia);
		evaluator.sub_reduced_error(cTwo, cKia, cTemp);

		/*
			b = k_i * b,			a = k_i * a
			b = b * (2 - k_i*a)		a = a * (2 - k_i*a)
		*/
		evaluator.multiply_vector_reduced_error(cb, Kis, cb);
		evaluator.rescale_to_next_inplace(cb);
		ca = cKia; // computed at above

		evaluator.multiply_reduced_error(cb, cTemp, rlks, cb);
		evaluator.rescale_to_next_inplace(cb);
		if (i != K.size()-1) {
			evaluator.multiply_reduced_error(ca, cTemp, rlks, ca);
			evaluator.rescale_to_next_inplace(ca);
		}
	}

	ctxt_out = cb;
}

// void Abs(double x);       // return |x|
// void Erf(double x);       // return erf(x)
// void Log(double x);       // return log(x)

// Comparison
// void Compare(double x, double y);     // return x > y ? 1.0 : 0.0

// for asor
void getRelaxationFactor(std::function<double(double)> f, double alpha, double epsilon, std::vector<double>& K) {
    std::vector<double> eps(1, epsilon);  // Init
    K.clear();

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

void compute_all_powers(const Ciphertext& ctxt, int deg, Evaluator &evaluator, RelinKeys &rlks, std::vector<Ciphertext>& powers, Timer& timer) {
	powers.resize(deg+1);
	powers[1] = ctxt;

	std::vector<int> levels(deg+1, 0);
	levels[1] = 0;
	levels[0] = 0;

	for (int i=2; i<=deg; i++) {
		// compute x^i
		int minlv = i;
		int cand = -1;
		for (int j=1; j<=i/2; j++) {
			int k = i-j;
			int newlv = max(levels[j], levels[k]) + 1;
			if (newlv < minlv) {
				cand = j;
				minlv = newlv;
			}
		}
		levels[i] = minlv;

		// use cand
		if (cand<0) throw std::runtime_error("Error");

		Ciphertext ctxt_temp = powers[cand];
		evaluator.multiply_reduced_error(ctxt_temp, powers[i-cand], rlks, powers[i]);
		evaluator.rescale_to_next_inplace(powers[i]);
	}
}

void evalPoly(std::vector<double> coeffs, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &pk, SecretKey &sk, RelinKeys &rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {
	Plaintext ptxt;
	Ciphertext ctxt_x, ctxt_temp;
	
	// compute all powers
	int deg = coeffs.size() - 1; // constant term
	std::vector<Ciphertext> powers(deg+1);
	compute_all_powers(ctxt_in, deg, evaluator, rlks, powers, timer);	

	long n = ctxt_in.poly_modulus_degree()/2;
    std::vector<double> m_coeffs(n);

	// evaluating polynomial
	for (int j=0; j<n; j++) m_coeffs[j] = coeffs[0];
	encoder.encode(m_coeffs, ctxt_in.scale(), ptxt);
	encryptor.encrypt(ptxt, ctxt_out); // constant term

	for (int i=1; i<=deg; i++) {
		for (int j=0; j<n; j++) m_coeffs[j] = coeffs[i];
		evaluator.multiply_vector_reduced_error(powers[i], m_coeffs, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
		evaluator.add_reduced_error(ctxt_out, ctxt_temp, ctxt_out);
	}
}

void evalPolyOdd(std::vector<double> coeffs, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &pk, SecretKey &sk, RelinKeys &rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {
	Plaintext ptxt;
	Ciphertext ctxt_x, ctxt_temp;
	
	// compute all powers
	int deg = (coeffs.size()-1)*2 - 1; // degree of poly
	int _deg = coeffs.size(); // odd terms + constant
	std::vector<Ciphertext> powers(deg+1);
	compute_all_powers(ctxt_in, deg, evaluator, rlks, powers, timer);	

	long n = ctxt_in.poly_modulus_degree()/2;
    std::vector<double> m_coeffs(n);

	// evaluating polynomial
	for (int j=0; j<n; j++) m_coeffs[j] = coeffs[0];
	encoder.encode(m_coeffs, ctxt_in.scale(), ptxt);
	encryptor.encrypt(ptxt, ctxt_out); // constant term

	for (int i=1, j=1; i<_deg; i++, j+=2) {
		for (int k=0; k<n; k++) m_coeffs[k] = coeffs[i];
		evaluator.multiply_vector_reduced_error(powers[j], m_coeffs, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
		evaluator.add_reduced_error(ctxt_out, ctxt_temp, ctxt_out);
	}
}
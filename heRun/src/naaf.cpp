#include "naafs.hpp"

// Non-Arithmetic Activation Functions
// void ReLU(double x);      // return max(x, 0)
// void Swish(double x);     // return x * sigmoid(x)

/*
 * Tanh function
 * Reference : standard math library implementation
 * Input : x
 * Output : tanh(x)
*/
void Tanh(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

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

	// double t = expm1(2 * x);
	// t = t + 2;
	// z = 1 - 2 / t
	// return z;

}
// void Sigmoid(double x);   // return 1 / {exp(-x)+1}
// void GeLU(double x);      // return x/2 * (1 + erf(x/sqrt(2)))
// void Softplus(double x);  // return  log(1 + exp(x))

// Inner Functions

/*
 * Exp function
 * Reference : Minimax Approximation
 * Input : x
 * Output : Exp(x)
 * TODO : PA, NA, LA selection option
*/
void Exp(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

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
	Exp(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_in, cTemp, timer);
	evaluator.sub_reduced_error(cTemp, cOne, cTemp);

	ctxt_out = cTemp;
}

/*
 * Inverse function
 * Reference : https://eprint.iacr.org/2024/1366.pdf
 * Input : x
 * Output : 1/x
*/
void Inv(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

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

		Ciphertext cipher_temp = powers[cand];
		evaluator.multiply_reduced_error(cipher_temp, powers[i-cand], rlks, powers[i]);
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
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
void Tanh(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out) {

}
// void Sigmoid(double x);   // return 1 / {exp(-x)+1}
// void GeLU(double x);      // return x/2 * (1 + erf(x/sqrt(2)))
// void Softplus(double x);  // return  log(1 + exp(x))

// Inner Functions
// void Exp(double x);       // return exp(x)

/*
 * Inverse function
 * Reference : https://eprint.iacr.org/2024/1366.pdf
 * Input : x
 * Output : 1/x
*/
void Inv(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out) {

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
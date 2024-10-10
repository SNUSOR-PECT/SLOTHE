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

	// tanh(x) for input range {-4, 4}, deg = 3
	// Estimated max error: 1.789207510168448e-1
	coeffs = { 0.0, 0.62332351004520858, -0.026138835804040812 };
	// tanh(x) for input range {-4, 4}, deg = 5
	// Estimated max error: 8.1706630221135251e-2
	coeffs = { 0.0, 0.786385891351014, -0.077138167574924602, 0.0028050147356946491 };
	// tanh(x) for input range {-4, 4}, deg = 7
	// Estimated max error: 3.7784620234726626e-2
	coeffs = { 0.0, 0.88222902467195541, -0.13688849446536952, 0.011159724738052965, -0.00031946203034460873 };
	// tanh(x) for input range {-4, 4}, deg = 9
	// Estimated max error: 1.7532633168081485e-2
	coeffs = { 0.0, 0.93657377120439311, -0.19241024637097784, 0.025186659290744155, -0.0015796948113737689, 3.6908734480506796e-05 };
	// tanh(x) for input range {-4, 4}, deg = 11
	// Estimated max error: 8.1431912744971558e-3
	coeffs = { 0.0, 0.96646785974779381, -0.23741387595641772, 0.042846720971703278, -0.0043070605315796151, 0.00021730961122456692, -4.2807508754235473e-06 };
	// tanh(x) for input range {-4, 4}, deg = 13
	// Estimated max error: 3.783216478934053e-3
	coeffs = { 0.0, 0.98252959106739335, -0.27072421260223462, 0.061452695943391832, -0.0086191833740324499, 0.00069358272749582746, -2.9202772402216391e-05, 4.9703177949257924e-07 };
	// tanh(x) for input range {-4, 4}, deg = 15
	// Estimated max error: 1.7577702922961753e-3
	coeffs = { 0.0, 0.99100392699616702, -0.29383017851338983, 0.078798779536478181, -0.014198267091101651, 0.0015980123896941422, -0.00010675175571847973, 3.8534061756133606e-06, -5.7727666405365866e-08 };
	// tanh(x) for input range {-4, 4}, deg = 17
	// Estimated max error: 8.167196400045807e-4
	coeffs = { 0.0, 0.99541175580410124, -0.30909547814177113, 0.093585182898206318, -0.020466617561347263, 0.00298367668408747, -0.00027848786902639751, 1.5872708915911474e-05, -5.0123093812338259e-07, 6.7053737914188064e-09 };
	// tanh(x) for input range {-4, 4}, deg = 19
	// Estimated max error: 3.7947814019801303e-4
	coeffs = { 0.0, 0.99767839249907575, -0.31880495482725274, 0.10535423679882261, -0.0268047399648969, 0.0048042004261275089, -0.00058229149586322589, 4.6265833340040046e-05, -2.2970427564201655e-06, 6.4453036567128196e-08, -7.788849575878072e-10 };
	// // tanh(x) for input range {-4, 4}, deg = 21 ------> scalingfactor >= 45
	// // Estimated max error: 1.763199136017716e-4
	// coeffs = { 0.0, 0.99883313321301748, -0.32479544985518882, 0.11422658006526534, -0.032708425022993169, 0.0069330505217938534, -0.0010390032560081844, 0.0001072041365415903, -7.4005373001906433e-06, 3.2526885257103384e-07, -8.2106040366209758e-09, 9.0474652911861078e-11 };
	// // tanh(x) for input range {-4, 4}, deg = 23 ------> scalingfactor >= 50
	// // Estimated max error: 8.1924954349264908e-5
	// coeffs = { 0.0, 0.99941686671499574, -0.32840024803104267, 0.12062648996143112, -0.037856858558370121, 0.0092040787825085867, -0.0016451128737233391, 0.00021032249179001481, -1.8833955800622345e-05, 1.1479907181724712e-06, -4.5244789363000279e-08, 1.0378149603070405e-09, -1.0509486286423538e-11 };

	bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;
	for (size_t i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);

	evalPolyOdd(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}

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

	// Expm1(2x)+2
	evaluator.multiply_vector_reduced_error(ctxt_in, twos, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);
	// Expm1_PA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	Expm1_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	evaluator.add_reduced_error(ctxt_temp, cTwo, ctxt_temp); // expm1(2x)+2

	// after expm1(x) + 2, always positive -> just normalize
	for (long i=0; i<n; i++) decoded[i] = expm1(2*decoded[i]) + 2;

	// // encoder.encode(decoded, ctxt_in.scale(), ptxt_decrypted);
	// // encryptor.encrypt(ptxt_decrypted, ctxt_temp);

	// std::cout << "\n check 1 : expm1(2*decoded[i]) + 2 \n";
	// std::cout << "plain : "; print_part(decoded, n, 0, 3);
	// std::cout << "cipher : "; decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// double normN = 1e+04; // for x in [-4, 4]
	// std::vector<double> n_inv(n, 1/normN);
	std::vector<double> n_inv(n);
	for (long i=0; i<n; i++) {
		n_inv[i] = 1 / pow(10, ceil(log10(decoded[i])));
	}

	// normalization
	evaluator.multiply_vector_reduced_error(ctxt_temp, n_inv, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);

	// std::cout << "remaining level : " << context.get_context_data(ctxt_temp.parms_id())->chain_index() << std::endl;

	// std::cout << "\n check 2 : t = t * (1/normN) \n";
	// for (long i=0; i<n; i++) decoded[i] *= n_inv[i];
	// std::cout << "plain : "; print_part(decoded, n, 0, 3);
	// std::cout << "cipher : "; decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// inverse
	// reEncrypt(n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp);
	// Inv_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	Inv_NA2(7, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	// reEncrypt(n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp);

	// std::cout << "\n check 3 : Inverse(t)\n";
	// for (long i=0; i<n; i++) decoded[i] = 1 / decoded[i];
	// std::cout << "plain : "; print_part(decoded, n, 0, 3);
	// std::cout << "cipher : "; decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// normalization back and multiply -2
	for (long i=0; i<n; i++) n_inv[i] = -2.0 * n_inv[i];
	evaluator.multiply_vector_reduced_error(ctxt_temp, n_inv, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);

	// std::cout << "\n check 4 : \n";
	// for (long i=0; i<n; i++) decoded[i] *= n_inv[i];
	// std::cout << "plain : "; print_part(decoded, n, 0, 3);
	// std::cout << "cipher : "; decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);

	// z = 1 + (-2/t)
	evaluator.add_reduced_error(ctxt_temp, cOne, ctxt_temp);
	
	// std::cout << "\n check 5 : \n";
	// for (long i=0; i<n; i++) decoded[i] += 1.0;
	// std::cout << "plain : "; print_part(decoded, n, 0, 3);
	// std::cout << "cipher : "; decrypt_and_print_part(ctxt_temp, decryptor, encoder, n, 0, 3);
	
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

	// sigmoid(x) for input range {-4, 4}, deg = 3
	// Estimated max error: 2.2760475992094946e-2, max error: 0.0227605
	coeffs = { 0.5, 0.21453639271347655, -0.0062326915126264485};

	// sigmoid(x) for input range {-4, 4}, deg = 5
	// Estimated max error: 5.3519175433499358e-3, max error: 0.005352
	coeffs = { 0.5, 0.23897898076877397, -0.013012638866364314, 0.0003557213593295689};
	
	// sigmoid(x) for input range {-4, 4}, deg = 7
	// Estimated max error: 1.2638290536590791e-3, max error: 0.00126389
	coeffs = { 
		0.5, 0.24676528422735949, -0.017300805442284611, 0.00092136471074142593, -2.0906804896129981e-05
		};

	// sigmoid(x) for input range {-4, 4}, deg = 9
	// Estimated max error: 2.986590082101457e-4, max error: 0.000298841	
	coeffs = {
		0.5, 0.24908625585511535, -0.019409172860760358, 0.0014218805139471745, -6.41719807419047e-05,
		1.2342169758139805e-06
	};

	// sigmoid(x) for input range {-4, 4}, deg = 11
	// Estimated max error: 7.0585572261025242e-5, max error: 7.1015e-05
	coeffs = {
		0.5, 0.24974875098161844, -0.020303466240063534, 0.0017514267618046176, -0.00011302253734716232,
		4.3735408648458223e-06, -7.2914888563853401e-08
	};

	// sigmoid(x) for input range {-4, 4}, deg = 13
	// Estimated max error: 1.6682667234334763e-5, max error: 1.74713e-05
	coeffs = {
		0.5, 0.24993227677273527, -0.020647575074437364, 0.0019320831871432814, -0.00015316829601868468,
		8.6748629899631875e-06, -2.9286849555362973e-07, 4.3082023776660073e-09
	};	

	// sigmoid(x) for input range {-4, 4}, deg = 15
	// Estimated max error: 3.9429080156412192e-6, max error: 4.2184e-06
	coeffs = {
		0.5, 0.24998202232448144, -0.020771123126905967, 0.0020194025489048404, -0.0001800905884758714,
		1.2904789395484202e-05, -6.4691690967627061e-07, 1.934098497867749e-08, -2.545573101034679e-10
	};

	// sigmoid(x) for input range {-4, 4}, deg = 17
	// Estimated max error: 9.3189736937572539e-7, max error: 4.07247e-06
	coeffs = {
		0.5, 0.2499952850684404, -0.020813241102316516, 0.0020578865978420138, -0.00019573351427033028,
		1.6254737873407808e-05, -1.0519204719654216e-06, 4.7125872458180907e-08, -1.2631257205667961e-09,
		1.5041000526967648e-11
	};

	// sigmoid(x) for input range {-4, 4}, deg = 19
	// Estimated max error: 2.202518552944876e-7, max error: 1.08728e-06
	coeffs = {
		0.5, 0.24999877551056107, -0.020827032313063785, 0.0020736887771779845, -0.00020389787373538914,
		1.8526251223613942e-05, -1.4215247770013663e-06, 8.3350346370101543e-08, -3.3673755797502322e-09,
		8.1744027045685571e-11, -8.8872658774586251e-13
	};

	// sigmoid(x) for input range {-4, 4}, deg = 21
	// Estimated max error: 5.2056033550013923e-8, max error: 7.37231e-07
	coeffs = {
		0.5, 0.24999968456664207, -0.02083140487523473, 0.0020798235481334399, -0.00020781701443499536,
		1.9895263437650307e-05, -1.7078282449306869e-06, 1.2076020918652356e-07, -6.4460994747614892e-09,
		2.367677392869483e-10, -5.2501013746844492e-12, 5.2512134394612221e-14
	};

	// sigmoid(x) for input range {-4, 4}, deg = 23
	// Estimated max error: 5.2056033550013923e-8, max error: 3.20765e-06
	coeffs = {
		0.5, 0.24999991929636725, -0.02083275509848543, 0.0020820990743386906, -0.00020957599628934303,
		2.0647072088220173e-05, -1.90340716364376e-06, 1.533369714793988e-07,  -9.9942888965579785e-09,
		4.8823308333023612e-10, -1.6421936173580823e-11, 3.3503468033081045e-13, -3.1027813882763066e-15
	};


	// sigmoid(x) for input range {-4, 4}, deg = 5
	// Estimated max error: 8.94603755084224e-2

	// // sigmoid(x) for input range {-8, 8}, deg = 3
	// // Estimated max error: 8.94603755084224e-2
	// coeffs = { 0.5, 0.15583087751130215, -0.0016336772377525508};
	// // sigmoid(x) for input range {-8, 8}, deg = 5
	// // Estimated max error: 4.0853315110567625e-2
	// coeffs = { 0.5, 0.1965964728377535, -0.0048211354734327876, 4.3828355245228892e-05};
	// // sigmoid(x) for input range {-8, 8}, deg = 7
	// // Estimated max error: 1.8892310117363313e-2
	// coeffs = { 0.5, 0.22055725616798885, -0.008555530904085595, 0.00017437069903207758, -1.2478985560336278e-06};
	// // sigmoid(x) for input range {-8, 8}, deg = 9
	// // Estimated max error: 1.8892310117363313e-2
	// coeffs = { 0.5, 0.23414344280109828, -0.012025640398186115, 0.00039354155141787742, -6.1706828569287849e-06, 3.6043686016119918e-08};
	// // sigmoid(x) for input range {-8, 8}, deg = 11
	// // Estimated max error: 4.0715956372485779e-3, max_error : 0.0040716
	// coeffs = { 0.5, 0.24161696493694845, -0.014838367247276107, 0.00066948001518286372, -1.6824455201482871e-05, 2.1221641721149113e-07, -1.045105194195202e-09};
	// // sigmoid(x) for input range {-8, 8}, deg = 13
	// // Estimated max error: 4.0715956372485779e-3, max_error : 0.00189177
	// coeffs = {
	// 	0.5, 0.24563239776684834, -0.016920263287639663, 0.00096019837411549737, 
	// 	-3.3668685054814257e-05, 6.77326882320144e-07, -7.1295831060098612e-09, 3.033641232254512e-11
	// };
	// // sigmoid(x) for input range {-8, 8}, deg = 15
	// // Estimated max error: 4.0715956372485779e-3, max_error : 0.000878985, time : 2625.1 
	// coeffs = {
	// 	0.5, 0.24775098174904175, -0.018364386157086864, 0.0012312309302574716, -5.5461980824615824e-05,
	// 	1.5605589743106857e-06, -2.6062440360956966e-08, 2.351932480232764e-10, -8.8085428474984537e-13
	// };
	// // sigmoid(x) for input range {-8, 8}, deg = 17, depth 7//transparant
	// // Estimated max error: 4.0835982000229035e-4, max_error : 0.000408506, time : 2930.8
	// coeffs = {
	// 	0.5, 0.24885293895102531, -0.019318467383860696, 0.0014622684827844737, -7.9947724849012747e-05,
	// 	2.91374676180417e-06, -6.7990202399022829e-08, 9.6879326879342494e-10, -7.6481771564236845e-12,
	// 	2.557897106711886e-14
	// };
	// // sigmoid(x) for input range {-8, 8}, deg = 19, depth 7//transparant
	// // Estimated max error: 1.8973907009900651e-4, max_error : 0.000204864, time : 3231.2
	// coeffs = {
	// 	0.5, 0.24941959812476894, -0.019925309676703296, 0.0016461599499816033, -0.00010470601548787851,
	// 	4.6916019786401454e-06, -1.4216100973223288e-07, 2.8238423669458036e-09, -3.5050090887758873e-11,
	// 	2.4586882235385207e-13, -7.4280257948666305e-16
	// };

    bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;

	for (int i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);

	evalPolyOdd(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
    
	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}

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

	// // GeLU(x) for input range {-4, 4}, deg = 2
	// // Estimated max error: 2.3236897642638954e-1, max error: 0.232369
	coeffs = {
		0.23236897642638954, 0.5, 0.12499208218954172
	};

	// // GeLU(x) for input range {-4, 4}, deg = 4
	// // Estimated max error: 8.5367342982687048e-2, max error: 0.0576975
	coeffs = {
		0.085367342982687042, 0.5, 0.23009710938324585, -0.0072359965666587501
	};

	// GeLU(x) for input range {-4, 4}, deg = 6
	// Estimated max error: 3.0403586794267702e-2, max error: 0.0304037
	coeffs = {
		0.030403586794267703, 0.5, 0.3095727644548717, -0.022057934197823387, 0.00065760259726501639
	};

	// GeLU(x) for input range {-4, 4}, deg = 8
	// Estimated max error: 9.7864875482398635e-3, max error: 0.00978655
	coeffs = {
		0.0097864875482398632, 0.5, 0.35871545290287182, -0.03898432461029893, 0.0024569927859161198,
		-5.8639560812159148e-05
	};

	// GeLU(x) for input range {-4, 4}, deg = 10
	// Estimated max error: 2.8080717546215479e-3, max error: 0.00280842
	coeffs = {
		0.0028080717546215481, 0.5, 0.38346715083479921, -0.052381346863555299, 0.0049315073065240274,
		-0.00024243334309001789, 4.7327817382536248e-06
	};

	// GeLU(x) for input range {-4, 4}, deg = 12
	// Estimated max error: 7.1940208816310654e-4, max error: 0.000724509
	coeffs = {
		0.00071940208816310654, 0.5, 0.39379826081165603, -0.060413467115863075, 0.0071747313406209023,
		-0.00052241330348353312, 2.0716376218231257e-05, -3.4034289741299491e-07
	};

	// GeLU(x) for input range {-4, 4}, deg = 14
	// Estimated max error: 1.6558474704683232e-4, max error: 0.000189826
	coeffs = {
		0.00016558474704683233, 0.5, 0.3974464456329086, -0.064257452575186905, 0.0086740168431946891,
		-0.00079846552147692148, 4.6649018320725335e-05, -1.5429873434924739e-06, 2.1837995880494254e-08
	};
	
	// GeLU(x) for input range {-4, 4}, deg = 16
	// Estimated max error: 3.4493328794615871e-5, max error: 8.68281e-05
	coeffs = {
		3.4493328794615872e-05, 0.5, 0.39855722270195454, -0.065779577518174925, 0.0094610101185500818,
		-0.00099694424560165906, 7.3698375688078315e-05, -3.5776876979608134e-06, 1.0131052780459556e-07,
		-1.2582506009220599e-09
	};

	// GeLU(x) for input range {-4, 4}, deg = 18
	// Estimated max error: 6.5499835537812364e-6, max error: 3.1279e-05
	coeffs = {
		6.5499835537812362e-06,  0.5, 0.39885358929095927, -0.066291628031025548, 0.0097991506357187428,
		-0.0011082107270140276, 9.4183720851956701e-05, -5.7864546510569294e-06, 2.3977943235317155e-07,
		-5.9305310954201521e-09, 6.5588333855389698e-11
	};

	// // GeLU(x) for input range {-4, 4}, deg = 20
	// // Estimated max error: 1.1413589951183467e-6, max error: 3.0849e-05
	// coeffs = {
	// 	1.1413589951183468e-06, 0.5, 0.39892382836274937, -0.066440971101446963, 0.0099216254950602392,
	// 	-0.0011590020421636861, 0.00010624292488900897, -7.5250748842505974e-06, 3.9460398110724235e-07,
	// 	-1.4248522872201567e-08, 3.1266892920303584e-10, -3.1157663517468109e-12
	// };

	// // GeLU(x) for input range {-4, 4}, deg = 22
	// // Estimated max error: 1.8360508589354925e-7, max error: 3.0849e-05
	// coeffs = {
	// 	1.8360508589354924e-07, 0.5, 0.39893878420174828, -0.066479345362134262, 0.0099598564545339743,
	// 	-0.0011784685134540486, 0.00011200962500321265, -8.5877553224965423e-06, 5.2014542959251707e-07,
	// 	-2.3761947196583011e-08, 7.5977765225374954e-10, -1.4978578003708428e-11, 1.3580095113824087e-13
	// };

	

	// // GeLU(x) for input range {-8, 8}, deg = 2
	// // Estimated max error: 4.9993715738368353e-1
	// coeffs = {0.49993715738368355, 0.5, 0.062499999999999924};
	// // GeLU(x) for input range {-8, 8}, deg = 6
	// // Estimated max error: 1.5609816295815388e-1
	// coeffs = {0.15609816295815387, 0.5, 0.17542866, 1e-11, -0.0038431061, -1e-11, 3.2478059e-05};
	// // GeLU(x) for input range {-8, 8}, deg = 8
	// // Estimated max error: 9.5342600112376785e-2
	// coeffs = {0.095342600112376788, 0.5, 0.22588139582610095, -1e-11, -0.0087719276478916801, 1e-11, 0.00017221815387257933, -1e-11, -1.1839410433831415e-06};
	// // GeLU(x) for input range {-8, 8}, deg = 10
	// // Estimated max error: 5.7697200707737303e-2
	// coeffs = {0.057697200707737303, 0.5, 0.27025638888361592, -1e-11, -0.015662347546635213, 1e-11, 0.00051628622235233275, -1e-11, -7.902185216961197e-06, 1e-11, 4.4789071611223128e-08};
	
	bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;

	for (int i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);

	evalPolyEven(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	//evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	
	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}
	
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

	// Softplus(x) for input range {-4, 4}, deg = 2
	// Estimated max error: 2.6227782350570383e-1, max error: 0.0593421
	coeffs = {
		0.75248924295936692, 0.5, 0.082812671709866531
	};

	// Softplus(x) for input range {-4, 4}, deg = 4
	// Estimated max error: 9.1383682419161638e-3, max error: 0.00913842
	coeffs = {
		0.70228554880186145, 0.5, 0.1114485441906545, -0.0018611355319392187
	};
	
	// Softplus(x) for input range {-4, 4}, deg = 6
	// Estimated max error: 1.6197740322244552e-3, max error: 0.00161983
	coeffs = {
		0.69476695459216975, 0.5, 0.12094120990149097, -0.0035278977357680128, 7.1554006174467811e-05
	};
	
	// Softplus(x) for input range {-4, 4}, deg = 8
	// Estimated max error: 3.0699287083856586e-4, max error: 0.000307113
	coeffs = {
		0.69345417343078386, 0.5, 0.12383954735192812, -0.0044842536188108798, 0.00017055092283964416, 
		-3.1685127189675882e-06
	};
	
	// Softplus(x) for input range {-4, 4}, deg = 10
	// Estimated max error: 6.0596666859050606e-5, max error: 6.10069e-05
	coeffs = {
		0.69320777722680438, 0.5, 0.12467850055940667, -0.004926573388780834, 0.00025080357415446574,
		-9.053064260606408e-06, 1.5006990965047925e-07
	};

	// Softplus(x) for input range {-4, 4}, deg = 12
	// Estimated max error: 1.2297546646296879e-50606e-5, max error: 1.27979e-05
	coeffs = {
		0.69315947810659162, 0.5, 0.12491290931047742, -0.0051063460177903199, 0.00030050880940701353,
		-1.5209626819075473e-05, 4.9944054069265138e-07, -7.4035621084141629e-09
	};

	// Softplus(x) for input range {-4, 4}, deg = 14
	// Estimated max error: 2.5467780061674809e-6, max error: 3.13842e-06
	coeffs = {
		0.69314972733795144, 0.5, 0.12497680140513724, -0.0051733554500065131, 0.00032654498798652181,
		-1.9988455627916607e-05, 9.4720496867399717e-07, -2.8123873040190334e-08, 3.7555386437188879e-10
	};

	// Softplus(x) for input range {-4, 4}, deg = 16
	// Estimated max error: 5.3566018372224658e-7, max error: 7.54947e-07
	coeffs = {
		0.69314771622012905, 0.5, 0.12499390069663668, -0.0051968274810943555, 0.00033869213385439212,
		-2.3053529264913293e-05, 1.3650434565126281e-06, -5.9559260399872748e-08, 1.6034667461659484e-09,
		-1.9441438938010064e-11
	};

	// Softplus(x) for input range {-4, 4}, deg = 18
	// Estimated max error: 1.1405052810205151e-7, max error: 7.81791e-07
	coeffs = {
		0.69314729461047342, 0.5, 0.12499841305587897, 	-0.0052046723131143223, 0.00034389594330438614,
		-2.4771694596504033e-05, 1.6822170059063372e-06, -9.3830343474475882e-08, 3.755687693460758e-09,
		-9.2168143009115088e-11, 1.0221734691366547e-12
	};

	// // Softplus(x) for input range {-8, 8}, deg = 2
	// // Estimated max error: 2.6227782350570383e-1
	// coeffs = {0.95542500406564912, 0.5, 0.051674816028327353};
	// // Softplus(x) for input range {-8, 8}, deg = 4
	// // Estimated max error: 7.8099534057619972e-2
	// coeffs = {0.77124671461756533, 0.5, 0.082536603095807487, 1e-11, -0.00052034996104344949};
	// // Softplus(x) for input range {-8, 8}, deg = 6
	// // Estimated max error: 2.7286444052623291e-2
	// coeffs = {0.72043362461256855, 0.5, 0.1013129408489427, -1e-11, -0.0013919469397213795, 1e-11, 9.6304881156135099e-06};
	// // Softplus(x) for input range {-8, 8}, deg = 8
	// // Estimated max error: 1.0233577491629362e-2
	// coeffs = {0.70338075805157463, 0.5, 0.11216236159902405, 1e-11, -0.0023454093711466227, -1e-11, 3.5215274922081019e-05, 1e-11, -2.0959057310016689e-07};
	// // Softplus(x) for input range {-8, 8}, deg = 10
	// // Estimated max error: 3.9968457802034743e-3
	// coeffs = {0.69714402634014883, 0.5, 0.11819128930392243, -1e-11, -0.0031936017375783091, 1e-11, 7.5258450189936426e-05, -1e-11, -9.6379894363676408e-07, 1e-11, 4.9036464829536086e-09};

bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;

	for (int i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);

	evalPolyEven(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	//evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	
	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}
	
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
	// exp(x) for input range {-8, 8}, deg = 12
	// Estimated max error: 6.9847911540546618e-2
	coeffs = { 
		1.0348549157458065, 0.90254574305568946, 0.45460894517316036, 0.2083068371604872, 0.051460849541035601,
		0.003300068424021529, 0.00059146168240658582, 0.00045473808525950491, 5.5583873985284263e-05,
		-3.3756379439475385e-06, -3.1998158686145635e-07, 8.938536508918577e-08, 7.3567573506096368e-09
	};
	// exp(x) for input range {-8, 8}, deg = 13
	// Estimated max error: 1.8464536674464091e-2
	coeffs = {
		1.0162688150196024, 1.0151656120796484, 0.4754984444738129, 0.15898817244307911, 0.047655502146270609, 
		0.009469437414373557, 0.00084501819147693848, 0.00012404460213915011, 4.7946371846829354e-05, 
		5.2402032461441366e-06, -2.1394041129885851e-07, -1.8354208687483416e-08, 6.8046104925637566e-09, 5.1814626949681181e-10
	};
	// exp(x) for input range {-8, 8}, deg = 14
	// Estimated max error: 4.5986100436831798e-3
	coeffs = {
		0.99793439351428825, 1.0076540467235238, 0.50359780424691192, 0.1622723520222516, 0.04062592459335404,
		0.0090591172951875916, 0.001504391404722287, 0.00014601752841999479, 1.8497424594732924e-05, 4.6681701170016995e-06,
		4.6117029005224032e-07, -1.1205629866794604e-08, -8.6937996298728718e-10, 4.8378548649822663e-10,
		3.426753658217539e-11
	};
	// exp(x) for input range {-8, 8}, deg = 15
	// Estimated max error: 1.082120710159185e-3
	coeffs = {
		0.9990217145299044, 0.99907996327451232, 0.50193385382991584, 0.16727699688056655, 0.041041720222194954,
		0.0082141712465480417, 0.0014654247003862545, 0.00020891033993638476, 2.0236501050563403e-05, 
		2.2652325774980806e-06, 4.2132604434418509e-07, 3.7958775994285675e-08, -4.1669566760104027e-10, 
		-2.8462589448106963e-11, 3.2246970421738508e-11, 2.1347933669654444e-12
	};
	// exp(x) for input range {-8, 8}, deg = 16 // transparant for scaling factor 2^40
	// Estimated max error: 2.4123936424149114e-4
	// coeffs = {
	// 	1.0000983086534192, 0.99953404385689315, 0.4997794397789348, 0.16701223093803194, 0.041748954741921848,
	// 	0.0082588353304666306, 0.0013769882969444369, 0.00020558802924816117, 2.5666700211076415e-05,
	// 	2.3920977867122337e-06, 2.4027395216735937e-07, 3.5364295652128558e-08, 2.9273099584048915e-09, 
	// 	-1.4410868494212733e-12, 8.7157133387653415e-14, 2.0222193023754537e-12, 1.2564436687560195e-13
	// };
	// // exp(x) for input range {-8, 8}, deg = 17
	// coeffs = {
	// 	1.0000470485337321, 1.0000445500947075, 0.49988191857744274, 0.16662919279441651, 0.041715339574576855,
	// 	0.008342652962348724, 0.0013811891155281579, 0.0001973536680004622, 2.5408891903948518e-05, 
	// 	2.8210700738583012e-06, 2.4886604271946425e-07, 2.268761787753262e-08, 2.7686730358909311e-09, 
	// 	2.1190457120993993e-10, 1.6123150126678339e-12, 1.1706631486766591e-13, 1.1968738821325122e-13,
	// 	7.0051460638760276e-15
	// };
	// // exp(x) for input range {-8, 8}, deg = 18 // error increases for scaling factor 2^40 in normalized setting
	// coeffs = {
	// 	0.99999616683294323, 1.0000226107597305, 0.50001076212528972, 0.16664564239630106, 0.041661638166427602,
	// 	0.0083390554763170723, 0.0013898059135244595, 0.00019770692262237812, 2.4716322493140667e-05, 2.802674444791058e-06,
	// 	2.801335465845061e-07, 2.3231048556053899e-08, 1.9394805033084182e-09, 2.0276141278016601e-10, 
	// 	1.442782029302512e-11, 1.9869306084984507e-13, 1.2878728607673224e-14,
	// 	6.7050754581321854e-15, 3.7090267822928621e-16
	// };
	// // exp(x) for input range {-8, 8}, deg = 19
	// coeffs = {
	// 	0.99999815129206371, 0.9999982399210785, 0.50000574036532008, 0.1666684965975094, 0.041663730095132114,
	// 	0.0083327690867660564, 0.0013894703955504889, 0.00019849287699167075, 2.474327956846644e-05, 2.7494498150180256e-06,
	// 	2.7891689488361976e-07, 2.5348244454163063e-08, 1.9717365417781227e-09, 1.5186085482154407e-10, 1.3929407400361025e-11,
	// 	9.2592448542632962e-13, 1.7031809422902854e-14, 1.0230227275577855e-15, 3.5648336397055117e-16, 1.8692596459501636e-17
	// };
	bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;
	for (size_t i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);

	evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);
	
	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}

	ctxt_out = ctxt_temp;
}
/*
 * Exp function
 * Reference : Minimax Approximation
 * Input : x
 * Output : Exp(x)
 * TODO : PA, NA, LA selection option
*/
void Exp_NA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Ciphertext ctxt_temp = ctxt_in;
	
	// polynomial coefficients
	// compute exp(x) by approximation of exp(x/B) 
	int log_B = 4; // B = 64 --> lob_B = 6
	// exp(x/64) for input range {-8, 8}, deg = 6, log_B = 6
	std::vector<double> coeffs = {
		0.99999999999997691,0.015625000001294038, 0.00012207031250902644,
	 	6.3578271585644867e-07, 2.4835262789494062e-09, 7.766075608611715e-12,2.0223685068764693e-14 };
    // exp(x/64) for input range {-8, 8}, deg = 7, log_B = 6
	coeffs = {
		0.99999999999998845, 0.015624999999999839, 0.00012207031250577666, 6.3578287763174613e-07, 2.4835264143568053e-09,
		7.7610201232207903e-12, 2.0222274575981516e-14, 4.5138306140717325e-17
	};
	// exp(x/32) for input range {-8, 8}, deg = 7, log_B = 5
	coeffs = {
		0.99999999999703881, 0.031249999999917746, 0.00048828125148044066, 5.0862630349692237e-06,
		3.9736314209061608e-08, 2.4835200397576942e-10, 1.2963936748973369e-12, 5.7871131015244916e-15
	};
	// exp(x/16) for input range {-8, 8}, deg = 7, log_B = 4 // the best
	coeffs = {
		0.99999999923843008, 0.062499999957691746, 0.0019531253806492812, 4.0690111435471404e-05,
		6.3575315810601785e-07, 7.9469351476292474e-09, 8.3526260566246237e-11, 7.4558560109173961e-13
	};
	// // exp(x/16) for input range {-8, 8}, deg = 8, log_B = 4 //
	// coeffs = {
	// 	1.0000000000010572, 0.062499999976216671, 0.0019531249993228611, 4.0690109119897049e-05, 
	// 	6.3578294983438113e-07, 7.9470075085679408e-09, 8.2781457326472864e-11, 7.4493952580533382e-13,
	// 	5.8188312893328979e-15
	// };
	// // exp(x/8) for input range {-8, 8}, deg = 9, log_B = 3 // not bad
	// coeffs = {
	// 	1.0000000005494576, 0.12500000006243503, 0.0078124995710659208, 0.00032552081676327728, 
	// 	1.017257959393278e-05, 2.5431441657319724e-07, 5.2958525224794877e-09, 9.4570322004214351e-11,
	// 	1.5198698902778767e-12, 2.1098698085983081e-14
	// };
	// // exp(x/8) for input range {-8, 8}, deg = 10, log_B = 3 // bad
	// coeffs = {
	// 	0.99999999999792144, 0.12500000003427839, 0.0078125000019799475, 0.00032552082262902847, 
	// 	1.0172525727470923e-05, 2.5431408663280106e-07, 5.2982092492310126e-09, 9.4577195641773637e-11, 
	// 	1.4777845590576949e-12, 2.1050965120512778e-14, 2.63037817172648e-16
	// };

	// scaling up for coefficients 
	bool valid = true;
	double norm = 0.0;
	double bound = 1e-08;
	for (size_t i=coeffs.size()-1; i>=0; i--) {
		if (abs(coeffs[i]) < bound) {
			valid = false;
			norm = floor(bound / pow(10, floor(log10(abs(coeffs[i])))));
			for (size_t j=0; j<coeffs.size(); j++) coeffs[j] *= norm;
			break;
			}
	}
	std::vector<double> norms(n, 1/norm);
	
	evalPoly(coeffs, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_temp, ctxt_temp, timer);

	// scale down
	if (!valid) {
		evaluator.multiply_vector_reduced_error(ctxt_temp, norms, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
	}	
	//compute exp(x/B)^B by iteration
	for (int i = 0; i<log_B;i++ ){
		evaluator.multiply_reduced_error(ctxt_temp, ctxt_temp, rlks, ctxt_temp);
	    evaluator.rescale_to_next_inplace(ctxt_temp);
	}

	ctxt_out = ctxt_temp;
}

/*
 * Expm1 function
 * Reference : definition
 * Input : x
 * Output : Exp(x) - 1
*/
void Expm1_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

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
 * Expm1 function
 * Reference : definition
 * Input : x
 * Output : Exp(x) - 1
*/
void Expm1_NA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

	Plaintext pOne;
	Ciphertext cOne, cTemp;

	// generate plaintext
	std::vector<double> ones(n, 1.0);
	encoder.encode(ones, ctxt_in.scale(), pOne);
	encryptor.encrypt(pOne, cOne);

	// compute exp(x) - 1
	Exp_NA(K, n, context, encryptor, evaluator, decryptor, encoder, pk, sk, rlks, ctxt_in, cTemp, timer);
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

/*
 * Inverse function
 * Reference : https://eprint.iacr.org/2019/417.pdf
 * Input : x
 * Output : 1/x
*/
void Inv_NA2(int d, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {

    Plaintext pa, pb, pOne, pTwo, pKi;
	Ciphertext cOne, cTwo, cTemp;
	Ciphertext ca, cb, cc;

	// generate plaintext
	std::vector<double> ones(n, 1.0);
	std::vector<double> twos(n, 2.0);
	encoder.encode(ones, ctxt_in.scale(), pOne);
	encoder.encode(twos, ctxt_in.scale(), pTwo);
	encryptor.encrypt(pOne, cOne);
	encryptor.encrypt(pTwo, cTwo);

	evaluator.negate_inplace(ctxt_in); // x = -x
	ca = ctxt_in;
	cb = ctxt_in;
	evaluator.add_inplace_reduced_error(ca, cTwo);
	evaluator.add_inplace_reduced_error(cb, cOne);

	// iteration
	for (int i=1; i<d; i++) {
		// b = b**2
		evaluator.multiply_inplace_reduced_error(cb, cb, rlks);
		evaluator.rescale_to_next_inplace(cb);

		// c = b+1
		evaluator.add_reduced_error(cb, cOne, cc);

		// a = a * c
		evaluator.multiply_reduced_error(ca, cc, rlks, ca);
		evaluator.rescale_to_next_inplace(ca);
	}

	ctxt_out = ca;
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

void evalPolyEven(std::vector<double> coeffs, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &pk, SecretKey &sk, RelinKeys &rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer) {
	Plaintext ptxt;
	Ciphertext ctxt_x, ctxt_temp;
	
	// compute all powers
	int deg = (coeffs.size()-2)*2; // degree of poly
	int _deg = coeffs.size(); // odd terms + constant
	std::vector<Ciphertext> powers(deg+1);
	compute_all_powers(ctxt_in, deg, evaluator, rlks, powers, timer);	

	long n = ctxt_in.poly_modulus_degree()/2;
    std::vector<double> m_coeffs(n);

	// evaluating polynomial
	for (int j=0; j<n; j++) m_coeffs[j] = coeffs[0];
	encoder.encode(m_coeffs, ctxt_in.scale(), ptxt);
	encryptor.encrypt(ptxt, ctxt_out); // constant term

	for (int j=0; j<n; j++) m_coeffs[j] = coeffs[1];
	evaluator.multiply_vector_reduced_error(powers[1], m_coeffs, ctxt_temp);
	evaluator.rescale_to_next_inplace(ctxt_temp);	
	evaluator.add_reduced_error(ctxt_out, ctxt_temp, ctxt_out); // first-degree polynomial

	for (int i=2, j=2; i<_deg; i++, j+=2) {
		for (int k=0; k<n; k++) m_coeffs[k] = coeffs[i];
		evaluator.multiply_vector_reduced_error(powers[j], m_coeffs, ctxt_temp);
		evaluator.rescale_to_next_inplace(ctxt_temp);
		evaluator.add_reduced_error(ctxt_out, ctxt_temp, ctxt_out);
	}
}
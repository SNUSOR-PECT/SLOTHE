#include "optimize.hpp"

void getParts(std::vector<double>& parts, int d, double begin, double end) {
	int sz = pow(2, d) + 1;
	parts.resize(sz);
	double gap = (end - begin) / (sz-1);
	for (int i=0; i<sz; i++) {parts[i] = begin + gap*i;}
}


OptimizeResult _maximize_scalar_bounded(
    std::function<double(double)> func, 
    std::pair<double, double> bounds,
    double xatol, 
    int maxiter, 
    int disp) 
{
    // Constants
    const double sqrt_eps = std::sqrt(2.2e-16);
    const double golden_mean = 0.5 * (3.0 - std::sqrt(5.0));
    const double big = 134217728.0;

    // Bounds check
    if (bounds.first > bounds.second) {
        throw std::invalid_argument("The lower bound exceeds the upper bound.");
    }

    double a = bounds.first;
    double b = bounds.second;

    // Initialization
    double fulc = a + golden_mean * (b - a);
    double nfc = fulc, xf = fulc;
    double rat = 0.0, e = 0.0;
    double x = xf;
    double fx = -func(x);
    int num = 1;
    double fu = std::numeric_limits<double>::infinity();

    double ffulc = fx, fnfc = fx;
    double xm = 0.5 * (a + b);
    double tol1 = sqrt_eps * std::fabs(xf) + xatol / 3.0;
    double tol2 = 2.0 * tol1;

    if (disp > 2) {
        std::cout << " Func-count     x          f(x)          Procedure\n";
        std::cout << num << "   " << xf << "   " << fx << "       initial\n";
    }

    int flag = 0;
    std::string step = "initial";

    // Main loop
    while (std::fabs(xf - xm) > (tol2 - 0.5 * (b - a))) {
        bool golden = true;

        // Parabolic fit check
        if (std::fabs(e) > tol1) {
            golden = false;
            double r = (xf - nfc) * (fx - ffulc);
            double q = (xf - fulc) * (fx - fnfc);
            double p = (xf - fulc) * q - (xf - nfc) * r;
            q = 2.0 * (q - r);
            if (q > 0.0) p = -p;
            q = std::fabs(q);
            double r_ = e;
            e = rat;

            // Acceptable parabola check
            if ((std::fabs(p) < std::fabs(0.5 * q * r_)) &&
                (p > q * (a - xf)) &&
                (p < q * (b - xf))) {
                rat = p / q;
                x = xf + rat;
                step = "parabolic";

                if ((x - a) < tol2 || (b - x) < tol2) {
                    double si = std::copysign(1.0, xm - xf) + ((xm - xf) == 0);
                    rat = tol1 * si;
                }
            } else {
                golden = true;
            }
        }

        if (golden) {
            if (xf >= xm) {
                e = a - xf;
            } else {
                e = b - xf;
            }
            rat = golden_mean * e;
            step = "golden";
        }

        double si = std::copysign(1.0, rat) + (rat == 0);
        x = xf + si * std::max(std::fabs(rat), tol1);
        fu = -func(x);
        num++;

        if (disp > 2) {
            std::cout << num << "   " << x << "   " << fu << "   " << step << "\n";
        }

        if (fu <= fx) {
            if (x >= xf) {
                a = xf;
            } else {
                b = xf;
            }
            fulc = nfc;
            ffulc = fnfc;
            nfc = xf;
            fnfc = fx;
            xf = x;
            fx = fu;
        } else {
            if (x < xf) {
                a = x;
            } else {
                b = x;
            }
            if ((fu <= fnfc) || (nfc == xf)) {
                fulc = nfc;
                ffulc = fnfc;
                nfc = x;
                fnfc = fu;
            } else if ((fu <= ffulc) || (fulc == xf) || (fulc == nfc)) {
                fulc = x;
                ffulc = fu;
            }
        }

        xm = 0.5 * (a + b);
        tol1 = sqrt_eps * std::fabs(xf) + xatol / 3.0;
        tol2 = 2.0 * tol1;

        if (num >= maxiter) {
            flag = 1;
            break;
        }
    }

    if (std::isnan(xf) || std::isnan(fx) || std::isnan(fu)) {
        flag = 2;
    }

    double fval = fx;
    bool success = (flag == 0);
    std::string message;

    switch (flag) {
        case 0:
            message = "Solution found.";
            break;
        case 1:
            message = "Maximum number of function calls reached.";
            break;
        case 2:
            message = "NaN encountered.";
            break;
        default:
            message = "Unknown error.";
            break;
    }

    if (disp > 0) {
        // std::cout << "Final result: x = " << xf << ", f(x) = " << -fval << ", iter = " << num << ", status = " << message << "\n";
        std::cout << xf << ", " << -fval << ", " << num << "\n";
    }

    return {fval, flag, success, message, xf, num, num};
}

OptimizeResult _runRandPoints(
    std::function<double(double)> func,
    std::pair<double, double> bounds,
    double xatol,
    int maxiter,
    int disp,
    int opt) {

    double xs, xMax, yMax;
    int cnt = 0; // count
    int flag = 0;
    double delta = 1e-6;

    // Bounds check
    if (bounds.first > bounds.second) {
        throw std::invalid_argument("The lower bound exceeds the upper bound.");
    }

    double a = bounds.first;
    double b = bounds.second;

    std::random_device rd; std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(a, b);

    // Check endpoints
    if (opt) {
        if (func(a) > xatol) {
            xMax = a; yMax = func(a);
            cnt = 1;
            flag = 3; // endpoint > prec
        }
        if (func(b) > xatol) {
            xMax = b; yMax = func(b);
            cnt = 1;
            flag = 3; // endpoint > prec
        }
    }

    // Main Loop
    double startPoint = dist(gen);
    double point = startPoint;
    double _xMax = std::numeric_limits<double>::min();
    double _yMax = std::numeric_limits<double>::min();

    while(1) {
        double cur = func(point);
        double next = func(point+delta);
        double grad = (next - cur) / delta; // numerical derivative

        if (cur > _yMax) {
            _xMax = point;
            _yMax = cur;
        }

        // break loop when yMax is less than prec
        if (opt && _yMax > xatol) {flag = 3; break;}

        // move
        if (fabs(grad) < delta) break;
        else if (grad > 0) point = point + grad * xatol;
        else point = point - grad * xatol;

        // end if point is out of range
        if (point < a || point > b) break;

        // end if cnt >= maxiter
        cnt += 1;
        if (cnt >= maxiter) {flag = 1; break;}
    }

    xs = startPoint;
    xMax = _xMax;
    yMax = _yMax;
    
    if (std::isnan(xMax) || std::isnan(yMax)) {
        flag = 2;
    }

    bool success = (flag == 0);
    std::string message;

    switch (flag) {
        case 0:
            message = "Solution found.";
            break;
        case 1:
            message = "Maximum number of function calls reached.";
            break;
        case 2:
            message = "NaN encountered.";
            break;
        case 3:
            message = "Solution not found : out of precision";
            break;
        default:
            message = "Unknown error.";
            break;
    }

    if (disp > 0) {
        // std::cout << "Final result: x = " << xMax << ", f(x) = " << yMax << ", iter = " << cnt << ", status = " << message << "\n";
        std::cout << xMax << ", " << yMax << ", " << cnt << "\n";
    }

    return {yMax, flag, success, message, xMax, cnt, cnt};
}

OptimizeResult _runBinTreePoints(
    std::function<double(double)> func,
    std::pair<double, double> bounds,
    double xatol,
    int maxiter,
    int d,
    int disp,
    int opt) {
  
    // Bounds check
    if (bounds.first > bounds.second) {
        throw std::invalid_argument("The lower bound exceeds the upper bound.");
    }

    double a = bounds.first;
    double b = bounds.second;

    // get binary partitions
    std::vector<double> parts;
	getParts(parts, d, a, b);

    int cnt = parts.size()-1; // = pow(2, d)
	std::vector<int> iters(cnt, 0);
	std::vector<double> xs(cnt), xMax(cnt), yMax(cnt);
    int totalCnt = 0;
    int flag = 0;
    double delta = 1e-6;
    maxiter /= cnt;

    // Check endpoints
    if (opt) {
        if (func(a) > xatol) {
            xMax[0] = a; yMax[0] = func(a);
            cnt = 1;
            flag = 3; // endpoint > prec
        }
        if (func(b) > xatol) {
            xMax[0] = b; yMax[0] = func(b);
            cnt = 1;
            flag = 3; // endpoint > prec
        }
    }

    std::vector<std::pair<double,double>> mixIdx(cnt); // [0] [-1] ..
	for (int i=0;i<cnt;i++) { 
		if (i%2==0)
			mixIdx[i] = {parts[i/2], parts[i/2+1]}; 
		else
			mixIdx[i] = {parts[cnt-(i/2)-1], parts[cnt-(i/2)]};
	}

    // Main Loop
    for (int i=0; i<cnt; i++) {
		// for random in partial range start point
		double begin = mixIdx[i].first, end = mixIdx[i].second;
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(begin, end);
		double startPoint = dist(gen);
		
		double point = startPoint; // init
		double _xMax = std::numeric_limits<double>::min();
		double _yMax = std::numeric_limits<double>::min();
		
		int iter = 0;
		while(1) {
			iter += 1;
			double cur = func(point); // current point
			double next = func(point+delta); // next point
			double grad = (next - cur) / delta; // numerical derivative
			
			// find _yMax
			if (cur > _yMax) {
				_xMax = point;
				_yMax = cur;
			}

			// break loop when yMax is less than prec
            if (opt && _yMax > xatol) {flag = 3; break;}

			// move
            if (fabs(grad) < delta) break;
            else if (grad > 0) point = point + grad * xatol;
            else point = point - grad * xatol;
					
			// end if point is out of range
			if (point < begin || point > end) break;

            // end if cnt >= maxiter
            iter += 1;
            if (iter >= maxiter) {flag = 1; break;}
		}

		// Check end points
		if (func(begin) > _yMax) {
			_xMax = begin; _yMax = func(begin);
		}
		if (func(end) > _yMax) {
			_xMax = end; _yMax = func(end);
		}

		iters[i] = iter;
		xs[i] = startPoint;
		xMax[i] = _xMax;
		yMax[i] = _yMax;
	}
    
    int idx = max_element(yMax.begin(), yMax.end()) - yMax.begin();
	double xMaxFin = xMax[idx];
	double yMaxFin = yMax[idx];

    int iterSum = accumulate(iters.begin(), iters.end(), 0.0);

    bool success = (flag == 0);
    std::string message;

    switch (flag) {
        case 0:
            message = "Solution found.";
            break;
        case 1:
            message = "Maximum number of function calls reached.";
            break;
        case 2:
            message = "NaN encountered.";
            break;
        case 3:
            message = "Solution not found : out of precision";
            break;
        default:
            message = "Unknown error.";
            break;
    }

    if (disp > 0) {
        // std::cout << "Final result: x = " << xMaxFin << ", f(x) = " << yMaxFin << ", iter = " << iterSum << ", status = " << message << "\n";
        std::cout << xMaxFin << ", " << yMaxFin << ", " << iterSum << "\n";
    }

    return {yMaxFin, flag, success, message, xMaxFin, iterSum, iterSum};
}


// int main() {
//     auto func = [](double x) { return (x - 2.0) * (x - 2.0); };
//     std::pair<double, double> bounds = {0.0, 4.0};

//     OptimizeResult result = _minimize_scalar_bounded(func, bounds, 1e-5, 500, 3);

//     std::cout << "Minimum at x = " << result.x << ", with function value = " << result.fun << "\n";
//     return 0;
// }

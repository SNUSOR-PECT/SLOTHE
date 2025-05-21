// goldschmidtDivision
double _inverse(double x, int d) {
    double a=2-x, b=1-x;
    for (int i=0;i<d-1;i++) {
        b = b*b;
        a = a*(1+b);
    }
    return a;
}
#include <iostream>
#include <math.h>

using namespace std;
int main(){
    //uint ix = 0;
    uint jx = 0x7FF00000;
    uint lx = 0x00077777;
    uint ix = jx & 0x7fffffff;
    bool sign = -1;
    double temp = 0;



    //from binary format, create exponent and mantissa
    uint exponent = jx>>20;
    uint64_t mantissa = (ix%20)<<32 + lx;

    //translate into the actual double precision 
    temp = sign * (1+mantissa*pow(2,-52))*pow(2,exponent);
    cout<<temp<<endl;

return 0;
}
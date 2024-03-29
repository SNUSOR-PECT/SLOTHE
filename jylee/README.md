## Compile & Run 

```
g++ trans.cpp
./a.out
```
## Input example :
Check inf
ugt i32 %6, 2146435071

Check NaN
sgt i64 %2, -1

Check 22
ult i32 %6, 1077280768

## Output example : 
Enter your input: ult i32 %6, 1077280768
unsigned less than
exponent : 10000000011
mantissa : 00000000000001100000000000000000

x value is : 22

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define packFloat32(f)   (packFloat((f), 32,  8))
#define packFloat64(f)   (packFloat((f), 64, 11))
#define unpackFloat32(i) (unpackFloat((i), 32, 8))
#define unpackFloat64(i) (unpackFloat((i), 64, 11))

uint64_t packFloat(long double f, unsigned bits, unsigned expbits)
{
  long double f_norm;
  int shift;
  long long sign;
  long long exp;
  long long significand;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bits
  
  if(f == 0.0)            // 0 special case
    return 0;
  
  if(f < 0)               // check sign and begin normalization
  {
    sign  = 1;
    f_norm = -f;
  }
  else
  {
    sign  = 0;
    f_norm = f;
  }
  
  shift = 0;              // get the normalized from of f an track the exponent
  while(f_norm >= 2.0)    // f_norm = [2.0, ...]
  {
    f_norm /= 2.0;         // x >> 1 | x/2 OR x*0.5
    shift++;
  }
  while(f_norm < 1.0)
  {
    f_norm *= 2.0;        // x << 1 | x*2
    shift--;
  }
  f_norm = f_norm - 1.0;
  
  significand = f_norm * ((1LL << significandbits) + 0.5f); // calculte binary form (non-float) of significand data
  
  exp = shift + ((1 << (expbits - 1)) - 1); // shift + bias
  
  return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
}

long double unpackFloat(uint64_t i, unsigned bits, unsigned expbits)
{
  long double result;
  long long shift;
  unsigned bias;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bit
  
  if(i == 0)
    return 0.0;
  
  // calculate significand
  result  = (i & ((1LL << significandbits) - 1)); // mask
  result /= (1LL << significandbits);             // convert back to float
  result += 1.0f;                                 // add 1 back
  
  // calculate exponent
  bias = (1 << (expbits - 1)) - 1;
  shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
  while(shift > 0)
  {
    result *= 2.0;
    shift--;
  }
  while(shift < 0)
  {
    result /= 2.0;
    shift++;
  }
  
  // sign it
  result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;
  
  return result;
}

int main()
{
  const float PI = 3.1415925f;
  uint64_t i = packFloat32(PI);
  printf("PI %.7f\n", PI);
  printf("-> 0x%08" "x" "\n", i);
  double f = unpackFloat32(i);
  printf("PI %.7f\n", f);
  std::cout << sizeof(uint64_t) << std::endl;
  
  std::cout << std::endl;
  
  std::cout << "short        : " << sizeof(short) << std::endl;           // 2
  std::cout << "short int    : " << sizeof(short int) << std::endl;       // 2
  std::cout << "int          : " << sizeof(int) << std::endl;             // 4
  std::cout << "long         : " << sizeof(long ) << std::endl;           // 4
  std::cout << "long int     : " << sizeof(long int) << std::endl;        // 4
  std::cout << "long long int: " << sizeof(long long int) << std::endl;   // 8
  
  std::cout << "int64_t      : " << sizeof(int64_t) << std::endl;         // 8
  
  std::cout << "float        : " << sizeof(float) << std::endl;           // 4
  std::cout << "double       : " << sizeof(double) << std::endl;          // 8
  std::cout << "long double  : " << sizeof(long double) << std::endl;     // 12
  
  return 0;
}

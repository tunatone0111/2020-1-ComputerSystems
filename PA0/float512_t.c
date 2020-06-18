#include "float512_t.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

int e = 27, f = 484, bias=(1<<(27-1))-1;

bool isdenorm32(float f){
  pointer pf = (pointer)&f;
  pf[3] &= 0x7F; // unconsider sign bit
  int mask = 0x7F800000;
  if((pf[3] == 0x7F) && (pf[2] == 0x80)) return true;
  if((pf[3] == 0x80) && (pf[2] == 0x00)) return true;
}

bool isinf32(float f){
  pointer pf = (pointer)&f;
  pf[3] &= 0x7F; // unconsider sign bit
  int mask = 0x7F800000;
  if(memcmp((pointer)&mask, pf, 4) != 0) return false;
  return true;
}

bool isnan32(float f){
  pointer pf = (pointer)&f;
  pf[3] &= 0x7F; // unconsider sign bit
  if((pf[2] & 0x80) != 0x80) return false; //normalized
  if(isinf32(f)) return false;
  return true;
}

void uint_bitshift_left(pointer p, size_t shift, size_t size){
  int carry = 0;
  while(shift--){
    for(int i = 0; i<size; i++){
      int next = (p[i] & 0x80) ? 1 : 0;
      p[i] = carry | (p[i] << 1);
      carry = next;
    }
  }
}

float512_t round512(pointer psign, pointer pexp, pointer pfrac){
	float512_t result;

	// if(exp > bias || exp < -bias){
	// 	memset(&result, 0x00, sizeof(float512_t));
	// 	printf("Exponent overflow\n");
	// 	return result;
	// }

	*pexp |= (*psign)<<3;
	*pexp <<= 4;
	memcpy(&result, pexp, 4);

	return result;
}

float512_t int_to_float512(int i)
{
	float512_t result;
	unsigned char sign = 0x01, mask = 0x80; //sign: 00000001 mask: 10000000
	int k = 0, exp = 0, cnt = 0;
	pointer psign = &sign, pexp = (pointer)&exp, pfrac, pi = (pointer)&i;

	sign &= pi[3];
	pfrac = (pointer)calloc(1, 64);


	if(i == 0){
		result = round512(psign, pexp, pfrac);
	}

	exp = bias;

	while(mask & pi[k++] == 0){
		i <<= 1;
		cnt++;
	}
	i <<= 1;
	exp += (sizeof(int)*8)-1-cnt;

	memcpy(pfrac, (pointer)&i, sizeof(int));

	result = round512(psign, pexp, pfrac);

	return result;
}

float512_t float32_to_float512(float f)
{
	float512_t result;

  pointer pf = (pointer)&pf, pres = (pointer)&result;
  memset(pres, 0x00, sizeof(float512_t));
  unsigned char sign = 0x80, temp;
  sign &= pf[3];

  if(isinf32(f)){
    pres[63] |= 0x7F;
    pres[62] |= 0xFF;
    pres[61] |= 0xFF;
    pres[60] |= 0xF0;
    pres[63] |= sign;
    return result;
  }
  if(isnan32(f)){
    pres[63] |= 0x7F;
    pres[62] |= 0xFF;
    pres[61] |= 0xFF;
    pres[60] |= 0xF1;
    pres[63] |= sign;
    return result;
  }
  pres[63] |= sign;
  memcpy(pres+57, pf, 3);
  pres[59] &= 0x7F;

  uint_bitshift_left(pres+57, 5, 4);

	return result;
}

bool float512_equal(const float512_t lhs, const float512_t rhs)
{
	// TODO: Put something!

	return true;
}

bool float512_is_nan(const float512_t f)
{
	// TODO: Put something!
  pointer temp = (pointer)calloc(1, 4);
  pointer mask = (pointer)malloc(64);
  memcpy(temp, ((pointer)&f)+60, 4);
  memset(mask, 0xFF, 60);
  mask[60] = 0x0F;
  memset(mask+61, 0x00, 3);
  int mask2 = 0x7FFFFFF0;
  if((mask2 & *temp) != 0x7FFFFFF0) return false;
  if(float512_is_inf(f)) return false;
  free(temp);
  free(mask);
	return true;
}

bool float512_is_inf(const float512_t f)
{
  pointer temp = (pointer)calloc(1, 4);
  pointer mask = (pointer)malloc(64);
  memcpy(temp, ((pointer)&f)+60, 4);
  memset(mask, 0xFF, 60);
  mask[60] = 0x0F;
  memset(mask+61, 0x00, 3);
  int mask2 = 0x7FFFFFF0;
  if(mask2 & *temp != 0x7FFFFFF0) return false;
  if((0x0F & ((pointer)&f)[60]) != 0) return false;
  free(temp);
  free(mask);
	return true;
}

float512_t float512_neg(float512_t f)
{
	float512_t result;

	// TODO: Put something!

	return result;
}

float512_t float512_add(float512_t lhs, float512_t rhs)
{
	float512_t result;

	// TODO: Put something!

	return result;
}

float512_t float512_mul(float512_t lhs, float512_t rhs)
{
	float512_t result;

	// TODO: Put something!

	return result;
}

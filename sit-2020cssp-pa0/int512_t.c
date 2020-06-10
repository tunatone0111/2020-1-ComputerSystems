#include "int512_t.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void uint_bitshift_right(pointer p, size_t shift, size_t size){
  int carry = 0;
  while(shift--){
    for(int i = size-1; i>=0; i--){
      int next = (p[i] & 0x01) ? 0x80 : 0;
      p[i] = carry | (p[i] >> 1);
      carry = next;
    }
  }
}

int512_t int_to_int512(int i)
{
	int512_t result;

	// TODO: Put something!
	int mask = 0x80000000;
	bool isminus = mask&i ? true : false;
	if(isminus == true){
    memset(result.data, 0xff, sizeof(result.data));
    result.data[0] = i;
	}
  else{
    memset(result.data, 0x00, sizeof(result.data));
    result.data[0] = i;
  }
	return result;
}

bool int512_equal(const int512_t lhs, const int512_t rhs)
{
	// TODO: Put something!
	pointer plhs = (pointer)&lhs, prhs = (pointer)&rhs;
	for(int i = 0; i < sizeof(int512_t); i++){
		if(plhs[i] != prhs[i]) return false;
	}
	return true;
}

bool int512_greater(const int512_t lhs, const int512_t rhs)
{
	int512_t result = int512_add(lhs, int512_mul(int_to_int512(-1), rhs));
  if(result.data[63] & 0x80000000 == 1) return false;
	return true;
}

int512_t int512_add(int512_t lhs, int512_t rhs)
{
	int512_t result;
	unsigned char c = 0x00, s = 0x00;
	pointer plhs = (pointer)&lhs, prhs = (pointer)&rhs, pres = (pointer)&result;
	for(int i = 0; i < sizeof(int512_t); i++){
		s = c + plhs[i] + prhs[i];
		c = (plhs[i] + prhs[i])>>8;
		pres[i] = s;
	}
	return result;
}

int512_t int512_mul(int512_t lhs, int512_t rhs)
{
	int512_t result, temp;
	pointer plhs = (pointer)&lhs, prhs = (pointer)&rhs, pres = (pointer)&result;
  unsigned char mask;
  memset(&result, 0x00, sizeof(int512_t));

	for(int i = 0; i < 512; i++){
		mask = 1u<<(i%8);
		if((mask & prhs[i/8]) != 0x00)
			result = int512_add(result, lhs);
		uint_bitshift_left(plhs, 1, sizeof(int512_t));
	}

	return result;
}

int512_t int512_div(int512_t lhs, int512_t rhs)
{
  int512_t result;
	pointer plhs = (pointer)&lhs, prhs = (pointer)&rhs, pres = (pointer)&result;
  unsigned char rsign = 0x80, lsign = 0x80, sign = 0x00;
  int cnt = 0;
  rsign &= prhs[63];
  lsign &= plhs[63];
  sign = lsign^rsign;
  if(lsign == 0x80){
    lhs = int512_mul(int_to_int512(-1), lhs);
  }
  if(rsign == 0x80){
    rhs = int512_mul(int_to_int512(-1), rhs);
  }
  memset((pointer)&result, 0x00, sizeof(int512_t));

  while((0x80 & prhs[63]) == 0x00){
    cnt++;
    uint_bitshift_left(prhs, 1, sizeof(int512_t));
  }
  uint_bitshift_right(prhs, 1, sizeof(int512_t));



  for(int i = 0; i<cnt; i++){
    lhs = int512_add(lhs, int512_mul(int_to_int512(-1), rhs));

    if((plhs[63]&0x80) == 0x80){
      lhs = int512_add(lhs, rhs);
      uint_bitshift_left(pres, 1, sizeof(int512_t));
      pres[0] |= 0x00;
    }else{
      uint_bitshift_left(pres, 1, sizeof(int512_t));
      pres[0] |= 0x01;
    }
    uint_bitshift_right(prhs, 1, sizeof(int512_t));

  }

  if(sign == 0x80){
    result = int512_mul(int_to_int512(-1), result);
  }

  return result;
}

int512_t int512_mod(int512_t lhs, int512_t rhs)
{
	int512_t result;

	result = int512_add(lhs, int512_mul(int_to_int512(-1), int512_div(lhs, rhs)));

	return result;
}

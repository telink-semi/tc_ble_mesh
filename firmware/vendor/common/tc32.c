/*
 * tc32.c
 *
 *  Created on: Jun 21, 2013
 *  these are the platform dependent libc functions that
 *  needs to patch up here
 *      Author: peters
 */


#include "tc32.h"
// the fastest clzsi
// https://code.woboq.org/llvm/compiler-rt/lib/builtins/clzsi2.c.html

#define ASM_VERSION 1
#if ASM_VERSION
__attribute__((naked))
#endif
int __clzsi2(unsigned int x){
#if(ASM_VERSION)					// consume 50% of the time that the C version
	asm("tpush		{r4, lr}");
	asm("tmov  		r4, #1");
	asm("tshftl 	r4, #30");		// BIT(30) is the Z flag

	asm("tshftr		r3, r0, #16");
	asm("tmrcs 		r1");
	asm("tand 		r1, r4");		// BIT(30) is the Z flag
	asm("tshftr 	r1, #26");		// first part of the CLZ
	asm("tshftl 	r0, r0, r1");	// shift out if zeroes
	asm("tmov  		r2, r1");		// r2 == cnt

	asm("tshftr		r3, r0, #24");
	asm("tmrcs 		r1");
	asm("tand 		r1, r4");
	asm("tshftr 	r1, #27");
	asm("tshftl 	r0, r0, r1");
	asm("tadd  		r2, r2, r1");
	    									
	asm("tshftr		r3, r0, #28");
	asm("tmrcs 		r1");
	asm("tand 		r1, r4");
	asm("tshftr 	r1, #28");
	asm("tshftl 	r0, r0, r1");
	asm("tadd  		r2, r2, r1");

	asm("tshftr		r3, r0, #30");
	asm("tmrcs 		r1");
	asm("tand 		r1, r4");
	asm("tshftr 	r1, #29");
	asm("tshftl 	r0, r0, r1");
	asm("tadd  		r2, r2, r1");

	asm("tshftr		r0, r0, #30");
	asm("tshftr		r1, r0, #1");
	asm("tneg		r1, r1");
	asm("tmovn		r1, r1");
	asm("tmov		r3, #2");
	asm("tsub		r3, r0");
	asm("tand 		r3, r1");
	asm("tadd 		r0, r2, r3");

	asm("tpop		{r4, pc}");
#else
	// 1% faster than the org version showed above link
	unsigned int t = (!(x & 0xFFFF0000)) << 4;  /* if (x is small) t = 16 else 0 */
	x <<= t;     								/* x = [0 - 0xFFFF] */
	unsigned int r = t;       					/* r = [0, 16] */
	t = (!(x & 0xFF000000)) << 3;
	x <<= t;       								/* x = [0 - 0xFF] */
	r += t;            							/* r = [0, 8, 16, 24] */
	t = (!(x & 0xF0000000)) << 2;
	x <<= t;       								/* x = [0 - 0xF] */
	r += t;            							/* r = [0, 4, 8, 12, 16, 20, 24, 28] */
	t = (!(x & 0xC0000000)) << 1;
	x <<= t;       								/* x = [0 - 3] */
	r += t;            							/* r = [0 - 30] and is even */
	x >>= 30;
	return r + ((2 - x) & -(!(x & 2)));
#endif
	return 1;
}

int __ctzsi2(unsigned int x){
    int t = (!(x & 0x0000FFFF)) << 4;  	/* if (x has no small bits) t = 16 else 0 */
    x >>= t;           						/* x = [0 - 0xFFFF] + higher garbage bits */
    unsigned int r = t;       				/* r = [0, 16]  */
    t = (!(x & 0x00FF)) << 3;
    x >>= t;           						/* x = [0 - 0xFF] + higher garbage bits */
    r += t;           						 /* r = [0, 8, 16, 24] */
    t = (!(x & 0x0F)) << 2;
    x >>= t;          						 /* x = [0 - 0xF] + higher garbage bits */
    r += t;            						/* r = [0, 4, 8, 12, 16, 20, 24, 28] */
    t = (!(x & 0x3)) << 1;
    x >>= t;
    x &= 3;            						/* x = [0 - 3] */
    r += t;            						/* r = [0 - 30] and is even */
    return r + ((2 - (x >> 1)) & -(!(x & 1)));
}

int __clzsi(unsigned short x){
    unsigned short t = (!(x & 0xFF00)) << 3;
    x <<= t;       									/* x = [0 - 0xFF] */
    unsigned int r = t;            					/* r = [0, 8, 16, 24] */
    t = (!(x & 0xF000)) << 2;
    x <<= t;       								/* x = [0 - 0xF] */
    r += t;            								/* r = [0, 4, 8, 12, 16, 20, 24, 28] */
    t = (!(x & 0xC000)) << 1;
    x <<= t;       								/* x = [0 - 3] */
    r += t;            								/* r = [0 - 30] and is even */
	x >>= 14;
    return r + ((2 - x) & -((x & 2) == 0));
}

int __ctzsi(unsigned short x){
    int t = ((x & 0x00FF) == 0) << 3;
    x >>= t;           						/* x = [0 - 0xFF] + higher garbage bits */
    unsigned int r = t;           						 /* r = [0, 8, 16, 24] */
    t = ((x & 0x0F) == 0) << 2;
    x >>= t;          						 /* x = [0 - 0xF] + higher garbage bits */
    r += t;            						/* r = [0, 4, 8, 12, 16, 20, 24, 28] */
    t = ((x & 0x3) == 0) << 1;
    x >>= t;
    x &= 3;            						/* x = [0 - 3] */
    r += t;            						/* r = [0 - 30] and is even */
    return r + ((2 - (x >> 1)) & -(!(x & 1)));
}

typedef long long di_int;
typedef unsigned long long du_int;
typedef di_int fixint_t ;
typedef du_int fixuint_t;

struct DWstruct {
	unsigned int low;
	int high;
};
typedef union
{
	struct DWstruct s;
	long long ll;
} DWunion;

struct UDWstruct {
	unsigned int low;
	unsigned int high;
};
typedef union
{
	struct UDWstruct s;
	unsigned long long ll;
} UDWunion;

unsigned long long __umulsidi3(unsigned int u, unsigned int v) {
    unsigned int  u0,   v0,   w0;
    unsigned int  u1,   v1,   w1,   w2,   t;
    unsigned int  x, y;

    u0   =   u & 0xFFFF;
    u1   =   u >> 16;
    v0   =   v & 0xFFFF;
    v1   =   v >> 16;
    w0   =   u0 * v0;
    t    =   u1 * v0 + (w0 >> 16);
    w1   =   t & 0xFFFF;
    w2   =   t >> 16;
    w1   =   u0 * v1 + w1;

    //   x为高32位,   y为低32位
    x = u1 * v1 + w2 + (w1 >> 16);
    y = u * v;

    return(((unsigned long long)x << 32) | y);
}

long long __muldi3(long long u, long long v){
	const DWunion uu = {.ll = u};
	const DWunion vv = {.ll = v};
	DWunion w = {.ll = __umulsidi3(uu.s.low, vv.s.low)};

	w.s.high += ((unsigned long) uu.s.low * (unsigned long) vv.s.high
		+ (unsigned long) uu.s.high * (unsigned long) vv.s.low);

	return w.ll;
}

long long __ashrdi3(long long u, int b){
	DWunion uu, w;
	int bm;

	if (b == 0)
		return u;

	uu.ll = u;
	bm = 32 - b;

	if (bm <= 0) {
		/* w.s.high = 1..1 or 0..0 */
		w.s.high =
		    uu.s.high >> 31;
		w.s.low = uu.s.high >> -bm;
	} else {
		const unsigned int carries = (unsigned int) uu.s.high << bm;

		w.s.high = uu.s.high >> b;
		w.s.low = ((unsigned int) uu.s.low >> b) | carries;
	}
	return w.ll;
	
}

long long __ashldi3(long long a, int b) {
  const int bits_in_word = (int)(sizeof(int) * CHAR_BIT);
  DWunion uu;
  DWunion w;
  uu.ll = a;
  if (b & bits_in_word) /* bits_in_word <= b < bits_in_dword */ {
    w.s.low = 0;
    w.s.high = uu.s.low << (b - bits_in_word);
  } else /* 0 <= b < bits_in_word */ {
    if (b == 0)
      return a;
    w.s.low = uu.s.low << b;
    w.s.high = (uu.s.high << b) | (uu.s.low >> (bits_in_word - b));
  }
  return w.ll;
}

// https://code.woboq.org/llvm/compiler-rt/lib/builtins/clzdi2.c.html
int __clzdi2(long long a)
{
    DWunion x;
    x.ll = a;
    const int f = -(x.s.high == 0);
    return __clzsi2((x.s.high & ~f) | (x.s.low & f)) +
           (f & ((int)(sizeof(int) * CHAR_BIT)));
}

long long __lshrdi3(long long a, int b)
{
    const int bits_in_word = (int)(sizeof(int) * CHAR_BIT);
    DWunion input;
    DWunion result;
    input.ll = a;
    if (b & bits_in_word)  /* bits_in_word <= b < bits_in_dword */
    {
        result.s.high = 0;
        result.s.low = input.s.high >> (b - bits_in_word);
    }
    else  /* 0 <= b < bits_in_word */
    {
        if (b == 0)
            return a;
        result.s.high  = input.s.high >> b;
        result.s.low = (input.s.high << (bits_in_word - b)) | (input.s.low >> b);
    }
    return result.ll;
}

// https://github.com/DynamoRIO/dynamorio/blob/master/third_party/libgcc/udivmoddi4.c
unsigned long long __udivmoddi4 (unsigned long long n, unsigned long long d, unsigned long long *rp)
{
  unsigned long long q = 0, r = n, y = d;
  unsigned int lz1, lz2, i, k;

  /* Implements align divisor shift dividend method. This algorithm
     aligns the divisor under the dividend and then perform number of
     test-subtract iterations which shift the dividend left. Number of
     iterations is k + 1 where k is the number of bit positions the
     divisor must be shifted left  to align it under the dividend.
     quotient bits can be saved in the rightmost positions of the dividend
     as it shifts left on each test-subtract iteration. */

  if (y <= r)
  {
      lz1 = __clzdi2 (d);
      lz2 = __clzdi2 (n);

      k = lz1 - lz2;
      y = (y << k);

      /* Dividend can exceed 2 ^ (width - 1) - 1 but still be less than the
         aligned divisor. Normal iteration can drops the high order bit
         of the dividend. Therefore, first test-subtract iteration is a
         special case, saving its quotient bit in a separate location and
         not shifting the dividend. */
      if (r >= y)
      {
          r = r - y;
          q =  (1ULL << k);
      }

      if (k > 0)
      {
          y = y >> 1;

          /* k additional iterations where k regular test subtract shift
            dividend iterations are done.  */
          i = k;
          do
          {
              if (r >= y)
                r = ((r - y) << 1) + 1;
              else
                r =  (r << 1);
              i = i - 1;
          } while (i != 0);

          /* First quotient bit is combined with the quotient bits resulting
             from the k regular iterations.  */
          q = q + r;
          r = r >> k;
          q = q - (r << k);
      }
  }

  if (rp)
    *rp = r;
  return q;
}

// https://github.com/DynamoRIO/dynamorio/blob/master/third_party/libgcc/udivmoddi4.c
long long __moddi3 (long long u, long long v)
{
  int c = 0;
  DWunion uu = {.ll = u};
  DWunion vv = {.ll = v};
  long long w;

  if (uu.s.high < 0)
    c = ~c,
    uu.ll = -uu.ll;
  if (vv.s.high < 0)
    vv.ll = -vv.ll;

  (void) __udivmoddi4 (uu.ll, vv.ll, (unsigned long long*)&w);
  if (c)
    w = -w;

  return w;
}

long long __divdi3(long long a, long long b)
{
    const int bits_in_dword_m1 = (int)(sizeof(long long) * CHAR_BIT) - 1;
    long long s_a = a >> bits_in_dword_m1;           /* s_a = a < 0 ? -1 : 0 */
    long long s_b = b >> bits_in_dword_m1;           /* s_b = b < 0 ? -1 : 0 */
    a = (a ^ s_a) - s_a;                         /* negate if s_a == -1 */
    b = (b ^ s_b) - s_b;                         /* negate if s_b == -1 */
    s_a ^= s_b;                                  /*sign of quotient */
    return (__udivmoddi4(a, b, (unsigned long long*)0) ^ s_a) - s_a;  /* negate if s_a == -1 */
}


#define clz(a) (sizeof(a) == sizeof(unsigned long long) ? __clzdi2(a) : __ctzsi2(a))
// Adapted from Figure 3-40 of The PowerPC Compiler Writer's Guide
static fixuint_t __udivXi3(fixuint_t n, fixuint_t d) {
  const unsigned N = sizeof(fixuint_t) * CHAR_BIT;
  // d == 0 cases are unspecified.
  unsigned sr = (d ? clz(d) : N) - (n ? clz(n) : N);
  // 0 <= sr <= N - 1 or sr is very large.
  if (sr > N - 1) // n < d
    return 0;
  if (sr == N - 1) // d == 1
    return n;
  ++sr;
  // 1 <= sr <= N - 1. Shifts do not trigger UB.
  fixuint_t r = n >> sr;
  n <<= N - sr;
  fixuint_t carry = 0;
  for (; sr > 0; --sr) {
    r = (r << 1) | (n >> (N - 1));
    n = (n << 1) | carry;
    // Branch-less version of:
    // carry = 0;
    // if (r >= d) r -= d, carry = 1;
    const fixint_t s = (fixint_t)(d - r - 1) >> (N - 1);
    carry = s & 1;
    r -= d & s;
  }
  n = (n << 1) | carry;
  return n;
}
// Mostly identical to __udivXi3 but the return values are different.
static fixuint_t __umodXi3(fixuint_t n, fixuint_t d) {
  const unsigned N = sizeof(fixuint_t) * CHAR_BIT;
  // d == 0 cases are unspecified.
  unsigned sr = (d ? clz(d) : N) - (n ? clz(n) : N);
  // 0 <= sr <= N - 1 or sr is very large.
  if (sr > N - 1) // n < d
    return n;
  if (sr == N - 1) // d == 1
    return 0;
  ++sr;
  // 1 <= sr <= N - 1. Shifts do not trigger UB.
  fixuint_t r = n >> sr;
  n <<= N - sr;
  fixuint_t carry = 0;
  for (; sr > 0; --sr) {
    r = (r << 1) | (n >> (N - 1));
    n = (n << 1) | carry;
    // Branch-less version of:
    // carry = 0;
    // if (r >= d) r -= d, carry = 1;
    const fixint_t s = (fixint_t)(d - r - 1) >> (N - 1);
    carry = s & 1;
    r -= d & s;
  }
  return r;
}


du_int __udivdi3(du_int a, du_int b) {
  return __udivXi3(a, b);
}

du_int __umoddi3(du_int a, du_int b) {
  return __umodXi3(a, b);
}




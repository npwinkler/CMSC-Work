/* 
 * CS:APP Data Lab 
 * 
 * Luke Bertels, lwbertels
 * Nolan Winkler, nolanwinkler
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * absVal - shift right 31, add this to given number and then bitwise xor
to find the spots that are different */
int absVal(int x) {
  int y=x>>31;
  return (x+y)^y;
}

/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
	int sum = x+y;
	int x_neg = ((x>>31)&1);
	int y_neg = ((y>>31)&1);
	int sum_pos = (sum>>31)+1;
	int x_pos = !x_neg;
	int y_pos = !y_neg;
	int sum_neg = !!(sum>>31);
	int neg_over = x_neg&y_neg&sum_pos;
	int pos_over = x_pos&y_pos&sum_neg;
	return (!neg_over)&(!pos_over);
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
  int y=(0x55<<24)^(0x55<<16)^(0x55<<8)^(0x55); /*construct 0101...0101 using
small constant*/
  return !((x&y)^y);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
  int mshift = (m << 3);
  int nshift = (n << 3);
  int bytem = 0xFF & (x >> mshift); /* isolate the bytes */
  int byten = 0xFF & (x >> nshift);
  int xholes= (x & (~(0xFF << mshift))) & (~(0xFF << nshift));
  return (xholes|(bytem << nshift))|(byten << mshift); 
/*replace where old bytes used to be with the new bytes */
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int y = x>>31;
    return (x + (y & ((1<<n) + ~0))) >> n;
}
/*
 * ezThreeFourths - multiplies by 3/4 rounding toward 0,
 *   Should exactly duplicate effect of C expression (x*3/4),
 *   including overflow behavior.
 *   Examples: ezThreeFourths(11) = 8
 *             ezThreeFourths(-9) = -6
 *             ezThreeFourths(1073741824) = -268435456 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int ezThreeFourths(int x) {
  int threex = x + x + x; /* adding first stops overflow */
  int y = threex >> 31;
  return (threex + (y & (4 + ~0))) >> 2; /* bias if neg is 3 */
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
	int shifted = x>>(n+(~1+1));
	/* fitting in n bits is the  same as everything to the left of n being all ones and thus the number is negative & fits or all 0's and thus it's
 positive and  less than 2^n */
	int allones = ~0;
	int allzeros = 0;
	int shiftedallones = !(shifted^allones); /*is it negative */
	int shiftedallzeros = !(shifted^allzeros); /* is it pos and less */
	return shiftedallones|shiftedallzeros;
}
/* 
 * greatestBitPos - return a mask that marks the position of the
 *               most significant 1 bit. If x == 0, return 0
 *   Example: greatestBitPos(96) = 0x40
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 70
 *   Rating: 4 
 */
int greatestBitPos(int x) {
  /* pretty much same as ilog2, but if 0 return all 0s & if neg return 100...0*/
	int isxzero = (!x)<<31;
	int isxneg = (x&(1<<31))>>31;
	int isxpos = ((!(isxzero|isxneg))<<31)>>31;
	int zero_result = isxzero&0; /*return 0 if 0 */
	int neg_result = isxneg&(1<<31); /*return 100..000 if negative */
	int pos_result;
	int ilog;	
	int onehalfones = (~0)<<16;
	int twohalfones = ((~0)<<8)^onehalfones;
	int threehalfones = (((~0)<<4)^twohalfones)^onehalfones;
	int fourhalfones = ((((~0)<<2)^threehalfones)^twohalfones)^onehalfones;
	int fivehalfones = (((((~0)<<1)^fourhalfones)^threehalfones)^twohalfones)^onehalfones;
	int anybitsthishalf = !!(x&onehalfones);
	int logtwosofar = anybitsthishalf << 4;
	x = x>>(anybitsthishalf << 4);
	anybitsthishalf = !!(x&twohalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 3);
	x = x >> (anybitsthishalf<<3);
	anybitsthishalf = !!(x&threehalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 2);
	x = x >> (anybitsthishalf<<2);
	anybitsthishalf = !!(x&fourhalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 1);
	x = x >> (anybitsthishalf<<1);
 	anybitsthishalf = !!(x&fivehalfones);
	logtwosofar = logtwosofar + anybitsthishalf;
 	ilog = logtwosofar;
	pos_result = isxpos&(1<<ilog); /*return 1<<ilog2(x) if pos i.e. just the greatestbitpos(x)th bit on */
  return zero_result|neg_result|pos_result;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  /* essentially a binary search for the highest bit turned on, which we can
then deduce the number from which bit this is - branching into 2 works by if any bits are on in the left half, we shift that to the right & add to the counter
but if none of the bits in the left are on, we just look at the right */
	int onehalfones = (~0)<<16;
	int twohalfones = ((~0)<<8)^onehalfones;
	int threehalfones = (((~0)<<4)^twohalfones)^onehalfones;
	int fourhalfones = ((((~0)<<2)^threehalfones)^twohalfones)^onehalfones;
	int fivehalfones = (((((~0)<<1)^fourhalfones)^threehalfones)^twohalfones)^onehalfones;
	int anybitsthishalf = !!(x&onehalfones);
	int logtwosofar = anybitsthishalf << 4; /*add 16 if any on */
	x = x>>(anybitsthishalf << 4);
	anybitsthishalf = !!(x&twohalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 3); /*add 8 if any on */
	x = x >> (anybitsthishalf<<3);
	anybitsthishalf = !!(x&threehalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 2); /* 4 */
	x = x >> (anybitsthishalf<<2);
	anybitsthishalf = !!(x&fourhalfones);
	logtwosofar = logtwosofar + (anybitsthishalf << 1); /* 2 */
	x = x >> (anybitsthishalf<<1);
 	anybitsthishalf = !!(x&fivehalfones);
	logtwosofar = logtwosofar + anybitsthishalf; /* 1 */
 return logtwosofar;
}
/* 
 * implication - return x -> y in propositional logic - 0 for false, 1
 * for true
 *   Example: implication(1,1) = 1
 *            implication(1,0) = 0
 *   Legal ops: ! ~ ^ |
 *   Max ops: 5
 *   Rating: 2
 */
int implication(int x, int y) {
    return (!x)|y;
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
  /* if positive, then power of 2 is equivalent to only having one bit on, so by subtracting one, makes it from 0..010...0 to 0...001...1, so x&(x-1) will be 0.
     We also check for the negative and zero cases. */
  int y = x >> 31;
  int z = (x + ~0) >> 31;
  return (!(x & (x + ~0))) & ~y & ~z;
}
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
	int negx = ~x+1;
	int onlyzerozero = x|(negx); /* x = -x is only true for 0 and only -0|0 will give 0 as neither of them have any bits on */
	int diffsigntonegone = (onlyzerozero>>31); /* 0 -> 0, anything else ->-1 as -x will have the 1st bit on */
	return  diffsigntonegone+1;
}
/*
 * multFiveEighths - multiplies by 5/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*5/8),
 *   including overflow behavior.
 *   Examples: multFiveEighths(77) = 48
 *             multFiveEighths(-22) = -13
 *             multFiveEighths(1073741824) = 13421728 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int multFiveEighths(int x) {
  int fivex = x + x + x + x + x; /*adding first avoids overflow */
  int y = fivex >> 31; /*is it negative */
  return (fivex + (y & (8 + ~0))) >> 3; /* 7 is bias */
}
/* 
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
  /* shift x right 32-n, turning top bits to all 1's or 0's */
  int y = (x >> (32 + ~n + 1)) & (~((~0) << n)); /* and with 00...00 followed by n ones to get your mask */
  return (x << n) ^ y;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
  /* same as AddOK but do conditional with all 1's or all 0's & possible return
     value to make a neg overflow into int_min and pos overflow into int_max */
	int sum = x+y;
	int x_pos = ~((x>>31)&1);
	int y_pos = ~((y>>31)&1);
	int sum_pos = ~((sum>>31)&1);
	int neg_over = (((~(x_pos|y_pos))&sum_pos)<<31)>>31; /* all 1's or 0's*/
	int pos_over = ((x_pos&y_pos&(~sum_pos))<<31)>>31; /*same*/
	int over = (neg_over)|(pos_over);
	int neg_result = neg_over&(1<<31);
	int pos_result = pos_over&(~(1<<31));
	int result = (~over)&sum;
	return result|neg_result|pos_result;
}
/* 
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x) {
  int y = x >> 31;
  return (~y & x) | ((~(x ^ (1 << 31)) + 1) & y); 
}
/* 
 * tc2sm - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: tc2sm(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int tc2sm(int x) {
  int y = x >> 31;
  return (~y & x) | ((~(x ^ (1 << 31)) + 1) & y);
}
/*
 * trueThreeFourths - multiplies by 3/4 rounding toward 0,
 *   avoiding errors due to overflow
 *   Examples: trueThreeFourths(11) = 8
 *             trueThreeFourths(-9) = -6
 *             trueThreeFourths(1073741824) = 805306368 (no overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int trueThreeFourths(int x)
{
  int y = x >> 31;
  int bit2 = !!(3 & x); /*check if anything in bits less than 4*/
  int quarter = x >> 2;
  quarter = quarter + (bit2 & (!y)); /* if so, and positive, add back 1 */
  return x + ~quarter + 1; /* x - x/4 conveniently = 3x/4 */
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x) {
  /*my easier method works for 5/8ths under operation limit but not for 3/4ths -
the first (divisor) bits are the only place to worry about overflow errors when
dealing with them and the last bits at the same time, so just do these two
multiplications separately and add back together */
	int firstbit = (x >> 31) & 1;
	int bias = (firstbit << 3) + (~firstbit + 1); /*0 if pos, 9 if neg */
	int firstbyte = x&((1<<31)>>7); /*all 1's in first byte selector */
	firstbyte = (firstbyte + bias) >>3;
	firstbyte = (firstbyte << 2) + firstbyte;
	int otherbytes = x&(~((1<<31)>>7)); /*all 1's in rest of num selector */
	otherbytes = (otherbytes << 2) + otherbytes;
	otherbytes = (otherbytes + bias) >> 3;
	return  firstbyte + otherbytes;
}

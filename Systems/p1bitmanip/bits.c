/* 
 * CS:APP Data Lab 
 * 
 * Nolan Winkler - nolanwinkler, 428237
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

int absVal(int x) {
  int y = x >> 31;
  return (y+x)^y;
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
  return 2;
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
    return 2;
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
	int neg = (x>>31)&1;
	int notdivis = x&(~(1<<(n-1))); 
	return (x>>n)+((neg&notdivis));
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
  return ((x + x + x)>>2);
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
/* int fitsBits(int x, int n) {
	int negtwon = ((~(1<<n)+1);
	int twonminone = ~(~0<<n);
	int isxnegtwon = !(x^negtwon);
	int y=x>>31;
  	int absx= (x+y)^y;
	int isabsxlstwonminone = (absx+(twonminone+(~x+1)))&(1<<(n+(~1+1)));
	return isxnegtwon|isabsxlesstwonminone;
} */

/* int fitsBits(int x, int n) {
	int twoton = 1<<n;
	int abshelp = x>>31;
	int absx = (x+abshelp)^abshelp;
	int twonminabs = (twoton + (~absx+1));
	int abslesseqtwon =(twonminabs>>31)&1;
	int xnottwoton = !!(x^twoton);
  return (abslesseqtwon&xnottwoton);
} */

int fitsBits(int x, int n) {
	int shifted = x>>(n+(~1+1));
	int allones = ~0;
	int allzeros = 0;
	int shiftedallones = !(shifted^allones);
	int shiftedallzeros = !(shifted^allzeros);
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
	int isxzero = (!x)<<31;
	int isxneg = (x&(1<<31))>>31;
	int isxpos = ((!(isxzero|isxneg))<<31)>>31;
	int zero_result = isxzero&0;
	int neg_result = isxneg&(1<<31);
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
	pos_result = isxpos&(1<<ilog);
  return zero_result|neg_result|pos_result;
}


int ilog2(int x) {
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
	return !(!(x&(x+(~0))));
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
	int onlyzerozero = x|(negx);
	int diffsigntonegone = (onlyzerozero>>31);
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
  return 2;
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
  return 2;
}
int satAdd(int x, int y) {
	int sum = x+y;
	int x_pos = ~((x>>31)&1);
	int y_pos = ~((y>>31)&1);
	int sum_pos = ~((sum>>31)&1);
	int neg_over = (((~(x_pos|y_pos))&sum_pos)<<31)>>31;
	int pos_over = ((x_pos&y_pos&(~sum_pos))<<31)>>31;
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
  return 2;
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
  return 2;
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
int trueThreeFourths(int x) {
  return (((x<<1) + x)>>2);
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
int trueFiveEighths(int x)
{
    return 2;
}

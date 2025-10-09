/*
 * CS:APP Data Lab Solutions
 */

// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
    /* Use De Morgan's law: x^y = (x&~y)|(~x&y) = ~(~(x&~y) & ~(~x&y)) */
    return ~(~(x & ~y) & ~(~x & y));
}

/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
    /* Minimum int is 0x80000000, which is 1 shifted left 31 bits */
    return 1 << 31;
}

// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
    /* Tmax is 0x7FFFFFFF. Tmax+1 = 0x80000000 (Tmin)
     * Tmax+1+Tmax+1 = 0. Also need to exclude -1 (0xFFFFFFFF)
     * because -1+1 = 0, and 0+0 = 0 */
    int i = x + 1;
    return !(i + i) & !!(i);
}

/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
    /* Create mask 0xAAAAAAAA by building 0xAA and shifting/ORing
     * Check if all odd bits are set by masking and comparing */
    int mask = 0xAA;
    mask = mask | (mask << 8);
    mask = mask | (mask << 16);
    return !((x & mask) ^ mask);
}

/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
    /* Two's complement negation: flip bits and add 1 */
    return ~x + 1;
}

// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
    /* Check if upper bits are 0x3 and lower 4 bits are 0-9
     * x - 0x30 should be >= 0 and x - 0x3A should be < 0 */
    int lower_bound = !((x + (~0x30 + 1)) >> 31);
    int upper_bound = ((x + (~0x39)) >> 31);
    return lower_bound & upper_bound;
}

/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
    /* Convert x to 0 or 1, then create mask of all 0s or all 1s
     * If x != 0, mask = 0xFFFFFFFF, return y
     * If x == 0, mask = 0x00000000, return z */
    int mask = (!!x << 31) >> 31;
    return (mask & y) | (~mask & z);
}

/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
    /* Check if x - y <= 0, which is x + (-y) <= 0
     * Need to handle overflow: if signs differ, use sign of x
     * If signs same, use sign of difference */
    int sign_x = (x >> 31) & 1;
    int sign_y = (y >> 31) & 1;
    int sign_diff = ((x + (~y + 1)) >> 31) & 1;
    int signs_differ = sign_x ^ sign_y;
    return (signs_differ & sign_x) | (!signs_differ & (sign_diff | !(x ^ y)));
}

// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x)
{
    /* For 0: both x and -x have sign bit 0
     * For any other number: either x or -x has sign bit 1
     * Combine sign bits of x and -x */
    return ((x | (~x + 1)) >> 31) + 1;
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
    /* For positive: find highest set bit + 1 for sign
     * For negative: flip bits and find highest set bit + 1
     * Use binary search to find highest bit position */
    int sign = x >> 31;
    int b16, b8, b4, b2, b1, b0;

    x = (sign & ~x) | (~sign & x);

    b16 = !!(x >> 16) << 4;
    x = x >> b16;
    b8 = !!(x >> 8) << 3;
    x = x >> b8;
    b4 = !!(x >> 4) << 2;
    x = x >> b4;
    b2 = !!(x >> 2) << 1;
    x = x >> b2;
    b1 = !!(x >> 1);
    x = x >> b1;
    b0 = x;

    return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}

// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
    unsigned exp = (uf >> 23) & 0xFF;
    unsigned sign = uf & 0x80000000;
    unsigned frac = uf & 0x7FFFFF;

    if (exp == 0xFF)
        return uf; /* NaN or infinity */
    if (exp == 0)
    { /* Denormalized */
        frac <<= 1;
        return sign | frac;
    }
    exp += 1;
    if (exp == 0xFF)
        return sign | 0x7F800000; /* Overflow to infinity */
    return sign | (exp << 23) | frac;
}

/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
    unsigned sign = uf >> 31;
    unsigned exp = (uf >> 23) & 0xFF;
    unsigned frac = uf & 0x7FFFFF;
    int E = exp - 127;
    int result;

    if (exp == 0xFF || E > 30)
        return 0x80000000u;
    if (E < 0)
        return 0;

    frac = frac | 0x800000;
    if (E < 23)
        result = frac >> (23 - E);
    else
        result = frac << (E - 23);

    if (sign)
        return -result;
    return result;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
    if (x < -149)
        return 0; /* Too small, underflow */
    if (x < -126)
        return 1 << (149 + x); /* Denormalized */
    if (x > 127)
        return 0x7F800000;  /* Too large, overflow to infinity */
    return (x + 127) << 23; /* Normalized */
}

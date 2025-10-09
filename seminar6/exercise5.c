long test(long x, long y, long z)
{
    long val = x + y + z;
    if (x < -3)
    {
        if (y >= z)
            val = y * z;
        else
            val = x * y;
    }
    else if (x > 2)
        val = x * z;
    return val;
}

long loop_while(long a, long b)
{
    long result = 1;
    while (a < b)
    {
        result = result * (a + b);
        a = a + 1;
    }
    return result;
}
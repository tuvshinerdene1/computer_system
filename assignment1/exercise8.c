#include <stdio.h>

int count_trailing_zeros(unsigned int n)
{
    if (n == 0)
        return 32;

    int count = 0;
    while ((n & 1) == 0)
    {
        count++;
        n >>= 1;
    }
    return count;
}

int main()
{
    unsigned int x;
    scanf("%u", &x);
    printf("%d\n", count_trailing_zeros(x));
    return 0;
}

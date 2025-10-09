#include <stdio.h>
#include <limits.h>

int getLastSetBitPosition(int n)
{
    if (n == 0)
    {
        return 0;
    }

    const int total_bits = sizeof(int) * CHAR_BIT;

    for (int position = total_bits; position >= 1; position--)
    {
        unsigned int mask = 1U << (position - 1);

        if (n & mask)
        {
            return position;
        }
    }

    return 0;
}

int main()
{
    int num;
    printf("num = ");
    scanf("%d", &num);
    int pos = getLastSetBitPosition(num);
    printf(pos);
    return 0;
}
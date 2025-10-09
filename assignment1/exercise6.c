#include <stdio.h>

int getFirstSetBitPosition(int n)
{
    if (n == 0)
    {
        return 0;
    }

    int position = 1;
    while (n > 0)
    {
        if (n & 1)
        {
            return position;
        }

        n >>= 1;

        position++;
    }

    return 0;
}

int main()
{
    int num;
    printf("num = ");
    scanf("%d", &num);
    int pos = getFirstSetBitPosition(num);
    printf(pos);
    return 0;
}
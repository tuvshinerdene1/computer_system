#include <stdio.h>

unsigned int rotate(unsigned int n, int k)
{
    int bit_size = 32;
    k %= bit_size;
    return (n << k) | (n >> (bit_size - k));
}

int main()
{
    unsigned int n;
    int k;
    scanf("%u %d", &n, &k);

    unsigned int rotated = rotate(n, k);

    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (rotated & (1u << i)) ? 1 : 0);
        if (i % 8 == 0 && i != 0)
            printf(" ");
    }
    printf("\n");
    return 0;
}

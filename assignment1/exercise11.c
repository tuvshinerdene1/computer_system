#include <stdio.h>

int main()
{
    unsigned int n;
    scanf("%u", &n);

    for (int i = 31; i >= 0; i--)
    {
        unsigned int mask = 1u << i;
        printf("%d", (n & mask) ? 1 : 0);

        if (i % 8 == 0 && i != 0)
            printf(" ");
    }

    printf("\n");
    return 0;
}

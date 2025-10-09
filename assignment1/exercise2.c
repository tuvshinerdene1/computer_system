#include <stdio.h>
#include <limits.h>
int main()
{
    int num;
    printf("Enter Number: ");
    scanf("%d", &num);

    int num_bits = sizeof(int) * CHAR_BIT;
    int msb = (num >> (num_bits - 1)) & 1;
    printf("Least significant bit : %d", msb);
    return 0;
}
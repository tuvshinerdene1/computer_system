#include <stdio.h>

int getNthBit(int num, int n)
{
    return (num >> n) & 1;
}

int main()
{
    int number, n;
    printf("number = ");
    scanf("%d", &number);
    printf("n = ");
    scanf("%d", &n);
    int bit = getNthBit(number, n);

    printf("The integer is: %d\n", number);
    printf("The bit position (n) is: %d\n", n);
    printf("The %d-th bit of %d is: %d\n", n, number, bit);
    return 0;
}
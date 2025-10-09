#include <stdio.h>

int reverseNthBit(int num, int n)
{
    return num ^ (1 << n);
}

int main()
{
    int number, n;
    printf("number = ");
    scanf("%d", &number);
    printf("n = ");
    scanf("%d", &n);
    int result = reverseNthBit(number, n);

    printf("The integer is: %d\n", number);
    printf("The bit position (n) is: %d\n", n);
    printf("After seting %dth bit to reverse : %d\n", n, result);
    return 0;
}
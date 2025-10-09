#include <stdio.h>

int setNthBit(int num, int n)
{
    return num | (1 << n);
}

int main()
{
    int number, n;
    printf("number = ");
    scanf("%d", &number);
    printf("n = ");
    scanf("%d", &n);
    int result = setNthBit(number, n);

    printf("The integer is: %d\n", number);
    printf("The bit position (n) is: %d\n", n);
    printf("After seting %dth bit to 1 : %d\n", n, result);
    return 0;
}
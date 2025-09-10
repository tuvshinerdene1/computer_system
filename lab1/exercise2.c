#include <stdio.h>

int main()
{
    int dec;
    printf("Enter integer number: ");
    scanf("%d", &dec);
    int temp = dec;
    int digitNum = 1;
    int result = 0;
    while (temp > 0)
    {
        result += (temp % 2) * digitNum;
        digitNum *= 10;
        temp /= 2;
    }
    printf("%d\n", result);

    return 0;
}
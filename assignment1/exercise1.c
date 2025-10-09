#include <stdio.h>
int main()
{
    int num;
    printf("Enter Number: ");
    scanf("%d", &num);

    int lsb = num & 1;
    printf("Least significant bit : %d", lsb);
    return 0;
}
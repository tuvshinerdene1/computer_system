#include <stdio.h>
#include <limits.h>
#include <math.h>
int tadd_ok(int x, int y)
{
    return y <= INT_MAX - x;
}
int main()
{
    int x, y;
    x = pow(2, 31) - 1;
    y = 1;
    printf("%d\n", tadd_ok(x, y));
    x = 100;
    y = 200;
    printf("%d\n", tadd_ok(x, y));
    return 0;
}
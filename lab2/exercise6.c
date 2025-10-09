#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

int tsub_ok(int x, int y)
{
    return ((x - y) >= INT_MIN) & ((y - x) >= INT_MIN);
}

int main()
{
    int x, y;
    x = pow(2, 31) - 1;
    y = -pow(2, 31);
    printf("%d\n", tsub_ok(x, y));
    x = 100;
    y = 200;
    printf("%d\n", tsub_ok(x, y));
    return 0;
}
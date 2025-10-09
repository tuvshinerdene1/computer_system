#include <stdio.h>
#include <limits.h>
#include <math.h>
int uadd_ok(unsigned x, unsigned y)
{
    return y <= UINT_MAX - x;
}
int main()
{
    unsigned x, y;
    x = pow(2, 32) - 1;
    y = pow(2, 32) - 1;
    printf("%d\n", uadd_ok(x, y));
    x = 100;
    y = 200;
    printf("%d\n", uadd_ok(x, y));
    return 0;
}
#include <stdio.h>
int fact(int n)
{
    if (n == 0)
    {
        return 1;
    }
    int x = fact(n - 1);
    return x * n;
}

void decode1(long *xp, long *yp, long *zp)
{
    long val_x = *xp;
    long val_y = *yp;
    long val_z = *zp;

    *yp = val_x;
    *zp = val_y;
    *xp = val_z;
}

int main()
{
    int y = fact(5);
    printf("%d\n", y);
    return 0;
}
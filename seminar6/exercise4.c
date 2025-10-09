#include <stdio.h>
int main()
{
    return 0;
}
void cond(long a, long *p)
{
    if (p && a > *p)
    {
        *p = a;
    }
}

void cond(long a, long *p)
{
    if (p == 0)
        goto done;
    if (a <= *p)
        goto done;
    *p = a;
done:;
}
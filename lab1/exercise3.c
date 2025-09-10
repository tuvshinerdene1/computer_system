#include <stdio.h>
typedef struct
{
    int a[2];
    double d;
} struct_t;
double fun(int i)
{
    if (i > 1)
    {
        printf("Array size exceeded\n");
        return -1;
    }
    volatile struct_t s;
    s.d = 3.14;
    s.a[i] = 1073741824;
    return s.d;
}
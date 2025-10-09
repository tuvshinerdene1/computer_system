#include <stdio.h>

int main()
{
    unsigned int n;
    scanf("%u", &n);

    unsigned int inverted = ~n;

    printf("%u", inverted);
    return 0;
}

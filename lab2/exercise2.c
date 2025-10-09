#include <stdio.h>
int fun1(unsigned word)
{
    return (int)((word << 24) >> 24);
}
int fun2(unsigned word)
{
    return ((int)(word) << 24) >> 24;
}
int main()
{
    printf("0x00000076: %d %d %d \n", (int)(0x00000076), fun1(0x00000076), fun2(0x00000076));
    printf("0x87654321: %d %d %d \n", (int)(0x87654321), fun1(0x87654321), fun2(0x87654321));
    printf("0x000000C9: %d %d %d \n", (int)(0x000000C9), fun1(0x000000C9), fun2(0x000000C9));
    printf("0xEDCBA987: %d %d %d \n", (int)(0xEDCBA987), fun1(0xEDCBA987), fun2(0xEDCBA987));
    return 0;
}
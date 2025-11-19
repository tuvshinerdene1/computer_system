#include <stdio.h>
#include <unistd.h>
unsigned int wakeup(unsigned int secs)
{
    unsigned int rc = sleep(secs);
    printf("slept for %d secs.\n", secs - rc);
    return rc;
}
int main()
{
    wakeup(3);
    return 0;
}

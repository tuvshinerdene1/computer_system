#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t interrupted = 0;
void sigint_handler(int sig)
{
    interrupted = 1;
}

unsigned int snooze(unsigned int secs)
{
    signal(SIGINT, sigint_handler);
    unsigned int rc = sleep(secs);
    if (interrupted)
    {
        unsigned int slept = secs - rc;
        printf("Slept for %u of %u seconds (interrupted!).\n", slept, secs);
    }
    else
    {
        printf("Slept for %u of %u seconds.\n", secs, secs);
    }
    return rc;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
        return 1;
    }
    snooze(atoi(argv[1]));
    return 0;
}
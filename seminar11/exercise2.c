#include "csapp.h"
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig)
{
    sio_puts("\ninterrupted by ctrl+c\n");
}

int main()
{
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        sio_puts("Error installing signal handler\n");
        return 1;
    }

    while (1)
    {
        pause(); // Wait for signals
    }

    return 0;
}

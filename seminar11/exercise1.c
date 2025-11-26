#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

volatile long counter = 2;

void handler1(int sig)
{
    sigset_t mask, prev_mask;

    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);

    counter--;
    printf("%ld", counter);
    fflush(stdout);

    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    _exit(0);
}

int main()
{
    pid_t pid;
    sigset_t mask, prev_mask;

    printf("%ld", counter);
    fflush(stdout);

    signal(SIGUSR1, handler1);

    if ((pid = fork()) == 0)
    {
        while (1)
        {
        }
    }

    kill(pid, SIGUSR1);
    waitpid(pid, NULL, 0);

    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    printf("%ld", ++counter);
    fflush(stdout);
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    return 0;
}

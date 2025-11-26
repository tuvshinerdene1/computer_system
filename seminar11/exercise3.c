#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

volatile sig_atomic_t children_left = 0;

static void safe_write(const char *s)
{
    ssize_t unused = write(STDOUT_FILENO, s, (size_t)strlen(s));
    (void)unused;
}

static void sigchld_handler(int signo)
{
    (void)signo;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        children_left--;

        char buf[128];
        int len = snprintf(buf, sizeof buf, "reaped child %d, children_left=%d\n",
                           (int)pid, (int)children_left);
        if (len > 0)
        {
            write(STDOUT_FILENO, buf, (size_t)len);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <N>\n", argv[0]);
        return 1;
    }

    long N = strtol(argv[1], NULL, 10);
    if (N <= 0)
    {
        fprintf(stderr, "N must be positive\n");
        return 1;
    }

    sigset_t block_mask, prev_mask, empty_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &block_mask, &prev_mask) < 0)
    {
        perror("sigprocmask");
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0)
    {
        perror("sigaction");
        return 1;
    }

    children_left = (sig_atomic_t)N;

    for (long i = 0; i < N; ++i)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            children_left--;
            continue;
        }
        else if (pid == 0)
        {

            sleep(1);
            _exit(0);
        }
    }

    sigemptyset(&empty_mask);

    while (children_left > 0)
    {
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR)
        {
            perror("sigsuspend");
            break;
        }
    }

    printf("parent: all children reaped (children_left = %d). Exiting.\n", (int)children_left);

    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) < 0)
    {
        perror("sigprocmask restore");
    }

    return 0;
}
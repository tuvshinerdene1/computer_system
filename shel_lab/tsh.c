/*
 * tsh - A tiny shell program with job control
 *
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE 1024   /* max line size */
#define MAXARGS 128    /* max args on a command line */
#define MAXJOBS 16     /* max jobs at any point in time */
#define MAXJID 1 << 16 /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* Global variables */
extern char **environ;   /* defined in libc */
char prompt[] = "tsh> "; /* command line prompt (DO NOT CHANGE) */
int verbose = 0;         /* if true, print additional output */
int nextjid = 1;         /* next job ID to allocate */
char sbuf[MAXLINE];      /* for composing sprintf messages */

struct job_t
{                          /* The job struct */
    pid_t pid;             /* job PID */
    int jid;               /* job ID [1, 2, ...] */
    int state;             /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE]; /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */

/* Function prototypes */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);
void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Helper routines */
int parseline(const char *cmdline, char **argv);
void sigquit_handler(int sig);
void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs);
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid);
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *jobs);
void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1;

    dup2(1, 2);

    while ((c = getopt(argc, argv, "hvp")) != EOF)
    {
        switch (c)
        {
        case 'h':
            usage();
            break;
        case 'v':
            verbose = 1;
            break;
        case 'p':
            emit_prompt = 0;
            break;
        default:
            usage();
        }
    }

    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGQUIT, sigquit_handler);

    initjobs(jobs);

    while (1)
    {
        if (emit_prompt)
        {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin))
        {
            fflush(stdout);
            exit(0);
        }

        eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    }

    exit(0);
}

/*
 * eval - Evaluate the command line that the user has just typed in
 */
void eval(char *cmdline)
{
    char *argv[MAXARGS];                   // Аргументуудыг хадгалах массив
    char buf[MAXLINE];                     // Коммандын мөрийг хадгалах буфер
    int bg;                                // Арын горимд (background) ажиллах эсэхийг заах хувьсагч
    pid_t pid;                             // Процессын ID
    sigset_t mask_all, mask_one, prev_one; // Сигналын багцуудыг тодорхойлох хувьсагчид

    strcpy(buf, cmdline);      // Оруулсан коммандыг буфер руу хуулах
    bg = parseline(buf, argv); // Коммандыг задалж аргументуудад хуваах ба BG эсэхийг шалгах

    /* Ignore empty lines */
    if (argv[0] == NULL) // Хэрэв комманд хоосон байвал
        return;          // Функцээс буцах

    /* If it is a built-in command, run it immediately */
    if (!builtin_cmd(argv)) // Хэрэв built-in (дотоод) комманд биш бол (ж.нь: ls, sleep)
    {
        /*
         * Signal Blocking:
         * We must block SIGCHLD before forking to prevent a race condition
         */
        sigfillset(&mask_all);         // Бүх сигналыг агуулсан багцыг бэлтгэх
        sigemptyset(&mask_one);        // Хоосон сигналын багц үүсгэх
        sigaddset(&mask_one, SIGCHLD); // Багцад SIGCHLD сигналыг нэмэх

        /* Block SIGCHLD */
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one); // SIGCHLD сигналыг түр хаах (block)

        if ((pid = fork()) == 0) // Хүү процесс үүсгэх (fork хийх)
        {                        /* Child process */
            /* Child unblocks signals (inherits blocked status from parent) */
            sigprocmask(SIG_SETMASK, &prev_one, NULL); // Хүү процесс дотор сигналыг буцааж нээх (unblock)

            /* Put child in a new process group */
            setpgid(0, 0); // Хүү процессыг шинэ бүлэгт оруулах (бүлгийн ID-г өөрийн PID-тай ижил болгох)

            if (execve(argv[0], argv, environ) < 0) // Шинэ программыг ачаалж ажиллуулах
            {
                printf("%s: Command not found.\n", argv[0]); // Программ олдохгүй бол алдаа хэвлэх
                exit(0);                                     // Хүү процессыг дуусгах
            }
        }

        /* Parent process */
        /* Block all signals while adding to job list (global data structure) */
        sigprocmask(SIG_BLOCK, &mask_all, NULL);                               // Жагсаалттай ажиллаж байх үед бүх сигналыг хаах
        addjob(jobs, pid, bg ? BG : FG, cmdline);                              // Ажлын жагсаалтад (jobs list) шинэ процессыг нэмэх
        sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Restore original mask */ // Сигналуудыг буцааж хэвийн төлөвт оруулах

        /* Foreground Job */
        if (!bg) // Хэрэв нүүрэн талын (foreground) ажил бол
        {
            waitfg(pid); // Тухайн процесс дуустал эсвэл зогстол хүлээх
        }
        /* Background Job */
        else // Хэрэв арын (background) ажил бол
        {
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline); // Ажлын мэдээллийг дэлгэцэнд хэвлэх
        }
    }
    return; // Функцээс гарах
}

/*
 * parseline - Parse the command line and build the argv array.
 */
int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE];
    char *buf = array;
    char *delim;
    int argc;
    int bg;

    strcpy(buf, cmdline);
    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' '))
        buf++;

    argc = 0;
    if (*buf == '\'')
    {
        buf++;
        delim = strchr(buf, '\'');
    }
    else
    {
        delim = strchr(buf, ' ');
    }

    while (delim)
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))
            buf++;

        if (*buf == '\'')
        {
            buf++;
            delim = strchr(buf, '\'');
        }
        else
        {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL;

    if (argc == 0)
        return 1;

    if ((bg = (*argv[argc - 1] == '&')) != 0)
    {
        argv[--argc] = NULL;
    }
    return bg;
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute it immediately.
 */
int builtin_cmd(char **argv)
{
    if (strcmp(argv[0], "quit") == 0) // Хэрэв комманд "quit" байвал
    {
        exit(0); // Программыг шууд дуусгах
    }
    if (strcmp(argv[0], "jobs") == 0) // Хэрэв комманд "jobs" байвал
    {
        listjobs(jobs); // Идэвхтэй ажлуудын жагсаалтыг хэвлэх
        return 1;       // Дотоод комманд байсан тул 1-ийг буцаах
    }
    if (strcmp(argv[0], "bg") == 0 || strcmp(argv[0], "fg") == 0) // Хэрэв комманд "bg" эсвэл "fg" байвал
    {
        do_bgfg(argv); // BG/FG функцийг дуудах
        return 1;      // Дотоод комманд байсан тул 1-ийг буцаах
    }
    /* Not a built-in command */
    return 0; // Дотоод комманд биш байсан тул 0-ийг буцаах
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
    struct job_t *job = NULL; // Ажлын бүтцийг хадгалах заагч
    char *id = argv[1];       // PID эсвэл JID аргумент
    int jid;                  // Ажлын ID (Job ID)
    pid_t pid;                // Процессын ID (Process ID)

    /* Ignore command if no argument provided */
    if (id == NULL) // Хэрэв ID аргумент өгөгдөөгүй бол
    {
        printf("%s command requires PID or %%jobid argument\n", argv[0]); // Алдааны мэдээлэл хэвлэх
        return;                                                           // Буцах
    }

    /* Parse JID or PID */
    if (id[0] == '%') // Хэрэв аргумент '%' тэмдэгтээр эхэлсэн бол (JID)
    {
        jid = atoi(&id[1]);         // '%' тэмдэгтийн дараах тоог JID болгон хөрвүүлэх
        job = getjobjid(jobs, jid); // JID-аар ажлыг хайх
        if (job == NULL)            // Ажил олдохгүй бол
        {
            printf("%s: No such job\n", id); // Алдаа хэвлэх
            return;                          // Буцах
        }
    }
    else if (isdigit(id[0])) // Хэрэв аргумент тоогоор эхэлсэн бол (PID)
    {
        pid = atoi(id);             // Тоог PID болгон хөрвүүлэх
        job = getjobpid(jobs, pid); // PID-аар ажлыг хайх
        if (job == NULL)            // Ажил олдохгүй бол
        {
            printf("(%d): No such process\n", pid); // Алдаа хэвлэх
            return;                                 // Буцах
        }
    }
    else // Хэрэв буруу форматтай аргумент байвал
    {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]); // Зөв хэрэглээг сануулах
        return;                                                     // Буцах
    }

    /* Handle bg command */
    if (strcmp(argv[0], "bg") == 0) // Хэрэв комманд "bg" байвал
    {
        job->state = BG;                                          // Ажлын төлөвийг Background (BG) болгож өөрчлөх
        kill(-(job->pid), SIGCONT);                               // Процессын бүлэгт SIGCONT сигнал илгээж ажиллагааг үргэлжлүүлэх
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline); // Мэдээллийг хэвлэх
    }
    /* Handle fg command */
    else // Хэрэв комманд "fg" байвал
    {
        job->state = FG;            // Ажлын төлөвийг Foreground (FG) болгож өөрчлөх
        kill(-(job->pid), SIGCONT); // Процессын бүлэгт SIGCONT сигнал илгээж ажиллагааг үргэлжлүүлэх
        waitfg(job->pid);           // Тухайн процесс дуустал эсвэл зогстол хүлээх
    }

    return; // Функцээс гарах
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    /*
     * Spin wait: check if the process is still the foreground process.
     */
    while (pid == fgpid(jobs)) // Тухайн PID нь FG (нүүрэн талын) процесс байсаар байвал давталт хийнэ
    {
        sleep(0); // CPU-г ачааллахгүйн тулд түр хүлээх (context switch хийх боломж олгох)
    }
    return; // Процесс FG байхаа больсон үед функцээс гарна
}

/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates or stops.
 */
void sigchld_handler(int sig)
{
    int olderrno = errno; // Глобал errno хувьсагчийг хадгалж авах (handler дотор өөрчлөгдөхөөс сэргийлэх)
    pid_t pid;            // Процессын ID
    int status;           // Процессын төлөв
    struct job_t *job;    // Ажлын бүтэц

    /*
     * WNOHANG: return immediately if no child has exited
     * WUNTRACED: also return if a child has stopped (but not traced)
     */
    // Дууссан эсвэл зогссон бүх хүү процессуудыг шалгах (waitpid-г блоклохгүй горимоор дуудах)
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {

        /* Case 1: Child exited normally */
        if (WIFEXITED(status)) // Хэрэв хүү процесс хэвийн дууссан бол
        {
            deletejob(jobs, pid); // Ажлын жагсаалтаас хасах
        }

        /* Case 2: Child terminated by a signal (e.g., Ctrl-C / SIGINT) */
        else if (WIFSIGNALED(status)) // Хэрэв хүү процесс гадны сигналаар (ж.нь: SIGINT) дуусгавар болсон бол
        {
            job = getjobpid(jobs, pid); // PID-аар ажлыг олох
            if (job != NULL)
            {
                // Ямар сигналаар дууссан тухай мэдээллийг хэвлэх
                printf("Job [%d] (%d) terminated by signal %d\n",
                       job->jid, job->pid, WTERMSIG(status));
            }
            deletejob(jobs, pid); // Ажлын жагсаалтаас хасах
        }

        /* Case 3: Child stopped (e.g., Ctrl-Z / SIGTSTP) */
        else if (WIFSTOPPED(status)) // Хэрэв хүү процесс түр зогссон (stopped) бол
        {
            job = getjobpid(jobs, pid); // PID-аар ажлыг олох
            if (job != NULL)
            {
                job->state = ST; // Ажлын төлөвийг Stopped (ST) болгож өөрчлөх
                // Ямар сигналаар зогссон тухай мэдээллийг хэвлэх
                printf("Job [%d] (%d) stopped by signal %d\n",
                       job->jid, job->pid, WSTOPSIG(status));
            }
        }
    }

    errno = olderrno; // errno-г буцааж сэргээх
    return;           // Функцээс гарах
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.
 */
void sigint_handler(int sig)
{
    int olderrno = errno;    // errno-г хадгалах
    pid_t pid = fgpid(jobs); // Одоо ажиллаж буй нүүрэн талын (foreground) процессын PID-г авах

    if (pid != 0) // Хэрэв нүүрэн талд процесс байвал
    {
        /* Send SIGINT to the entire process group (-pid) */
        kill(-pid, SIGINT); // Тухайн процессын бүлэгт (бүх хүү процессуудад) SIGINT илгээх
    }

    errno = olderrno; // errno-г сэргээх
    return;           // Функцээс гарах
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard.
 */
void sigtstp_handler(int sig)
{
    int olderrno = errno;    // errno-г хадгалах
    pid_t pid = fgpid(jobs); // Одоо ажиллаж буй нүүрэн талын (foreground) процессын PID-г авах

    if (pid != 0) // Хэрэв нүүрэн талд процесс байвал
    {
        /* Send SIGTSTP to the entire process group (-pid) */
        kill(-pid, SIGTSTP); // Тухайн процессын бүлэгт (бүх хүү процессуудад) SIGTSTP илгээх
    }

    errno = olderrno; // errno-г сэргээх
    return;           // Функцээс гарах
}

/*********************
 * End signal handlers
 *********************/

/* Helper routines */
void clearjob(struct job_t *job)
{
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

void initjobs(struct job_t *jobs)
{
    int i;
    for (i = 0; i < MAXJOBS; i++)
        clearjob(&jobs[i]);
}

int maxjid(struct job_t *jobs)
{
    int i, max = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid > max)
            max = jobs[i].jid;
    return max;
}

int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
    int i;
    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == 0)
        {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(jobs[i].cmdline, cmdline);
            if (verbose)
            {
                printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

int deletejob(struct job_t *jobs, pid_t pid)
{
    int i;
    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == pid)
        {
            clearjob(&jobs[i]);
            nextjid = maxjid(jobs) + 1;
            return 1;
        }
    }
    return 0;
}

pid_t fgpid(struct job_t *jobs)
{
    int i;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].state == FG)
            return jobs[i].pid;
    return 0;
}

struct job_t *getjobpid(struct job_t *jobs, pid_t pid)
{
    int i;
    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
            return &jobs[i];
    return NULL;
}

struct job_t *getjobjid(struct job_t *jobs, int jid)
{
    int i;
    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid == jid)
            return &jobs[i];
    return NULL;
}

int pid2jid(pid_t pid)
{
    int i;
    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
        {
            return jobs[i].jid;
        }
    return 0;
}

void listjobs(struct job_t *jobs)
{
    int i;
    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid != 0)
        {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
            switch (jobs[i].state)
            {
            case BG:
                printf("Running ");
                break;
            case FG:
                printf("Foreground ");
                break;
            case ST:
                printf("Stopped ");
                break;
            default:
                printf("listjobs: Internal error: job[%d].state=%d ",
                       i, jobs[i].state);
            }
            printf("%s", jobs[i].cmdline);
        }
    }
}

void usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}

void sigquit_handler(int sig)
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
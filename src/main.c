#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    pid_t pid;
    int rv;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(1);
    }
    switch ( pid = fork() ) {
        case -1:
            perror("fork");
            exit(1);

        case 0:
            close(fd[0]);                     // потомок не читает
            dup2(fd[1], STDOUT_FILENO);       // перенаправление stdout
            rv = execvp(argv[1], argv + 1);
            exit(0);

        default:
            waitpid(pid, &rv, 0);
            close(fd[1]);                     // родитель не записывает
            char buf[1];
            while(read(fd[0], buf, 1) > 0) {
                if (buf[0] == ' ') {
                    buf[0] = '_';   //замена замена пробелов на нижнее подчеркивание
                }
                write(1, buf, 1);             // в stdout
            }
            exit(WEXITSTATUS(rv));
            wait(0);
    }
return 0;
}

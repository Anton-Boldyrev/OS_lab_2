#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char** argv) {
    pid_t pid;
    int rv;
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);

    } else if (pid == 0) {                // потомок
        close(fd[0]);                     // потомок не читает
        dup2(fd[1], STDOUT_FILENO);       // перенаправление stdout
        rv = execvp(argv[1], argv + 1);
        if (rv) {
            perror("exec error");
        }
        _exit(rv);

    } else {                              // родитель
        waitpid(pid, &rv, 0);

        close(fd[1]);                     // родитель не записывает
        char buf[1];

        while(read(fd[0], buf, 1) > 0) {
           if (buf[0] == ' ') {
                buf[0] = '_'; //замена строчных букв на заглавные
           }
            write(1, buf, 1);             // в stdout
        }
        exit(WEXITSTATUS(rv));
    }
}

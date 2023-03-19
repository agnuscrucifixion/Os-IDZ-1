#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *file1;
FILE *file2;

int main(int argc, char **argv)
{
    if (argc == 3) {
        file1 = fopen(*(argv+1), "r");
        file2 = fopen(*(argv+2), "w");
    } else {
        printf("Недостаточно данных в аргументах командной строки");
        return 0;
    }
    int fd[2];
    pid_t pid1, pid2;
    char buf[5000];
    if (pipe(fd) == -1) {
        printf("Ошибка создания канала");
        return 0;
    }
    pid1 = fork();
    if (pid1 == -1) {
        printf("Ошибка создания первого дочернего процесса");
        return 0;
    }
    if (pid1 == 0) {
        close(fd[0]);
        int size = 5000;
        char message[5000];
        fgets(message, size, file1);
        write(fd[1], message, sizeof(message));
        close(fd[1]);
    } else {
        pid2 = fork();
        if (pid2 == -1) {
            printf("Ошибка создания второго дочернего процесса");
            return 0;
        }
        if (pid2 == 0) {
            read(fd[0], buf, sizeof(buf));
            close(fd[0]);
            int sum = 0;
            for (int i = 0; buf[i] != '\0'; i++) {
                if (buf[i] >= '0' && buf[i] <= '9') {
                    sum += buf[i] - '0';
                }
            }
            char res[5000];
            sprintf(res, "%d", sum);
            write(fd[1], res, sizeof(buf));
            close(fd[1]);
        } else {
            pid1 = fork();
            if (pid1 == -1) {
                printf("Ошибка создания первого дочернего процесса");
                return 0;
            }
            if (pid1 == 0) {
                char output[5000];
                read(fd[0], output, sizeof(output));
                fprintf(file2, "%s", output);
                close (fd[0]);
            }
        }
    }

    return 0;
}
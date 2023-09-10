#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    int pipe_fd[2];
    char buffer[100];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return 1;
    }

    if (child_pid == 0) {
        // 자식 프로세스 (파이프 쓰기)
        close(pipe_fd[0]);
        const char *data = "Hello, Parent!";
        write(pipe_fd[1], data, strlen(data));
        close(pipe_fd[1]);
    } else {
        // 부모 프로세스 (파이프 읽기)
        close(pipe_fd[1]);
        read(pipe_fd[0], buffer, sizeof(buffer));
        printf("부모 프로세스: %s\n", buffer);
        close(pipe_fd[0]);
    }

    return 0;
}

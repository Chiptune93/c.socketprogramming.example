#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return 1;
    }

    if (child_pid == 0) {
        // 자식 프로세스
        printf("자식 프로세스\n");
    } else {
        // 부모 프로세스
        printf("부모 프로세스\n");
    }

    return 0;
}

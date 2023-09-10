#include <sys/types.h>
#include <sys/wait.h>
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
        printf("자식 프로세스 시작\n");
        sleep(2); // 2초 대기
        printf("자식 프로세스 종료\n");
    } else {
        // 부모 프로세스
        printf("부모 프로세스\n");
        int status;
        pid_t terminated_pid = wait(&status);
        printf("자식 프로세스 %d 종료 (상태: %d)\n", terminated_pid, WEXITSTATUS(status));
    }

    return 0;
}

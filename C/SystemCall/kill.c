#include <stdio.h>
#include <signal.h>

int main() {
    pid_t target_pid = getpid(); // 현재 프로세스 ID

    printf("프로세스 ID: %d\n", target_pid);

    // 자신에게 시그널 보내기
    int result = kill(target_pid, SIGUSR1);
    if (result == -1) {
        perror("kill");
        return 1;
    }

    return 0;
}

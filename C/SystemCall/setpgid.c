#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = getpid();
    int result = setpgid(pid, pid);

    if (result == -1) {
        perror("setpgid");
        return 1;
    }

    printf("프로세스 그룹 ID: %d\n", getpgid(pid));

    return 0;
}

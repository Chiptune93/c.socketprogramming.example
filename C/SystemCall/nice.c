#include <stdio.h>
#include <unistd.h>

int main() {
    int priority = nice(10); // 프로세스 우선순위를 증가시킴

    if (priority == -1) {
        perror("nice");
        return 1;
    }

    printf("프로세스 우선순위: %d\n", priority);

    return 0;
}

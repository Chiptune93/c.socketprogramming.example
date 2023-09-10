#include <stdio.h>
#include <unistd.h>

int main() {
    printf("현재 프로세스 시작\n");

    execl("/bin/ls", "ls", "-l", NULL);

    printf("이 부분은 실행되지 않습니다.\n");

    return 0;
}

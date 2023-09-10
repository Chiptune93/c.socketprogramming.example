#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void alarm_handler(int signo) {
    printf("알람 시그널 수신\n");
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(5); // 5초 후 알람 시그널 발생

    printf("프로그램 시작\n");

    while (1) {
        // 계속 실행
    }

    return 0;
}

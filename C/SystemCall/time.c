#include <stdio.h>
#include <time.h>

int main() {
    time_t current_time;
    struct tm *time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    printf("현재 시간: %s", asctime(time_info));

    return 0;
}

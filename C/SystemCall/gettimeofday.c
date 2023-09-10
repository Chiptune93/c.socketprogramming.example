#include <stdio.h>
#include <sys/time.h>

int main() {
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    // 시간 측정을 하려는 작업

    gettimeofday(&end_time, NULL);

    long elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
    printf("작업 수행 시간: %ld 마이크로초\n", elapsed);

    return 0;
}

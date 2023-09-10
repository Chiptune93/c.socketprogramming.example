#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("example.txt", O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;   // 쓰기 잠금 설정
    lock.l_start = 0;        // 파일의 시작부터
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;          // 전체 파일을 잠금

    int result = fcntl(fd, F_SETLK, &lock);
    if (result == -1) {
        perror("fcntl");
        return 1;
    }

    printf("파일 잠금 설정 완료\n");

    // 잠금 해제
    lock.l_type = F_UNLCK;
    result = fcntl(fd, F_SETLK, &lock);

    close(fd);

    return 0;
}

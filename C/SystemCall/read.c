#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char buffer[1024];
    ssize_t num_read = read(fd, buffer, sizeof(buffer));
    if (num_read == -1) {
        perror("read");
        return 1;
    }

    buffer[num_read] = '\0';  // 문자열 종료
    printf("읽은 내용: %s", buffer);

    close(fd);
    return 0;
}

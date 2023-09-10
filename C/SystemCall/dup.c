#include <stdio.h>
#include <unistd.h>

int main() {
    int original_fd = open("example.txt", O_RDONLY);
    if (original_fd == -1) {
        perror("open");
        return 1;
    }

    int copied_fd = dup(original_fd);
    if (copied_fd == -1) {
        perror("dup");
        return 1;
    }

    // original_fd와 copied_fd를 사용하여 파일 읽기 등의 작업을 수행할 수 있습니다.

    close(original_fd);
    close(copied_fd);

    return 0;
}

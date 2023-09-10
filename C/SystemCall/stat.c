#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct stat file_info;
    if (stat("example.txt", &file_info) == -1) {
        perror("stat");
        return 1;
    }

    printf("파일 크기: %ld 바이트\n", (long)file_info.st_size);
    printf("최근 수정 시간: %s", ctime(&file_info.st_mtime));

    return 0;
}

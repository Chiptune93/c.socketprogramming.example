#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    if (mkdir("my_directory", 0755) == -1) {
        perror("mkdir");
        return 1;
    }

    printf("디렉토리 생성 완료\n");

    if (rmdir("my_directory") == -1) {
        perror("rmdir");
        return 1;
    }

    printf("디렉토리 삭제 완료\n");

    return 0;
}

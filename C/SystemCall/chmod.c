#include <stdio.h>
#include <sys/stat.h>

int main() {
    const char *filename = "example.txt";
    mode_t new_mode = S_IRUSR | S_IWUSR;  // 새로운 파일 권한 (읽기 및 쓰기 권한)

    int result = chmod(filename, new_mode);

    if (result == -1) {
        perror("chmod");
        return 1;
    }

    printf("%s의 파일 권한을 변경했습니다.\n", filename);

    return 0;
}

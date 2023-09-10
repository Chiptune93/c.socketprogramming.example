#include <stdio.h>

int main() {
    const char *old_filename = "oldfile.txt";
    const char *new_filename = "newfile.txt";

    int result = rename(old_filename, new_filename);

    if (result == -1) {
        perror("rename");
        return 1;
    }

    printf("%s 파일을 %s로 이동 또는 이름 변경했습니다.\n", old_filename, new_filename);

    return 0;
}

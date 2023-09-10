#include <stdio.h>
#include <stdlib.h>

int main() {
    // 환경 변수 설정
    setenv("MY_VARIABLE", "Hello, Environment!", 1);

    // 환경 변수 가져오기
    char *value = getenv("MY_VARIABLE");
    if (value != NULL) {
        printf("MY_VARIABLE 값: %s\n", value);
    } else {
        printf("MY_VARIABLE를 찾을 수 없습니다.\n");
    }

    return 0;
}

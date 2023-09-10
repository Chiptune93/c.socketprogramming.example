#include <stdio.h>
#include <unistd.h>

int main() {
    char str[] = "Hello, World!\n";
    int len = sizeof(str) - 1;  // 문자열 길이 계산

    // write 시스템 콜 호출
    // write 시스템 콜을 사용하여 표준 출력 파일 디스크립터인 STDOUT_FILENO를 통해 문자열을 출력합니다. 
    // write 시스템 콜은 호출 결과로 쓰여진 바이트 수를 반환하며, 오류가 발생한 경우 -1을 반환하고 perror 함수를 사용하여 오류 메시지를 출력합니다.
    ssize_t bytes_written = write(STDOUT_FILENO, str, len);

    if (bytes_written == -1) {
        perror("write");
        return 1;
    }

    return 0;
}

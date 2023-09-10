#include <stdio.h>
#include <stdlib.h>

int main() {
    int *array;
    int n = 5;

    // 메모리 동적 할당
    array = (int *)malloc(n * sizeof(int));
    if (array == NULL) {
        perror("malloc");
        return 1;
    }

    // 배열 초기화
    for (int i = 0; i < n; i++) {
        array[i] = i * 2;
    }

    // 배열 출력
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
   

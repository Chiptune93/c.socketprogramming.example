#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

struct msg_buffer {
    long msg_type;
    char msg_text[100];
};

int main() {
    key_t key;
    int msg_id;
    struct msg_buffer msg;

    key = ftok("/tmp", 'a'); // 키 생성
    msg_id = msgget(key, 0666 | IPC_CREAT); // 메시지 큐 생성

    msg.msg_type = 1;
    strcpy(msg.msg_text, "안녕하세요, 메시지 큐!");

    // 메시지 전송
    msgsnd(msg_id, &msg, sizeof(msg), 0);
    printf("메시지 전송: %s\n", msg.msg_text);

    // 메시지 수신
    msgrcv(msg_id, &msg, sizeof(msg), 1, 0);
    printf("메시지 수신: %s\n", msg.msg_text);

    // 메시지 큐 삭제
    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}

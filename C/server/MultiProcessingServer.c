#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// 쓰레드관련 헤더
#include <pthread.h>

// 쓰레드 메인 함수
void thread_main(client_socket) {
    // 쓰레드 메인 함수에서는 기존 클라이언트와 연결되었을 때 하던 작업이 들어와야 한다.
    char buffer[1024] = {0};
    ssize_t num_bytes;

    while (1) {
        // 클라이언트로부터 데이터 수신
        num_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (num_bytes == 0) {
            printf("클라이언트 연결이 종료되었습니다.\n");
            break;
        } else if (num_bytes == -1) {
            perror("recv failed");
            break;
        }

        printf("클라이언트로부터 받은 메시지: %s\n", buffer);

        // 클라이언트로 데이터 전송
        send(client_socket, buffer, num_bytes, 0);
    }

    // 소켓 종료
    close(client_socket);

    // 쓰레드 메인 함수 이므로, 쓰레드를 종료하는 구문이 추가되어야 함.
    pthread_exit(NULL);
}

int main() {

    pthread_t thread_id;

    // 소켓 생성
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345); // 포트 번호 12345로 설정

    // 소켓과 서버 주소를 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 접속 요청 대기
    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("서버가 클라이언트 연결을 기다리는 중...\n");


    // ===== 기존 클라이언트 연결부 + 데이터 처리부를 멀티스레딩으로 전환 =====

    // 클라이언트가 여러 개 접속할 수 있으므로, 접속 대기 및 스레드 생성을 무한 반복으로 돌림.
    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        // 클라이언트가 서버에 연결을 요청하면 accept 함수를 호출하여 해당 연결을 수락하고 통신을 위한 새로운 소켓을 생성합니다. 이후 서버와 클라이언트는 생성된 소켓을 통해 데이터를 주고받을 수 있습니다.
        // accept 함수가 호출되면, 연결 요청이 있을 때까지 함수는 블로킹됩니다. 연결 요청이 들어오면 함수는 새로운 클라이언트 소켓을 생성하고 해당 클라이언트와의 통신에 사용할 수 있도록 반환합니다. 만약 에러가 발생하면 -1을 반환합니다.
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            // 연결 에러 발생 하여도 서버는 죽으면 안됨.
            perror("accept failed");
            continue;
            // close(server_socket);
            // exit(EXIT_FAILURE);
        }
        // 위에서 에러가 발생하지 않았다 -> 클라이언트 연결이 되었다.
        // 쓰레드 생성하여 연결 처리를 함.
        printf("새로운 클라이언트가 접속하였습니다.\n");

        // 클라이언트 쓰레드 생성
        // 쓰레드 메인 함수 정의부 필요.
        pthread_create(&thread_id,NULL,thread_main,client_socket);

        // 쓰레드가 종료되기를 기다림
        // pthread_join(thread_id,NULL);
        // -> 하나의 스레드를 생성하고 그 스레드가 종료되기를 기다리는 설정을 하게되면
        //    한 번의 하나의 클라이언트 밖에 접속을 하지 못한다.
        // -> 클라이언트의 메세지를 계속 기다리는 작업의 경우
        //    특정 작업이 끝나려면 사용자가 빈 값을 입력해야 하는 상황이므로
        //    대기만 하게 되면 다른 클라이언트가 동시 접속을 못한다.
        //    따라서 대기 인원 (멀티쓰레드개수) 를 제한하는 방법 등을 사용하여야 한다.


    }

    // // 클라이언트와 연결 수락
    // struct sockaddr_in client_addr;
    // socklen_t addr_len = sizeof(client_addr);
    // // 클라이언트가 서버에 연결을 요청하면 accept 함수를 호출하여 해당 연결을 수락하고 통신을 위한 새로운 소켓을 생성합니다. 이후 서버와 클라이언트는 생성된 소켓을 통해 데이터를 주고받을 수 있습니다.
    // // accept 함수가 호출되면, 연결 요청이 있을 때까지 함수는 블로킹됩니다. 연결 요청이 들어오면 함수는 새로운 클라이언트 소켓을 생성하고 해당 클라이언트와의 통신에 사용할 수 있도록 반환합니다. 만약 에러가 발생하면 -1을 반환합니다.
    // int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    // if (client_socket == -1) {
    //     perror("accept failed");
    //     close(server_socket);
    //     exit(EXIT_FAILURE);
    // }

    // printf("클라이언트가 연결되었습니다.\n");

    // // 클라이언트와의 통신 처리
    // char buffer[1024] = {0};
    // ssize_t num_bytes;

    // while (1) {
    //     // 클라이언트로부터 데이터 수신
    //     num_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
    //     if (num_bytes == 0) {
    //         printf("클라이언트 연결이 종료되었습니다.\n");
    //         break;
    //     } else if (num_bytes == -1) {
    //         perror("recv failed");
    //         break;
    //     }

    //     printf("클라이언트로부터 받은 메시지: %s\n", buffer);

    //     // 클라이언트로 데이터 전송
    //     send(client_socket, buffer, num_bytes, 0);
    // }

    // // 소켓 종료
    // close(client_socket);

    // ===== 기존 클라이언트 연결부 + 데이터 처리부를 멀티스레딩으로 전환 =====


    close(server_socket);

    return 0;
}

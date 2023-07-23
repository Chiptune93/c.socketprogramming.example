#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
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

    // 클라이언트와 연결 수락
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    // 클라이언트가 서버에 연결을 요청하면 accept 함수를 호출하여 해당 연결을 수락하고 통신을 위한 새로운 소켓을 생성합니다. 이후 서버와 클라이언트는 생성된 소켓을 통해 데이터를 주고받을 수 있습니다.
    // accept 함수가 호출되면, 연결 요청이 있을 때까지 함수는 블로킹됩니다. 연결 요청이 들어오면 함수는 새로운 클라이언트 소켓을 생성하고 해당 클라이언트와의 통신에 사용할 수 있도록 반환합니다. 만약 에러가 발생하면 -1을 반환합니다.
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == -1) {
        perror("accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("클라이언트가 연결되었습니다.\n");

    // 클라이언트와의 통신 처리
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
    close(server_socket);

    return 0;
}

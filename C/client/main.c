#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    
    // AF_INET는 "Address Family Internet"의 약자로, 인터넷 주소 체계를 사용하는 네트워크 소켓 프로그래밍에서 주소 체계를 지칭합니다. 이것은 주로 IPv4 주소 체계를 의미합니다. 

    // SOCK_STREAM는 소켓 프로그래밍에서 사용되는 용어로, 한국어로는 "스트림 소켓"이라고 번역할 수 있습니다. 
    //  소켓은 컴퓨터 네트워크 간에 통신을 가능하게 하는 프로그래밍 인터페이스를 말하며, 이러한 소켓은 여러 종류가 있습니다. SOCK_STREAM은 TCP 프로토콜을 기반으로 동작하는 연결 지향적인 소켓 유형입니다.
    //  TCP (Transmission Control Protocol)는 신뢰성이 있고 연결 지향적인 프로토콜로, 데이터를 패킷 단위로 분할하여 전송하고, 패킷의 전송 순서를 보장하며, 손실된 패킷을 다시 요청하고 재전송하여 데이터의 정확성과 완전성을 보장합니다. 이러한 특성으로 인해 데이터를 신뢰성 있게 전송해야 하는 애플리케이션에 주로 사용됩니다.
    //  반면, SOCK_DGRAM은 UDP 프로토콜을 기반으로 하는 데이터그램 소켓으로, 데이터 전송을 보장하지 않고 신뢰성이 낮지만, 전송 속도가 빠르고 연결 설정 과정이 없기 때문에 실시간 통신이 요구되는 애플리케이션에서 사용됩니다.
    //  따라서 SOCK_STREAM은 데이터를 연결 지향적으로, 신뢰성 있게 전송해야 할 경우 사용되는 소켓 유형이라고 할 수 있습니다.

    // 소켓 생성
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.11"); // 실제 서버의 IP 주소로 대체해야 함
    server_addr.sin_port = htons(12345); // 서버와 동일한 포트 번호 12345 사용

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결되었습니다.\n");

    // 서버와의 통신 처리
    char message[1024];

    while (1) {
        printf("메시지를 입력하세요 (빈 줄이면 종료): ");
        fgets(message, sizeof(message), stdin);

        // 입력한 메시지를 서버로 전송
        send(client_socket, message, strlen(message), 0);

        // 입력한 메시지가 비어있다면 클라이언트 종료
        if (strlen(message) == 1) {
            printf("통신을 종료합니다.\n");
            break;
        }

        // 서버로부터 데이터 수신
        ssize_t num_bytes = recv(client_socket, message, sizeof(message), 0);
        if (num_bytes == 0) {
            printf("서버 연결이 종료되었습니다.\n");
            break;
        } else if (num_bytes == -1) {
            perror("recv failed");
            break;
        }

        // 서버로부터 받은 데이터 출력
        printf("서버로부터 받은 메시지: %s\n", message);
    }

    // 소켓 종료
    close(client_socket);

    return 0;
}

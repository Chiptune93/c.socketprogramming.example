#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10

int globalVar = 0; // 공유 자원으로 사용할 전역 변수

void handleClient(int clientSocket) {
    char buffer[1024];
    int readSize;

    while ((readSize = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[readSize] = '\0';
        printf("Received message from client: %s\n", buffer);

        // 경쟁 상태 발생 가능성이 있는 부분
        globalVar++;
        printf("globalVar is now: %d\n", globalVar);

        send(clientSocket, "Message received.", strlen("Message received."), 0);
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    int clientSockets[MAX_CLIENTS] = {0}; // 연결된 클라이언트 소켓들의 배열
    fd_set readfds; // 파일 디스크립터 집합

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding");
        exit(1);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Error listening");
        exit(1);
    }

    printf("Server started. Waiting for connections...\n");

    int maxSocket = serverSocket; // select 함수에 사용할 가장 큰 소켓 디스크립터 값

    while (1) {
        FD_ZERO(&readfds); // readfds 초기화
        FD_SET(serverSocket, &readfds); // 서버 소켓을 readfds에 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] > 0) {
                FD_SET(clientSockets[i], &readfds); // 연결된 클라이언트 소켓들을 readfds에 추가
            }
            if (clientSockets[i] > maxSocket) {
                maxSocket = clientSockets[i]; // 최대 소켓 디스크립터 값 갱신
            }
        }

        // select 함수 호출
        int activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error in select");
            exit(1);
        }

        // 클라이언트 연결 처리
        if (FD_ISSET(serverSocket, &readfds)) {
            clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
            if (clientSocket < 0) {
                perror("Error accepting connection");
                continue;
            }

            printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            // 연결된 클라이언트 소켓을 배열에 추가
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = clientSocket;
                    break;
                }
            }
        }

        // 클라이언트 메시지 처리
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int socketDesc = clientSockets[i];
            if (FD_ISSET(socketDesc, &readfds)) {
                handleClient(socketDesc);
            }
        }
    }

    close(serverSocket);
    return 0;
}

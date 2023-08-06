#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    // char buffer[1024];
    int readSize;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(1);
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {

        // Stress Test
        for(int i = 0; i < 1000; i++) {
            char buffer[1024];
            sprintf(buffer, "%s %d", "[p_client] Test Number -> ", i);
            send(clientSocket, buffer, strlen(buffer), 0);

            // 서버로부터 응답 받기
            readSize = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[readSize] = '\0';
            printf("[p_client] Server response: %s\n", buffer);
        }
        
    }

    close(clientSocket);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10

int globalVar = 0; // 공유 자원으로 사용할 전역 변수

// 하나의 클라이언트를 핸들링 하는 함수.
// 한 개의 스레드를 통해 관리된다.
void *handleClient(void *arg)
{
    int clientSocket = *((int *)arg);
    char buffer[1024];
    int readSize;

    while ((readSize = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[readSize] = '\0';
        printf("Received message from client: %s\n", buffer);

        // 경쟁 상태 발생 가능성이 있는 부분
        // 전역 변수에 접근하여 1증가 후, 해당 값을 클라이언트에 보낸다.
        globalVar++;
        printf("globalVar is now: %d\n", globalVar);

        char result[50];
        sprintf(result, "%s %d", "globalVar is", globalVar);

        send(clientSocket, result, strlen(result), 0);
    }

    close(clientSocket);
    free(arg);
    return NULL;
}

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    int *clientSocketPtr;
    pthread_t threadId;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error binding");
        exit(1);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        perror("Error listening");
        exit(1);
    }

    printf("Server started. Waiting for connections...\n");

    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket < 0)
        {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // 클라이언트 소켓을 메모리에 할당한다.
        clientSocketPtr = (int *)malloc(sizeof(int));
        *clientSocketPtr = clientSocket;

        // 클라이언트 연결을 별도의 스레드에서 처리한다.
        // 스레드 1개를 생성하여, 핸들링 함수와 메모리에 할당된 클라이언트 소켓을 전달한다.
        if (pthread_create(&threadId, NULL, handleClient, (void *)clientSocketPtr) != 0)
        {
            perror("Error creating thread");
            // 에러가 발생하는 경우, 소켓을 닫고 메모리를 해제한다.
            close(clientSocket);
            free(clientSocketPtr);
        }
    }

    close(serverSocket);
    return 0;
}

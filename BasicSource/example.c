#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define BACKLOG       128     // listen()를 위한 대기 큐의 크기
#define DEFAULT_PORT  9999    // 기본 포트 번호
#define BUF_SIZE      1024    // 메시지 조각 크기

time_t ticks;

typedef struct {
  int client_sock;
} thread_param_t;
thread_param_t *params;       // pthread를 위한 params 구조체
pthread_t thread_id;          // 스레드 식별자

void error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

/**
 * accept() 이후의 스레드 핸들러
 */
void *handle_client(void *params) {
  char msg[BUF_SIZE];
  thread_param_t *p = (thread_param_t *) params;  // 스레드 파라미터

  // 메시지 버퍼 초기화
  memset(msg, 0, sizeof(msg));

  // 현재 시간 정보를 메시지로 생성
  ticks = time(NULL);
  snprintf(msg, sizeof(msg), "%.24s\r\n", ctime(&ticks));

  // 클라이언트에게 메시지 전송
  write(p->client_sock, msg, strlen(msg));
  printf("Client #%d에게 메시지 전송\n", p->client_sock);

  // 대기
  usleep(10);

  // 메시지 버퍼 초기화
  memset(msg, 0, sizeof(msg));

  // 액티브 클로즈, `TIME_WAIT` 상태로 남습니다.
  if (close(p->client_sock) < 0)
    error("에러: close()");
  free(p);

  return NULL;
}

int main(int argc, char *argv[]) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;
  struct ifreq ifr;

  // 서버의 인터넷 주소 생성 및 초기화
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;                // 인터넷 주소 패밀리
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 어떤 인터페이스에서든 연결 허용
  serv_addr.sin_port = htons(DEFAULT_PORT);      // 사용할 포트 번호

  // 소켓 생성, 바인딩, 대기
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    error("에러: socket()");
  if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("에러: bind() 권한 부족(<1024) 또는 이미 사용 중");
  if (listen(listenfd, BACKLOG) < 0)
    error("에러: listen()");

  ifr.ifr_addr.sa_family = AF_INET;               // 인터넷 주소 패밀리
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);      // "eth0"에 연결된 IP 주소
  ioctl(listenfd, SIOCGIFADDR, &ifr);             // ifnet 주소 가져오기, 소켓 파일이 TCP/IP의 원리로 맞게 움직일 수 있도록 매핑.
  // 소켓이 Server/Client 소켓이 존재. 구분해서 어떤 소켓이 들어간지, 옵션이 어떤게 들어간지 중요하다.
  // 왜 소켓은 파일이냐? TCP/IP 원리로 파일을 매핑하여 연결하는 것.
  // fd -> 파일 기술자 -> 이를 컨트롤함에 따라 TCP/IP가 커널 옵션을 통해 자동으로 움직이게 하는. (커널 컨트롤, 옵셔닝)
  // OS에 의해서 커널이 콜링 되는 것.

  // <IP>:<Port> 출력
  printf("%s:%d에서 대기 중\n",
         inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
         DEFAULT_PORT);

  while (1) {
    connfd = accept(listenfd, (struct sockaddr *) NULL, NULL); // 함수 원형 체크.
    // 서버와 클라이언트의 구조를 잘 가지고 있어야 한다.
    // 웹으로 바꾸면 데이터가 왔다갔다 함. connfd -> jsessionid 로 바꾸어 사용한다. (똑같이 사용함.)
    // 웹이랑 똑같다. 세션을 이용한다 -> 이 번호로 모든걸 컨트롤하기 때문 -> request/인증 등등 ..

    // `thread_param_t` 구조체를 위한 메모리 할당 ( 힙 메모리에 할당. )
    params = malloc(sizeof(thread_param_t));
    // 해당 구조체를 클라이언트 소켓에 할당.
    params->client_sock = connfd;

    // 1 클라이언트, 1 스레드
    // 소켓 파일에서 이벤트를 찾아 기다리고 처리. (블로킹방식)
    // 비동기 방식은 서버가 직접 응답을 찾아냄 (논블로킹방식)
    // 헤더가 어떻게 나가는지 차이가 있음. 봐야됨.
    pthread_create(&thread_id, NULL, handle_client, (void *) params);
    pthread_detach(thread_id);
  }
}

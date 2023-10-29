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

#define BACKLOG 128       // listen()를 위한 대기 큐의 크기
#define DEFAULT_PORT 5000 // 기본 포트 번호
#define BUF_SIZE 1024     // 메시지 조각 크기

time_t ticks;

typedef struct
{
    int client_sock;
} thread_param_t;
thread_param_t *params; // pthread를 위한 params 구조체
pthread_t thread_id;    // 스레드 식별자

void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * accept() 이후의 스레드 핸들러
 */
void *handle_client(void *params)
{
    char msg[BUF_SIZE];
    thread_param_t *p = (thread_param_t *)params; // 스레드 파라미터

    // 메시지 버퍼 초기화
    // memset -> [_string.h] __builtin___memset_chk (dest, __VA_ARGS__, __darwin_obsz0 (dest))
    // 함수가 따로 정의되지 않은 경우, 내장 함수인 __builtin___memset_chk 을 호출한다.
    // __builtin__ ... 함수는 컴파일러 내장함수로써 시스템의 프로세서에 접근하게 해주는(시스템콜을 하게 해주는) c/c++ 확장이다.
    // 보통의 OS에서 memset은 메모리 블록을 모두 0 바이트로 설정하여 이전 정보를 모두 지운다.
    // 어셈블리 언어로 구현되며 x86 "문자열" 명령어 또는 ARM "다중 레지스터 저장소"와 같은 작업을 사용하여 속도를 높일 수 있다.
    memset(msg, 0, sizeof(msg));

    // 현재 시간 정보를 메시지로 생성
    // time()
    // -> [_time_t.h] typedef __darwin_time_t         time_t;
    // -> [_types.h] typedef long                    __darwin_time_t;        /* time() */
    // 기본 플랫폼(OS 및/또는 HW)에 따라 어떻게 동작하는지 다르다. C 언어 표준은 시간 관련 기능이 내부적으로 어떻게 구현되어야 하는지를 지시하지 않습니다.
    // 대부분의 플랫폼에서 이 함수는 아마도 OS 커널에 대해 낮은 수준의 시스템 호출을 수행할 것 입니다. 즉, 시작할 OS가 있는 경우입니다.
    // 그렇지 않은 경우 HW 시계 또는 유사한 항목에 액세스할 수 있습니다.
    // C 런타임 소스의 복사본을 찾을 수 있다면 주어진 표준 C 함수가 (적어도 특정 컴파일러/OS/플랫폼에 대해) 어떻게 구현되는지 확인할 수 있습니다.
    // MAC의 표준 시간 함수 https://opensource.apple.com/source/Libc/Libc-167/gen.subproj/ctime.c.auto.html
    ticks = time(NULL);
    // ctime()
    // -> localtime 형식대로 시간을 문자 형태로 반환
    // snprintf() : 버퍼에 문자의 형식과 값을 지정하고 저장한다. 그리고 출력한다.
    // -> [stdio.h] int	 sprintf(char * __restrict, const char * __restrict, ...) __printflike(2, 3);
    // -> [cdefs.h] #define __printflike(fmtarg, firstvararg) \ // __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
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

int main(int argc, char *argv[])
{
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
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("에러: bind() 권한 부족(<1024) 또는 이미 사용 중");
    if (listen(listenfd, BACKLOG) < 0)
        error("에러: listen()");

    ifr.ifr_addr.sa_family = AF_INET;            // 인터넷 주소 패밀리
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1); // "eth0"에 연결된 IP 주소
    ioctl(listenfd, SIOCGIFADDR, &ifr);          // ifnet 주소 가져오기, 소켓 파일이 TCP/IP의 원리로 맞게 움직일 수 있도록 매핑.
    // 소켓이 Server/Client 소켓이 존재. 구분해서 어떤 소켓이 들어간지, 옵션이 어떤게 들어간지 중요하다.
    // 왜 소켓은 파일이냐? TCP/IP 원리로 파일을 매핑하여 연결하는 것.
    // fd -> 파일 기술자 -> 이를 컨트롤함에 따라 TCP/IP가 커널 옵션을 통해 자동으로 움직이게 하는. (커널 컨트롤, 옵셔닝)
    // OS에 의해서 커널이 콜링 되는 것.

    // <IP>:<Port> 출력
    printf("%s:%d에서 대기 중\n",
           inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
           DEFAULT_PORT);

    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL); // 함수 원형 체크.
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
        pthread_create(&thread_id, NULL, handle_client, (void *)params);
        pthread_detach(thread_id);
    }
}
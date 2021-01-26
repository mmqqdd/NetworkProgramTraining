#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
/*
 * 服务器端模版
 * 完成socket()、bind()、listen();
 * 并提供错误打印、成功打印
 */
#define BUF_SIZE 1024
#define EPOLL_SIZE 50

void error_handling(char*);               //打印错误信息
void succ(char*);                         //打印成功信息
void sock_init(int*, sockaddr_in*, int);  //初始化套接字
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t sock_len;
    char buf[BUF_SIZE];

    int epfd;
    int timeout = 5000;
    struct epoll_event event, *events;

    sock_init(&serv_sock, &serv_addr, atoi(argv[1]));

    epfd = epoll_create(EPOLL_SIZE);
    events = (epoll_event*)malloc(sizeof(epoll_event) * EPOLL_SIZE);
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while (1) {
        int cnt = epoll_wait(epfd, events, EPOLL_SIZE, timeout);
        int i;
        if (cnt == 0) {
            printf("time out\n");
            continue;
        }
        for (i = 0; i < cnt; i++) {
            if (events[i].data.fd == serv_sock) {
                clnt_sock =
                    accept(serv_sock, (struct sockaddr*)&clnt_addr, &sock_len);
                if (clnt_sock == -1)
                    error_handling((char*)"accept()");
                else
                    succ((char*)"accept()");
                event.data.fd = clnt_sock;
                event.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
            } else {
                int read_len = read(events[i].data.fd, buf, BUF_SIZE);
                if (read_len <= 0) {
                    fprintf(stdout, "sock %d has closed\n`", events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    continue;
                }
                buf[read_len] = 0;
                write(events[i].data.fd, buf, read_len);
                fprintf(stdout, "sock %d message: %s\n", events[i].data.fd,
                        buf);
            }
        }
    }
    epoll_ctl(epfd, EPOLL_CTL_DEL, serv_sock, NULL);
    close(serv_sock);
}

void error_handling(char* mess) {
    fprintf(stderr, "pid %d %s error\n", getpid(), mess);
    exit(1);
}
void succ(char* mess) {
    fprintf(stdout, "pid %d %s successed\n", getpid(), mess);
}

void sock_init(int* sock, sockaddr_in* addr, int port) {
    *sock = socket(PF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
        error_handling((char*)"socket()");
    else
        succ((char*)"socket()");
    memset(addr, 0, sizeof(sockaddr));
    addr->sin_family = PF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(port);

    if (bind(*sock, (sockaddr*)addr, sizeof(sockaddr)) == -1)
        error_handling((char*)"bind()");
    else
        succ((char*)"bind()");

    if (listen(*sock, 5) == -1)
        error_handling((char*)"listen()");
    else
        succ((char*)"listen()");
}

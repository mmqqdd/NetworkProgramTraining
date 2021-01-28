#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

/*
 * 多人聊天服务器
 * 原理:
 *  1. 多线程处理每一个连接
 *  2. 一人发消息, 往所有人都发送一遍
 *  3. mutex解决临界区问题
 *
 */

#define CLNT_MAX_NUM 100
#define NAME_MAX_LEN 30
void error_handling(char*);               //打印错误信息
void succ(char*);                         //打印成功信息
void sock_init(int*, sockaddr_in*, int);  //初始化套接字
void* handle_clnt(void*);

int clnt_cnt = 0, socks[CLNT_MAX_NUM];
pthread_mutex_t mutx;
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t sock_len;
    pthread_t tr_id;
    pthread_mutex_init(&mutx, NULL);
    sock_init(&serv_sock, &serv_addr, atoi(argv[1]));
    while (1) {
        if ((clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &sock_len)) ==
            -1)
            error_handling((char*)"accept()");
        else
            succ((char*)"accept()");
        socks[clnt_cnt++] = clnt_sock;
        pthread_create(&tr_id, NULL, handle_clnt,
                       (void*)(socks + clnt_cnt - 1));
        pthread_detach(tr_id);
    }
    pthread_mutex_destroy(&mutx);
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

void* handle_clnt(void* args) {
    int sock = *((int*)args);
    int name_len, str_len;
    int i;
    char buf[BUFSIZ], name[NAME_MAX_LEN];

    // printf("%d %d\n",sock,name_len);
    read(sock, &name_len, sizeof(name_len));
    pthread_mutex_lock(&mutx);
    // printf("%d %d\n",sock,name_len);
    read(sock, name, name_len);
    name[name_len] = 0;
    pthread_mutex_unlock(&mutx);
    fprintf(stdout, "----------\n");
    while (1) {
        int sig = read(sock, &str_len, sizeof(str_len));
        pthread_mutex_lock(&mutx);      
        if (str_len == -1 || sig == -1 || sig == 0) {
            pthread_mutex_unlock(&mutx);
            fprintf(stderr, "%s 用户退出\n", name);
            break;
        }
        read(sock, buf, str_len);
        pthread_mutex_unlock(&mutx);
        buf[str_len] = 0;
        printf("--%s: %s\n", name, buf);
        for (i = 0; i < clnt_cnt; i++) {
            pthread_mutex_lock(&mutx);
            write(socks[i], &name_len, sizeof(name_len));
            write(socks[i], name, name_len);
            write(socks[i], &str_len, sizeof(str_len));
            write(socks[i], buf, str_len);
            pthread_mutex_unlock(&mutx);
        }
    }
    i = 0;
    while (socks[i] != sock)
        i++;
    clnt_cnt--;
    while (i < clnt_cnt)
        socks[i] = socks[i + 1];
    close(sock);
}
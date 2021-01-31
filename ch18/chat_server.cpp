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
int mqd_read(int, char*);
int mqd_write(int, char*);
void write_to_all(char*, char*);
void remove_sock(int);
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
        pthread_mutex_lock(&mutx);
        socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);
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
    int sig;
    char buf[BUFSIZ], name[NAME_MAX_LEN];

    // pthread_mutex_lock(&mutx); //注意这里不是临界区,
    // 因为只有一个线程会read(sock)
    name_len = mqd_read(sock, name);
    name[name_len] = 0;
    // pthread_mutex_unlock(&mutx);
    fprintf(stdout, "----------\n");
    while (1) {
        int str_len = mqd_read(sock, buf);
        if (str_len == -1) {
            fprintf(stderr, "%s 用户退出\n", name);
            break;
        }
        if (str_len == 0) {
            fprintf(stderr, "%s 异常退出\n", name);
            write_to_all(name, (char*)"异常退出");
            break;
        }
        buf[str_len] = 0;
        write_to_all(name, buf);
    }
    remove_sock(sock);
    return NULL;
}
void write_to_all(char* name, char* buf) {
    int i;
    char mesg[BUFSIZ];
    sprintf(mesg, "[%s]: %s", name, buf);
    printf("%s\n", mesg);
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++) {
        mqd_write(socks[i], mesg);
    }
    pthread_mutex_unlock(&mutx);
}
void remove_sock(int sock) {
    pthread_mutex_lock(&mutx);
    int i = 0;
    while (socks[i] != sock)
        i++;
    clnt_cnt--;
    while (i < clnt_cnt)
        socks[i] = socks[i + 1];
    pthread_mutex_unlock(&mutx);
    close(sock);
}
int mqd_write(int sock, char* s) {
    int str_len = strlen(s);
    static char buf[BUFSIZ + 4];
    for (int i = 0; i < 4; i++) {
        sprintf(buf + i, "%c", (char)str_len % 16);
        str_len /= 16;
    }
    sprintf(buf + 4, "%s", s);
    write(sock, buf,
          strlen(s) + 4);  //注意不是strlen(buf),因为前面四个字节可能出现\0
    return strlen(s);
}
int mqd_read(int sock, char* buf) {
    int str_len = 0;
    int x = 1;
    int sig = read(sock, buf, 4);
    if (sig == -1 || sig == 0)
        return sig;
    for (int i = 0; i < 4; i++) {
        str_len += x * (int)buf[i];
        x *= 16;
    }
    if (str_len == -1)
        return 0;
    read(sock, buf, str_len);
    buf[str_len] = 0;
    return str_len;
}
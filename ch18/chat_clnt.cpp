#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * 多人聊天客户端
 * 原理:
 *  1. 开两个线程, 一个负责读, 一个负责写
 *  2. 写和读的时候互斥一下
 */

#define BUF_SIZE 1024
#define CLNT_MAX_NUM 100
#define NAME_MAX_LEN 30

void error_handling(char* message);
void succ(char* message);
void init_sock(int*, char*, char*);
int mqd_read(int, char*);
int mqd_write(int, char*);
void* read_from_server(void*);
void* write_to_server(void*);

bool isClose = 0;  //进程结束的标志
int main(int argc, char** argv) {
    char buf[3][20] = {"chat_clnt", "121.37.129.143", "9190"};
    if (argc != 3) {
        // printf("Usage: %s <IP> <port>\n", argv[0]);
        // exit(1);
        argv[1] = buf[1];
        argv[2] = buf[2];
    }
    int sock;
    struct sockaddr_in serv_addr;
    init_sock(&sock, argv[1], argv[2]);
    pthread_t tr_id1, tr_id2;

    pthread_create(&tr_id1, NULL, read_from_server, (void*)&sock);
    pthread_create(&tr_id2, NULL, write_to_server, (void*)&sock);
    // pthread_join(tr_id1, NULL);
    pthread_join(tr_id2, NULL);
    close(sock);
}

void error_handling(char* message) {
    fprintf(stderr, "%s error\n", message);
    exit(1);
}
void succ(char* message) {
    fprintf(stdout, "%s successed\n", message);
}

void init_sock(int* sock, char* ip, char* port) {
    struct sockaddr_in serv_addr;
    *sock = socket(PF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
        error_handling((char*)"socket()");
    else
        succ((char*)"socket()");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(atoi(port));
    if (connect(*sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling((char*)"connect");
    else
        succ((char*)"connect");
}

void* read_from_server(void* args) {
    int sock = *((int*)args);
    int str_len, name_len;
    char name[NAME_MAX_LEN], buf[BUF_SIZE];
    while (1) {
        if (isClose)
            break;
        int str_len = mqd_read(sock, buf);
        fprintf(stdout, "%*c%s\n", 10,' ', buf);
    }
    close(sock);
    return NULL;
}

void* write_to_server(void* args) {
    int sock = *((int*)args);
    char name[NAME_MAX_LEN], buf[BUF_SIZE], *str;

    fprintf(stdout, "----多人聊天极简版-----\n请输入你的名字或昵称: ");
    fgets(name, NAME_MAX_LEN, stdin);
    name[strlen(name) - 1] = 0;
    mqd_write(sock, name);
    mqd_write(sock, (char*)"加入聊天室");
    fprintf(stdout,
            "开始多人聊天聊天, 可以开始输入, 按回车发送, 输入q结束程序\n");

    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        buf[strlen(buf) - 1] = 0;
        if (strlen(buf) == 0)
            continue;
        if (strlen(buf) == 1 && buf[0] == 'q') {
            mqd_write(sock, (char*)"退出聊天室");
            break;
        }
        mqd_write(sock, buf);
    }
    fprintf(stdout, "程序结束, 欢迎下次再来\n");
    isClose = 1;
    return NULL;
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
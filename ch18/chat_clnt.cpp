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
int read_msg(int, char*, int);
void* read_from_server(void*);
void* write_to_server(void*);

pthread_mutex_t mutx_read, mutx_write;
bool isClose = 0;
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
    pthread_mutex_init(&mutx_read, NULL);
    pthread_mutex_init(&mutx_write, NULL);
    pthread_create(&tr_id1, NULL, read_from_server, (void*)&sock);
    pthread_create(&tr_id2, NULL, write_to_server, (void*)&sock);
    // pthread_join(tr_id1, NULL);
    pthread_join(tr_id2, NULL);
    pthread_mutex_destroy(&mutx_read);
    pthread_mutex_destroy(&mutx_write);
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
    int name_len, str_len;
    char name[NAME_MAX_LEN], buf[BUF_SIZE];
    while (1) {
        if (isClose)
            break;
        pthread_mutex_lock(&mutx_read);
        int sig = read(sock, &name_len, sizeof(name_len));
        if (sig == -1) {
            exit(1);
        }
        if (name_len == -1) {
            pthread_mutex_unlock(&mutx_read);
            break;
        }
        read_msg(sock, name, name_len);
        read(sock, &str_len, sizeof(str_len));
        read_msg(sock, buf, str_len);
        pthread_mutex_unlock(&mutx_read);
        fprintf(stdout, "%*s: %s\n", 15, name, buf);
    }
    close(sock);
    return NULL;
}

int read_msg(int sock, char* buf, int str_len) {
    int read_len = 0;
    while (read_len < str_len) {
        read_len += read(sock, buf + read_len, str_len - read_len);
        if (read_len < 0)
            return -1;
    }
    buf[read_len] = 0;
    return read_len;
}

void* write_to_server(void* args) {
    int sock = *((int*)args);
    int name_len, str_len;
    char name[NAME_MAX_LEN], buf[BUF_SIZE], *str;

    fprintf(stdout, "----多人聊天极简版-----\n请输入你的名字或昵称: ");
    fgets(name, NAME_MAX_LEN, stdin);
    name_len = strlen(name) - 1;
    name[name_len] = 0;
    pthread_mutex_lock(&mutx_write);
    write(sock, &name_len, sizeof(name_len));
    write(sock, name, name_len);
    str = (char*)"加入聊天室";
    str = (char*)"加入聊天室";
    str_len = strlen(str);
    write(sock, &str_len, sizeof(str_len));
    write(sock, str, str_len);
    pthread_mutex_unlock(&mutx_write);
    fprintf(stdout,
            "开始多人聊天聊天, 可以开始输入, 按回车发送, 输入q结束程序\n");
    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        str_len = strlen(buf) - 1;
        buf[str_len] = 0;
        pthread_mutex_lock(&mutx_write);
        if (str_len == 1 && buf[0] == 'q') {
            char* ptr = (char*)"退出聊天室";

            str_len = strlen(ptr);
            write(sock, &str_len, sizeof(str_len));
            write(sock, ptr, str_len);
            str_len = -1;
            write(sock, &str_len, sizeof(str_len));
            pthread_mutex_unlock(&mutx_write);
            break;
        }
        // fprintf(stdout, "%d %s: %s\n", name_len, name, buf);
        write(sock, &str_len, sizeof(str_len));
        write(sock, buf, str_len);
        pthread_mutex_unlock(&mutx_write);
    }
    fprintf(stdout, "程序结束, 欢迎下次再来\n");
    // close(sock);
    isClose = 1;
    return NULL;
}

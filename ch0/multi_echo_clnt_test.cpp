#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define MAXN 10
/*
 * 创建多个回声客户端, 模拟多个客户端
 *
 */
void error_handling(char* message);
void succ(char* message);
void init_sock(int*, char*, char*);
int echo(int sock, FILE*);
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    int sock[MAXN];
    int i;
    FILE* fp = fopen("multi_echo_clnt_test.cpp", "r");
    for (i = 0; i < MAXN; i++) {
        init_sock(sock + i, argv[1], argv[2]);
    }
    int tot = 0;
    while (tot < MAXN) {
        tot = 0;
        for (i = 0; i < MAXN; i++) {
            if (sock[i] == -1) {
                tot++;
                continue;
            }
            sock[i] = echo(sock[i], fp);
        }
    }
    fclose(fp);
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
int echo(int sock, FILE* fp) {
    char message[BUF_SIZE];
    int str_len, read_len;
    char tp_c;
    // printf("input something:\n");
    message[0] = 0;
    fgets(message,BUF_SIZE, fp);
    str_len = strlen(message);
    if (str_len == 0){
        close(sock);
        return -1;
    }
        write(sock, message, str_len);
    read_len = 0;
    while (read_len < str_len) {
        read_len += read(sock, message + read_len, str_len - read_len);
    }
    message[read_len] = 0;
    printf("message from server is %s\n", message);
    return sock;
}
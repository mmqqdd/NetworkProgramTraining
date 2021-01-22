#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<arpa/inet.h>

/*
 * 服务器端模版
 * 完成socket()、bind()、listen();
 * 并提供错误打印、成功打印
 */ 
void error_handling(char*); //打印错误信息
void succ(char*); //打印成功信息
void sock_init(int* , sockaddr_in* , int); //初始化套接字
int main(int argc, char **argv){
    if (argc != 2){
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }
    int serv_sock;
    struct sockaddr_in serv_addr;
    sock_init(&serv_sock, &serv_addr, atoi(argv[1]));
    
}

void error_handling(char *mess){
    fprintf(stderr, "pid %d %s error\n",getpid(),mess);
    exit(1);
}
void succ(char *mess){
    fprintf(stdout,"pid %d %s successed\n",getpid(),mess);
}

void sock_init(int *sock, sockaddr_in* addr, int port){
    *sock = socket(PF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
        error_handling((char*)"socket()");
    else 
        succ((char*)"socket()");
    memset(addr, 0 ,sizeof(sockaddr));
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
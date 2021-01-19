#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void succ(char *message);

/*
 * UDP回声服务器端
 * 服务器收到什么, 就发送什么, 不会停止.
 */

int main(int argc, char **argv){
    if (argc != 2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    int sock;
    sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_addr_sz;
    int str_len;
    char message[BUF_SIZE];

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling((char*)"UDP socket() error");    
    else 
        succ((char*)"UDP socket()");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
        error_handling((char*)"UDP bind() error");
    else succ((char*)"UDP bind()");
    clnt_addr_sz = sizeof(clnt_addr);
    while( str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_sz)){
        printf("message: %s\n",message);
        sendto(sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, clnt_addr_sz);
    }
    close(sock);
}

void error_handling(char *message){
    fprintf(stdout, "!!!------%s\n",message);
    exit(1);
}

void succ(char *message){
    fprintf(stdout, "@@@------%s successed\n",message);
}
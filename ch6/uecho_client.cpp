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
 * UDP回声客户端
 * 输入什么,传回来什么, 输入q或Q结束
 */
int main(int argc, char **argv){
    if (argc != 3){
        printf("Usage: %s <IP> <port>",argv[0]);
        exit(1);
    }

    int sock;
    sockaddr_in serv_addr, from_addr;
    char message[BUF_SIZE];
    socklen_t sock_addr_sz;
    int str_len;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling((char*)"UDP socket() error");
    else 
        succ((char*)"UDP socket()");
    
    memset(&serv_addr, 0 ,sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    sock_addr_sz = sizeof(serv_addr); //这一句话开始忘记写了, 导致sendto发送不成功
    while(1){
        fputs((char*)"请输入,输入q/Q结束:",stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
            break;
        //printf("%d %d\n",strlen(message),sizeof(message));
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sock_addr_sz);
        succ((char*)"sendto");
        str_len = recvfrom(sock, message, sizeof(message), 0,(struct sockaddr*)&from_addr, &sock_addr_sz);
        printf("recive from server: %s\n",message);
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
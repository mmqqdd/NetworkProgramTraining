#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
/*
 * 这个版本的客户端有些问题, 调用read时可能出现问题, 字符串比较短时没有问题
 */
#define BUF_SIZE 1024
void error_handling(char *message);
void succ(char *message);

int main(int argc, char *argv[]){
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_addr;

    if (argc != 3){
        printf("Usage: %s <IP> <port>\n",argv[0]);
        exit(1);
    }
    
    sock=socket(PF_INET,SOCK_STREAM,0);
    if (sock == -1)
        error_handling((char*)"socket() error");
    else
        succ((char*)"socket()");
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
        error_handling((char*)"connect() error");
    else
        succ((char*)"connect()");
    printf("随便输入(q/Q to quit)\n");
    sleep(30);
    while(1){
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
            break;
        write(sock,message,strlen(message));
        str_len = read(sock,message,BUF_SIZE);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

void succ(char *message){
    fputs((char*)"--------------",stdout);
    fputs(message,stdout);
    fputs((char*)" successed!\n",stdout);
}
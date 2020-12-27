#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
/*
 * 客户端套接字
 * socket函数创建
 * connect函数连接
 */
void error_handling(char * message);

int main(int argc, char* argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;
    printf("服务器端套接字\n");
    if (argc!=3){
        printf("Usage : %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0); //创建一个套接字
    if (sock == -1)
        error_handling((char *)"socket() error");
    printf("服务器端套接字建立成功-------1\n");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]); //设置IP
    serv_addr.sin_port=htons(atoi(argv[2])); //设置端口号
    int test = atoi(argv[1]);
    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
        error_handling((char *)"connect() error");
    printf("服务器端套接字连接成功-------2\n");
    str_len=read(sock,message,sizeof(message)-1);
    if (str_len==-1)
        error_handling((char *)"read() error!");
    printf("Message from server : %s \n",message);
    close(sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
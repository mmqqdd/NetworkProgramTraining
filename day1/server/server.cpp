#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);
int main(int argc, char *argv[]){
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    char message[]= "hello world!";
    printf("服务器端套接字\n");
    if (argc!=2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1){
        error_handling((char*)"socket() error");
    }
    printf("服务器端套接字建立成功-------1\n");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1])); // atoi为字符串转为数字, htons将主机字节顺序转换为网络字节顺序 8888 = 0x22B8 47138 = 0xB822

    if (bind(serv_sock,(struct sockaddr*) &serv_addr,sizeof(serv_addr))==-1){
        error_handling((char*)"bind() error");
    }
    printf("服务器端套接字绑定成功-------2\n");
    if (listen(serv_sock,5)==-1){
        error_handling((char*)"listen() error");
    } 
    printf("服务器端套接字转为接听状态成功-------3\n");
    clnt_addr_size=sizeof(clnt_addr); 
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock==-1){
        error_handling((char*)"accept() error");
    }
    printf("服务器端套接字接收成功-------4\n");
    write(clnt_sock,message,sizeof(message));
    close(clnt_sock);
    close(serv_sock);
    printf("服务器端套接字传送完毕-------5\n");
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    exit(1);
}

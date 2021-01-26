#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024

/*
 * 回声客户端模版
 * 
 */
void error_handling(char *message);
void succ(char *message);
void echo(int sock);
int main(int argc, char** argv){
    if (argc != 3){
        printf("Usage: %s <IP> <port>\n",argv[0]);
        exit(1);
    }
    int sock;
    struct sockaddr_in serv_addr;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling((char*)"socket()");
    else 
        succ((char*)"socket()");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling((char*)"connect");
    else 
        succ((char*)"connect");

    echo(sock);
    close(sock);
}

void error_handling(char *message){
    fprintf(stderr, "%s error\n",message);
    exit(1);
}
void succ(char *message){
    fprintf(stdout,"%s successed\n",message);
}

void echo(int sock){
    char message[BUF_SIZE];
    int str_len, read_len;
    char tp_c;
    printf("input something:\n");
    while(1){
        scanf("%[^\n]",message); scanf("%*c");// 后面加一个%c是为了读取输入缓冲区的换行符, 如果不读出来,会死循环.
        str_len = strlen(message);
        if (str_len == 1 && message[0] == 'q')
            break;
        write(sock, message, str_len);
        read_len = 0;
        while( read_len < str_len){
            read_len += read(sock, message+read_len, str_len-read_len);
        }
        message[read_len] = 0;
        printf("message from server is %s\n",message);
    }
    //pipe();
}
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>

/*
 * 简单计算服务器, 基本是自己写的, 不会命名时参考了书籍, 如何存储也参考了书籍
 * 基本是自己完成. 
 */

#define BUF_SIZE 1024
#define OP_SIZE 4
void error_handling(char *message);
void succ(char *message);
int calculate(int op_cnt, int *opinfo, char op);
void add(int *result, int x, char c);

int main(int argc, char** argv){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_sock_sz;
    char opinfo[BUF_SIZE];
    int result, opt_cnt;
    int recv_len, recv_cnt;
    char c;
    if (argc != 2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling((char*)"socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling((char*)"bind() error");
    else   
        succ((char*)"bind()");

    if (listen(serv_sock, 5) == -1)
        error_handling((char*)"listen() error");
    else succ((char*)"listen()");
    int i;
    for(i=1; i<=3; i++){
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_sock_sz); 
        if (clnt_sock == -1)
            error_handling((char*)"accept() error");
        else 
            succ((char*)"accept()");
        printf("--clint_start--\n ID:%d\nclnt_sock:%d\n",i,clnt_sock);
        read(clnt_sock,&c,1);
        opt_cnt = (int)c;
        recv_len = 0;
        while (recv_len < opt_cnt*OP_SIZE+1){
            recv_len += read(clnt_sock, opinfo, BUF_SIZE);
        }
        c = (char)opinfo[opt_cnt*OP_SIZE];
        result = calculate(opt_cnt, (int *)opinfo, c); //
        write(clnt_sock, &result, sizeof(result));
        close(clnt_sock);
        printf("--clint_ending--\n");
    }
    close(serv_sock);
}

void error_handling(char *message){
    fprintf(stdout, "!!!------%s\n",message);
    exit(1);
}

void succ(char *message){
    fprintf(stdout, "@@@------%s successed\n",message);
}

int calculate(int op_cnt, int *opinfo, char c){
    int i, result = 0;
    for(i = 0; i < op_cnt; i++){
        add(&result,opinfo[i],c);
    } 
    return result;
}

void add(int *result, int x, char c){
    switch(c){
        case '+':
            *result += x; break;
        case '-':
            *result -= x; break;
        case '/':
            *result /= x; break;
        case '*':
            *result *= x; break;
    }
}
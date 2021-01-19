#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024
#define OP_SIZE 4

void error_handling(char *message);
void succ(char *message);

int main(int argc, char **argv){
    int sock;
    struct sockaddr_in clnt_addr;
    char op_info[BUF_SIZE];
    int result;
    if (argc != 3){
        printf("Usage: %s <IP> <port>\n",argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling((char *)"socket() error");
    else 
        succ((char *)"socket()");
    
    memset(&clnt_addr, 0, sizeof(clnt_addr));
    clnt_addr.sin_family = PF_INET;
    clnt_addr.sin_addr.s_addr = inet_addr(argv[1]);
    clnt_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&clnt_addr, sizeof(sockaddr)) == -1)
        error_handling((char *)"connect() error");
    else 
        succ((char *)"connect()");

    fputs("请输入数的个数: ",stdout);
    int x,i;
    scanf("%d",&x);
    op_info[0] = (char)x;
    for(i = 0; i < x; i++){
        printf("请输入第%d个数: ",i+1);
        scanf("%d", (int* )(op_info+i*OP_SIZE+1)); // 记得+1!!!,(op_info+i*OP_SIZE)需要打括号
    }
    printf("请输入计算符号: ");
    scanf("\n%c",op_info+x*OP_SIZE+1);
    printf("op is : %c\n",op_info[x*OP_SIZE+1]);
    op_info[x*OP_SIZE+2] = 0;
    write(sock, op_info, sizeof(op_info));
    read(sock, &result, sizeof(result));
    printf("the result is %d\n",result);
    close(sock);
}

void error_handling(char *message){
    fprintf(stdout, "!!!------%s\n",message);
    exit(1);
}

void succ(char *message){
    fprintf(stdout, "@@@------%s successed\n",message);
}
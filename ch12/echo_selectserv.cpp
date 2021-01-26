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

#define BUF_SIZE 1024

void error_handling(char*); //打印错误信息
void succ(char*); //打印成功信息
void sock_init(int* , sockaddr_in* , int); //初始化套接字
int main(int argc, char **argv){
    
    if (argc != 2){
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t sock_len = sizeof(sockaddr);
    char buf[BUF_SIZE];
    sock_init(&serv_sock, &serv_addr, atoi(argv[1]));

    int fd_max = serv_sock+1;
    fd_set cntl_fd, tp_cntl_fd;
    timeval time_out;
    int status;
    FD_ZERO(&cntl_fd);
    FD_SET(serv_sock, &cntl_fd);

    while(1){
        tp_cntl_fd = cntl_fd;
        time_out.tv_sec = 10;
        time_out.tv_usec = 0;
        
        status = select(fd_max, &tp_cntl_fd, 0, 0, &time_out);
        if (status == -1){
            error_handling((char*)"select()");
        }else if (status == 0){
            printf("time out!!!\n");
        }else{
            int i;
            for(i = 0;i < fd_max; i++){
                if (!FD_ISSET(i, &tp_cntl_fd))
                    continue;
                if (i == serv_sock){
                    clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &sock_len);
                    printf("--clnt %d--",clnt_sock);
                    if (clnt_sock == -1)
                        error_handling((char*)"accept()");
                    else    
                        succ((char*)"accept()");
                    FD_SET(clnt_sock, &cntl_fd);
                    if (clnt_sock+1 > fd_max) fd_max = clnt_sock+1;
                }else{
                    int read_len;
                    read_len = read(i, buf, BUF_SIZE);
                    if ( read_len == -1){
                        printf("--clnt %d-- closed\n",i);
                        close(i);
                    }else{
                        buf[read_len] = 0;
                        write(i, buf, read_len);
                        printf("sock %d message: %s\n",i, buf);
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
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
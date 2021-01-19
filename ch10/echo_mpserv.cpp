#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>

#define BUF_SIZE 1024

void error_handling(char *message);
void succ(char *message);
void hand_childproc(int sig);
void echo(int sock);
int main(int argc, char** argv){
    if (argc != 2){
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }
    socklen_t sock_len = sizeof(sockaddr_in);
    sockaddr_in serv_addr,clnt_addr;
    int sock, clnt_sock;
    struct sigaction act;
    act.__sigaction_u.__sa_handler = hand_childproc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD,&act,0);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling((char*)"socket");
    else 
        succ((char*)"socket()");
    
    memset(&serv_addr,0,sock_len);
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if (bind(sock, (struct sockaddr*)&serv_addr,sock_len) == -1)
        error_handling((char*)"bind()");
    else 
        succ((char*)"bind()");
    
    if (listen(sock, 5) == -1)
        error_handling((char*)"listen()");
    else 
        succ((char*)"listen()");
    
    // pid_t main_pid;
    // main_pid = fork();
    while(1){
        while((clnt_sock = accept(sock, (struct sockaddr*)&clnt_sock, &sock_len)) == -1); //这个问题我暂时不知道怎么解决
        if ( clnt_sock == -1)
            error_handling((char*)"accept()");
        else 
            succ((char*)"accept()");
        
        pid_t cpid = fork();
        if (cpid < 0){
            error_handling((char*)"fork()");
        }else if (cpid == 0){
            close(sock);
            echo(clnt_sock);
            close(clnt_sock); 
            exit(0);
        }else{
            close(clnt_sock);
            printf("Child: %d , socket: %d\n",cpid, clnt_sock);
        }       
    }
    close(sock);

}

void error_handling(char *message){
    fprintf(stderr, "%d %s error\n",getpid(),message);
    exit(1);
}
void succ(char *message){
    fprintf(stdout,"%d %s successed\n",getpid(),message);
}
void hand_childproc(int sig){
    pid_t pid;
    int status;
    pid = waitpid(-1,&status,WNOHANG);
    if (WIFEXITED(status))
        printf("%d Child send %d\n",pid,WEXITSTATUS(status));

}

void echo(int sock){
    char message[BUF_SIZE];
    int str_len;
    while((str_len = read(sock, message, BUF_SIZE)) > 0){
        write(sock, message, str_len);
    }
}
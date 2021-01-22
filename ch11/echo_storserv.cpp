#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
/*
 * 简单的回声服务器, 专门开一个进程存储客户端传来的信息到文件, 通过管道进行通信.
 */

void error_handling(char *message);
void succ(char *message);
void handle_childproc(int);
void record_to_file(int*);
void handle_accept(int, int*);

int main(int argc, char** argv){
    if (argc != 2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    int serv_sock;
    struct sockaddr_in serv_addr;
    int fds[2];
    struct sigaction act;

    act.__sigaction_u.__sa_handler = handle_childproc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serv_sock, 5);

    int fd[2];
    pipe(fd); //管道如何关闭
    pid_t pid = fork();
    if (pid == 0){
        //printf("child\n");
        close(serv_sock);
        record_to_file(fd);
    }
    else if (pid > 0){
        handle_accept(serv_sock, fd);
    }

}

void error_handling(char *message){
    fprintf(stderr, "%s error\n",message);
    exit(1);
}
void succ(char *message){
    fprintf(stdout,"%s successed\n",message);
}

void handle_childproc(int sig){
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)){
        printf("pid %d exit with %d", pid, WEXITSTATUS(status));
    }
    return;
}

void record_to_file(int fd[2]){
    char buf[BUFSIZ];
    FILE *fp = fopen("echomsg.txt","w");
    int read_len;
    while(1){
        read_len = read(fd[0], buf, BUFSIZ);
        if (read_len == 1 && buf[0] == 'q')
            break;
        buf[read_len] = 0;
        printf("read_len: %d %s\n",read_len,buf);
        fwrite((void*)buf, 1, read_len, fp);
    }
    fclose(fp);
    return;
}

void handle_accept(int serv_sock, int fd[2]){
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t sock_len;
    char buf[BUFSIZ];
    int read_len;
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &sock_len);
    while((read_len = read(clnt_sock, buf, BUFSIZ)) > 0){
        buf[read_len] = 0;
        write(clnt_sock, buf, read_len);
        write(fd[1], buf, read_len);
    }
    write(fd[1],"q",1);
    close(clnt_sock);
    return;
}


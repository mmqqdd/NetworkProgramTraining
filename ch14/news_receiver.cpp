#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
/*
 * 这个代码有个问题, 输出会重复一遍, 确实只发送了一遍
 */
#define BUF_SIZE 1024
void error_handling(char *message);
void succ(char *message);

int main(int argc, char **argv){
    int recv_sock;
    int read_len;
    char buf[BUF_SIZE];
    struct sockaddr_in recv_addr;
    struct ip_mreq join_addr;

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = PF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[2]));
    bind(recv_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr));
    
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_addr, sizeof(join_addr));

    while(1){
        read_len = recvfrom(recv_sock, buf, BUF_SIZE, 0, NULL, 0);
        //if (read_len == 0) continue;
        if (read_len == -1)
            break;
        //buf[read_len] = 0;
        fprintf(stdout, "%d-r-%d %s\n",read_len,strlen(buf),buf);
        //sleep(2);
    }
    close(recv_sock);
}

void error_handling(char *message){
    fprintf(stderr, "%d %s error\n",getpid(),message);
    exit(1);
}
void succ(char *message){
    fprintf(stdout,"%d %s successed\n",getpid(),message);
}
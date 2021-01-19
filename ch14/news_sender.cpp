#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define TTL 64
#define BUF_SIZE 1024
void error_handling(char *message);
void succ(char *message);

int main(int argc, char **argv){
    int send_sock;
    int time_live = TTL;
    char buf[BUF_SIZE];
    struct sockaddr_in mul_addr;
    FILE *fp = fopen("news_sender.cpp","r");
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, &time_live, sizeof(time_live));
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family = PF_INET;
    mul_addr.sin_addr.s_addr = inet_addr(argv[1]);
    mul_addr.sin_port = htons(atoi(argv[2]));
    while(!feof(fp)){
        fgets(buf, BUF_SIZE-1, fp);
        sendto(send_sock, buf, BUF_SIZE, 0, (struct sockaddr*)&mul_addr, sizeof(mul_addr));
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *message){
    fprintf(stderr, "%d %s error\n",getpid(),message);
    exit(1);
}
void succ(char *message){
    fprintf(stdout,"%d %s successed\n",getpid(),message);
}
/////最后一行为什么没有发送出去
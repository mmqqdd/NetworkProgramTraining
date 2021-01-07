#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void succ(char *message);

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_sz;

    if (argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
    }

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling((char*)"socket() error");
    else 
        succ((char*)"socket");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
        error_handling((char*)"bind() error");
    else
        succ((char*)"bind");
    if (listen(serv_sock,5) == -1)
        error_handling((char*)"listen() error");
    else succ((char*)"listen");

    clnt_addr_sz = sizeof(clnt_addr);
    int i,str_len;
    for(i=0; i<5; i++){
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
        if (clnt_sock == -1)
            error_handling((char*)"accept() error");
        else{
            succ((char*)"accept()");
            printf("--client--\n ID: %d\n sock: %d\n\n",i+1,clnt_sock);
        }
        while((str_len = read(clnt_sock,message,BUF_SIZE))!=0){
            write(clnt_sock,message,str_len);
            //write(clnt_sock,message,str_len/2);
            //write(clnt_sock,message+str_len/2,(str_len+1)/2);
            printf("message from client: %s",message);
        }
        close(clnt_sock);
    }
    close(serv_sock);

}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

void succ(char *message){
    fputs((char*)"--------------",stdout);
    fputs(message,stdout);
    fputs((char*)" successed!\n",stdout);
}


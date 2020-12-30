#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#define BUF_SIZE 100
void error_handling(char *message);
/*
 * 从文件读入
 */
int main(){
    int fd;
    char buf[BUF_SIZE];
    fd=open("data.txt",O_RDONLY);
    if (fd==-1)
        error_handling((char*)"open() error!");
    printf("文件描述符: %d\n",fd);
    if (read(fd,buf,sizeof(buf))==-1)
        error_handling((char*)"read() error!");
    printf("file data: %s",buf);
    close(fd);
    return 0;
}
void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
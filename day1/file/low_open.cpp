#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
void error_handling(char* message);
/*
 * 向文件写入
 */
int main(){
    int fd;
    char buf[]="Let's go!\n";
    fd=open("data.txt",O_CREAT|O_WRONLY|O_TRUNC);
    printf("文件描述符: %d\n",fd);
    if (fd==-1)
        error_handling((char*)"open() error!");
    if (write(fd,buf,sizeof(buf))==-1)
        error_handling((char*)"write() error!");
    close(fd);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
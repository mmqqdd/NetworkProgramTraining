#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#define BUF_SIZE 1024
/*
 * 这个程序简单的测试一下pipe, 为了简化,没有对错误进行处理.
 */
int main(){
    int fds[2];
    char buf[BUF_SIZE];
    char str1[] = "Hello, I am mqd";
    char str2[] = "Hi, I am Jack";
    pipe(fds);
    pid_t pid = fork();

    if (pid == 0){
        write(fds[1],str1,sizeof(str1));
        sleep(2);
        read(fds[0],buf,BUF_SIZE);
        printf("child buf is: %s\n",buf);
    }else{
        read(fds[0],buf,BUF_SIZE);
        printf("parent buf is: %s\n",buf);
        write(fds[1],str2,sizeof(str2));
    }
    return 0;
    
}
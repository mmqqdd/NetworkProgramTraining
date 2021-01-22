#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>

int main(){
    char buf[BUFSIZ];
    fd_set reads,tmps;
    struct timeval time_out;
    FD_ZERO(&reads);
    FD_SET(STDIN_FILENO, &reads);
    while(1){
        tmps = reads;
        time_out.tv_sec = 2;
        time_out.tv_usec = 1;
        int sig = select(1, &tmps,0, 0, &time_out);
        if (sig == -1){
            printf("select error()\n");
            exit(1);
        }else if (sig == 0){
            printf("time out!!!\n");
        }else{
            if (FD_ISSET(STDIN_FILENO, &tmps)){
                scanf("%s",buf);
                //read(STDIN_FILENO, buf, BUFSIZ);
                //fgets(buf,BUFSIZ, STDIN_FILENO);
                //gets(buf);
                printf("--%s\n",buf);
            }
            
        }
        
    }
}
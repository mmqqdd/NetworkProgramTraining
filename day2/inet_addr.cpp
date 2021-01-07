#include<stdio.h>
#include<arpa/inet.h>

int main(){
    char *addr1=(char *)"192.168.1.1";

    char *addr2=(char *)"192.256.1.1";

    unsigned long conv_addr=inet_addr(addr1);
    if (conv_addr == INADDR_NONE)
        printf("Error occured in %s!\n",addr1);
    else 
        printf("%s => %#x\n",addr1,conv_addr);
    
    conv_addr=inet_addr(addr2);
    if (conv_addr == INADDR_NONE)
        printf("Error occured in %s!\n",addr2);
    else 
        printf("%s => %#x\n",addr2,conv_addr);

}
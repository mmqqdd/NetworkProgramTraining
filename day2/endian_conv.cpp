#include<stdio.h>
#include<arpa/inet.h>
int main(){
    unsigned short host_port=0x1234; //2字节
    unsigned short net_port;
    unsigned long host_addr=0x12345678; //4字节
    unsigned long net_addr;
    net_port = htons(host_port);
    net_addr = htonl(host_addr);
    printf("host_port: %#x \n",host_port);
    printf("host_port: %#x \n",net_port);
    printf("host_port: %#x \n",host_addr);
    printf("host_port: %#x \n",net_addr);
}
#include<cstdio>

/*
 * 随便写的几个函数指针, 巩固记忆
 */
void echo(char c){
    printf("char is: %c",c);
}
void (*test(int x))(char){ //这是一个普通函数, 返回的是一个函数指针
    printf("test num is %d\n",x);
    return echo;
}
int main(){
    void (*(*p1)(int))(char) = test; //这是一个函数指针, 返回的也是一个函数指针, 所以有两个*
    void (*p2)(char) = p1(233);  
    p2('a');
    char ss[100];
    for(int i=1;i<=10;i++){
        scanf("%s%c",ss);
        printf("%s\n",ss);
    }
}
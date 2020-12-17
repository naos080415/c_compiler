#include <stdio.h>
#include <stdlib.h>
int foo()
{
    printf("foo OK\n");
}

int bar(int x,int y)
{
    printf("2つの引数の合計:%d\n",x+y);
    return x+y;
}

int buz(int x,int y,int z,int w)
{
    printf("%d+%d+%d+%d = %d\n",x,y,z,w,x+y+z+w);
}

int abi(int x,int y,int z,int w,int t,int u)
{
    printf("%d+%d+%d+%d+%d+%d= %d\n",x,y,z,w,t,u,x+y+z+w+t+u);
}

void alloc4(int *p,int x,int y,int z,int w)
{
    p = malloc(sizeof(int)*4);
    *(p+0) = x;
    *(p+1) = y;
    *(p+2) = z;
    *(p+3) = w;
}

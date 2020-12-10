#include <stdio.h>
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
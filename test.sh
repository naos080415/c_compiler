#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s func.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# これから頑張るやつ(配列やポインタを返す部分)
# assert 3 "int main(){
# int x[10];
# x[3] = 3;
# return x[3];}"

# assert 5 "int main(){
# int x[10];
# *x = 5;
# return *x;}"

# assert 3 "int main(){
# int a[2];
# *a = 1;
# *(a + 1) = 2;
# int *p;
# p = a;
# return *p + *(p + 1);}"

#演算子
assert 0 "int main(){return 0;}"
assert 42 "int main(){return 42;}"
assert 21 "int main(){ return 5 +20-4;}"
assert 41 "int main(){ return 12 + 34 - 5;}"
assert 47 "int main(){ return 5+6*7;}"
assert 15 "int main(){ return 5*(9-6);}"
assert 4 "int main(){ return (3+5)/2;}"
assert 10 "int main(){ return -10+20;}"
assert 0 "int main(){ return 0==1;}"
assert 1 "int main(){ return 42==42;}"
assert 1 "int main(){ return 0!=1;}"
assert 0 "int main(){ return 42!=42;}"
assert 1 "int main(){ return 0<1;}"
assert 0 "int main(){ return 1<1;}"
assert 0 "int main(){ return 2<1;}"
assert 1 "int main(){ return 0<=1;}"
assert 1 "int main(){ return 1<=1;}"
assert 0 "int main(){ return 2<=1;}"
assert 1 "int main(){ return 1>0;}"
assert 0 "int main(){ return 1>1;}"
assert 0 "int main(){ return 1>2;}"
assert 1 "int main(){ return 1>=0;}"
assert 1 "int main(){ return 1>=1;}"
assert 0 "int main(){ return 1>=2;}"

assert 14 "int main(){ 
int foo;
int var;
foo = 3;
var = 5 * 6 - 8;
return foo + var / 2;}"

assert 3 "int main(){
int a;
a = 3;
if( a == 3 ) 
  return a;
else
  return 0;}"

assert 3 "int main(){
int a;
a = 0;
while( a != 3 ) 
  a = a + 1;
return a;
}"

assert 5 "int main(){
int a;
int b;
for(a=0;a<5;a=a+1) b = 0;
return a;
}"

assert 5 "int main(){
int a;int b;int c;
for(a=0;a<5;a=a+1){b=0;c=0;}
return a;}"
assert 10 "int main(){
int a;
a=0;
for(;a<5;)a=a+1;
return 10;}"

assert 10 "int main(){
int a;int b;
b = 0;for(a=0;a<5;a=a+1){if(a==3)b=10;}
return b;}"

#　関数の読み出し
assert 128 "int main(){foo();return 128;}"
assert 128 "int main(){bar(1,2);return 128;}"
assert 128 "int main(){buz(1,2,3,4);return 128;}"
assert 128 "int main(){int *p;buzp(p,1,2,3,4);return 128;}"

assert 128 "int main(){abi(1,2,3,4,5,6);return 128;}"
# 演算子(&,*)
assert 3 "int main(){
int x;
int y;
x = 3;
y = &x;
return *y;}"
assert 3 "int main(){
int x;int y;int z;
x = 3;
y = 5;
z = &y + 8;
return *z;}"

# ポインタ
assert 3 "int main(){
int x;
int *y;
y = &x;
*y = 3;
return x;}"

assert 3 "int main(){
int x;
int *y;
y = &x;
*y = 3;
return *y;}"
assert 4 "int main(){
int *p;
int *q;
allocs(&p, 1, 2, 4, 8);
q = p + 2;
return *q;}"

# sizeof演算子
assert 4 "int main(){
int x;
int y;
y=sizeof(x);
return y;}"

assert 8 "int main(){
int *x;
int y;
y=sizeof(x);
return y;}"

assert 4 "int main(){
int x;
x=sizeof(1);
return x;}"

echo OK
 
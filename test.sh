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

assert 0 "main(){return 0;}"
assert 42 "main(){return 42;}"
assert 21 "main(){ return 5 +20-4;}"
assert 41 "main(){ return 12 + 34 - 5;}"
assert 47 "main(){ return 5+6*7;}"
assert 15 "main(){ return 5*(9-6);}"
assert 4 "main(){ return (3+5)/2;}"
assert 10 "main(){ return -10+20;}"
assert 0 "main(){ return 0==1;}"
assert 1 "main(){ return 42==42;}"
assert 1 "main(){ return 0!=1;}"
assert 0 "main(){ return 42!=42;}"
assert 1 "main(){ return 0<1;}"
assert 0 "main(){ return 1<1;}"
assert 0 "main(){ return 2<1;}"
assert 1 "main(){ return 0<=1;}"
assert 1 "main(){ return 1<=1;}"
assert 0 "main(){ return 2<=1;}"
assert 1 "main(){ return 1>0;}"
assert 0 "main(){ return 1>1;}"
assert 0 "main(){ return 1>2;}"
assert 1 "main(){ return 1>=0;}"
assert 1 "main(){ return 1>=1;}"
assert 0 "main(){ return 1>=2;}"
assert 14 "main(){a = 3;
b = 5 * 6 - 8;
return a + b / 2;}
"
assert 14 "main(){ foo = 3;
var = 5 * 6 - 8;
return foo + var / 2;}
"
assert 14 "main(){a = 3;
b = 5 * 6 - 8;
return a + b / 2;}
"

assert 3 "main(){a = 3;
if( a == 3 ) return a;
return 5;}
"
assert 3 "main(){a = 0;
while( a != 3 ) a = a + 1;
return a;}
"
assert 3 "main(){a = 3;
if( a == 3 ) return 3;
else return 0;}
"
assert 0 "main(){a = 5;
if( a == 3 ) return 3;
else return 0;}
"
assert 5 "main(){for(a=0;a<5;a=a+1) b = 0;
return a;}
"
assert 5 "main(){for(a=0;a<5;a=a+1){b=0;c=0;}
return a;}
"

assert 10 "main(){a=0;for(;a<5;)a=a+1;
return 10;}"

assert 5 "main(){b = 0;for(a=0;a<5;a=a+1){b=0;c=0;d=0;e=0;}
return 5;}
"
assert 0 "main(){b = 0;for(a=0;a<5;a=a+1){b=0;}
return b;}
"

assert 10 "main(){b = 0;for(a=0;a<5;a=a+1){if(a==3)b=10;}
return b;}
"

assert 128 "main(){foo();return 128;}"
assert 128 "main(){bar(1,2);return 128;}"
assert 128 "main(){buz(1,2,3,4);return 128;}"


echo OK

#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5 +20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'
assert 14 "a = 3;
b = 5 * 6 - 8;
a + b / 2;
"
assert 14 "foo = 3;
var = 5 * 6 - 8;
foo + var / 2;
"
assert 14 "a = 3;
b = 5 * 6 - 8;
return a + b / 2;
"
assert 3 "a = 3;
if( a == 3 ) return a;
return 5;
"
assert 3 "a = 0;
while( a != 3 ) a = a + 1;
return a;
"
assert 3 "a = 3;
if( a == 3 ) return 3;
else return 0;
"
assert 0 "a = 5;
if( a == 3 ) return 3;
else return 0;
"
assert 5 "for(a=0;a<5;a=a+1) b = 0;
return a;
"
assert 5 "for(a=0;a<5;a=a+1){b=0;c=0;}
return a;
"
assert 5 "b = 0;for(a=0;a<5;a=a+1){a=0;b = 3;c=5;d=8;}
return 5;
"
assert 0 "b = 0;for(a=0;a<5;a=a+1){b=0;}
return b;
"

assert 10 "b = 0;for(a=0;a<5;a=a+1){if(a==3)b=10;}
return b;
"
echo OK

#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" == "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 1 '1;'
assert 42 '42 ;'

assert 41 '12 + 34 - 5 ;'
assert 47 '5+6*7 ;'

assert 4 '(3+5) / 2 ;'
assert 7 '(9 - 7) * (2 *3 - 2) / 2 + 3 ;'

assert 5 '-10 + 15 ;'
assert 5 '- -5 ;'
assert 5 '- - +5 ;'

assert 0 '0 == 1 ;'
assert 1 '42 == 42 ;'
assert 1 '0 != 1 ;'
assert 0 '42 != 42 ;'

assert 1 '1 < 2 ;'
assert 0 '2 < 1 ;'
assert 0 '1 < 1 ;'
assert 1 '1 <= 2 ;'
assert 0 '2 <= 1 ;'
assert 1 '1 <= 1 ;'

assert 1 '2 > 1 ;'
assert 0 '1 > 2 ;'
assert 0 '1 > 1 ;'
assert 0 '1 >= 2 ;'
assert 1 '1 >= 0 ;'
assert 1 '1 >= 1 ;'

assert 13 'a = 2; b = 1; c = a + b; d = c + 10 ;'
assert 3 'foo = 2; bar = 1; hoge = foo + bar;'
assert 15 'opuy0sh1 = 10; c0mpiler = 5; sum_resu1t = opuy0sh1 + c0mpiler;'

assert 5 'return 5;'
assert 10 'return 10; return 5;'
assert 14 'a = 5; b = 9; return c = a + b ; return d = c + 5;'

echo OK

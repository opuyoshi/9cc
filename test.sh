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

echo OK

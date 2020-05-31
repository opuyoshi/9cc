#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" == "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 41 "12 + 34 - 5"
assert 47 "5+6*7"
assert 4 "(3+5) / 2"
assert 7 "(9 - 7) * (2 *3 - 2) / 2 + 3"

echo OK

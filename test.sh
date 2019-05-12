#!/bin/sh

try() {
    expected="${1}"
    input="${2}"

    ./9cc "${input}" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual=$?

    if [ "${expected}" = "${actual}" ] ; then
        echo "${input} => ${actual}"
    else
        echo "${expected} expected, but got ${actual}"
        exit 1
    fi
}

try 0 0
try 42 42
try 13 '2+11'
try 21 '5+20-4'
try 21 '5 + 20 - 4'
try 41 ' 12 + 34 - 5 '
try 7  '1+2*3'
try 9  '(1+2)*3'
try 17 '10 / 2 + 4 * 3'
try 5  '-1+2*3'
try 9  '-(1+2)*-3'
try 9  '(-1 + -2) * -3'

echo OK

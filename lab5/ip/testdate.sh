#!/bin/sh

while read line
do
    echo $line | 
    grep -E "^(0[1-9]|[1-2][0-9]|3[0-1])\-(0[1-9]|1[0-2])\-[0-9]{4}$"
done
#!/bin/sh

while read line
do
    echo $line | 
    grep -E "^([0-9]{1,3}\.){3}[0-9]{1,3}$" | 
    grep -E -v "2[6-9][0-9]" |
    grep -E -v "25[6-9]" |
    grep -E -v "[3-9][0-9][0-9]"
done
#!/bin/sh

if test "$#" -lt 1
then
    echo "Usage: $0 <character>\n"
else
    count=0
    while read line
    do
        res=`echo $line | grep -E "$1" | grep -E "^[A-Z][A-Za-z\ ,]*[.!?]$" | grep -E -v "si[ ]*," | grep -E -v "[n][pb]"`
        if [ ! -z "$res" ]
        then
            count=`expr $count + 1`
        fi
    done
    echo $count
fi
#!/bin/sh

while read line
do
    echo $line | grep -E "^[A-Z][A-Za-z0-9\ ,]*\.$" | grep -E -v "si[ ]*," | grep -E -v "[n][pb]"
done
#!/bin/sh

if test "$#" -lt 2
then
    echo "Usage: <script_name> <directory> <output_file>\n"
else
    count=0
    count2=0
    for entry in $1/*
    do
        if [ -f $entry ]
        then
            res=`cat $entry | grep -E "^[A-Z][ ]?[a-zA-Z\ ,]*\.$" | grep -E -v "(si[ ]*,)|(si[ ][ ]+)" | grep -E -v "n[bp]"`
            if [ ! -z "$res" ]
            then
                echo $entry >> $2
            fi
        elif [ -L $entry ]
        then
            count=`expr $count + 1`
        elif [ -d $entry ]
        then
            count2=`bash $0 $entry $2`
            count=`expr $count + $count2`
        fi
    done
    echo $count
fi
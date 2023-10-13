#!/bin/sh

# set_mode.sh <directory> <mode>
# nerecursiv si recursiv

if [ $# -ne 2 ]
then
    echo "Error! Usage: <script_name> <directory> <mode>."
else

        if [ -d $1 ]
        then 

            for entry in $1/*
            do
                if [ -d $entry ]
                then
                    bash $0 $entry $2
                fi
            done

            cd $1

            for line in $(ls *.txt | cut -f 9 -d ' ') # <=> (ls *.txt | cat) // displays each file on a new line
            do
                if [ -f $line ]
                then 
                    chmod "+$2" "$line"
                else 
                    echo "Script failed for $line, not a regular file."
                fi
            done

        else
            echo "Error: $1 is not a directory or doesn't exist."
            fi
fi


#!/bin/sh

# set_mode.sh <directory> <mode>
# nerecursiv si recursiv

if [ $# -ne 2 ]
then
    echo "Error! Usage: <script_name> <directory> <output_file>."
else

        if [ -d $1 ]
        then 
            cd $1
            total_chars=0
            for line in $(ls *.txt | cut -f 9 -d ' ') # <=> (ls *.txt | cat) // displays each file on a new line
            do
                if [ -f $line ]
                then 
                    for chars in $(wc -c $line | cut -f 1 -d ' ')
                    do
                        echo "$line $chars"
                        total_chars=`expr $total_chars + $chars`
                    done
                else 
                    echo "Script failed for $line, not a regular file."
                fi
            done
            echo "TOTAL $total_chars"
        else
            echo "Error: $1 is not a directory or doesn't exist."
            fi
fi


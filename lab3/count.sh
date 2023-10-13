#!/bin/sh

# count.sh <directory> <output_file>

if [ $# -ne 2 ]
then
    echo "Error! Usage: <script_name> <directory> <output_file>."
else
        directory=$1
        output_file=$2

        if [ -f $output_file ]
        then rm $output_file
        fi
        
        if [ -d $directory ]
        then 
            cd $directory
            total_chars=0
            for line in $(ls *.txt | cut -f 9 -d ' ') # <=> (ls *.txt | cat) // displays each file on a new line
            do
                if [ -f $line ]
                then 
                    for chars in $(wc -c $line | cut -f 1 -d ' ')
                    do
                        echo "$line $chars" >> $output_file
                        total_chars=`expr $total_chars + $chars`
                    done
                else 
                    echo "Script failed for $line, not a regular file."
                fi
            done
            echo "TOTAL $total_chars" >> $output_file
            mv $output_file "../$output_file"
        else
            echo "Error: $1 is not a directory or doesn't exist."
            fi
fi


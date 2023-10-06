#!/bin/sh

#scriptul se execută începând de aici
if [ $# -lt 3 ]
then
      echo "Usage: <scriptname> <numefisier> <dir> n1 n2 n3."
else
      count=0
      sum=0
      fisier=$2
      director=$3
      shift 3
      for number in $@
      do
            sum=`expr $sum + $number`
            if [ $number -gt 10 ]
            then count=`expr $count + 1`
            fi
      done
      echo "Numbers greater than 10: $count"
      echo "The sum of each argument: $sum"
      echo "Characters for sum: $( echo $sum | wc -L )"
      if [ -f $fisier ]
      then echo "$count $sum" > out.txt
      fi
      
      cd $director
      for linie in $(ls *.txt | cat)
      do
            cat $linie
            echo ""
      done
      cd ..

fi

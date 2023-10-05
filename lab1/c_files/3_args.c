#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int isnumber(char *number)
{
    int i = 0;
    int boolean = 1;
    while(number[i] != 0)
    {
        boolean = boolean && isdigit(number[i]);
        i++;
    }
    return boolean;
}

int main (int argc, char**argv)
{
    if(argc != 4)
    {
        printf("ERROR: Not 3 arguments.\nUsage: ./<program_name> <arg1> <arg2> <arg3>.\n");
        return -1;
    }

    int sum = 0;

    for(int i = 1; i < 4; i++) //starting from 1 (excluding program name)
    {
        if(isnumber(argv[i]))
        {
            int integer = atoi(argv[i]); //string needs converted to int
            sum = sum + integer;
        }
        else
        {
            printf("The argument \"%s\" is not a number.\n", argv[i]);
        }
    }
    
    printf("The sum is %d.\n", sum);

    return 0;
}
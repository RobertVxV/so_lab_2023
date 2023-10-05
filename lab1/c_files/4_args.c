#include <stdio.h>

int main (int argc, char**argv)
{
    if(argc != 5)
    {
        printf("ERROR: Not 4 arguments.\nUsage: ./<program_name> <arg1> <arg2> <arg3> <arg4>.\n");
        return -1;
    }

    printf("arg1: %s\n", argv[1]);
    printf("arg2: %s\n", argv[2]);
    printf("arg3: %s\n", argv[3]);
    printf("arg4: %s\n", argv[4]);

    return 0;
}
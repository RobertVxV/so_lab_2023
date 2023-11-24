#include <stdio.h>
#include <string.h>

int main(void)
{
    char student[6][50];
    strncpy(student[0], "ana maria pizzeria", 50);
    strncpy(student[1], "Cinderela", 50);
    strncpy(student[2], "Scoica", 50);
    strncpy(student[3], "Pandispan", 50);
    strncpy(student[4], "Langos", 50);
    strncpy(student[5], "Burger", 50);

    printf("studenti anul 3: ");
    for(int i = 0; i < 6; i++)
    {
        printf("%s\n", student[i]);
    }

    return 0;
}
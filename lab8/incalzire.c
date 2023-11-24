// PROBLEMA PROCESE

// Sa se scrie un program avand un proces parinte si un proces copil ruland in paralel, 
// procesul parinte citeste de la tastatura o data la 500 ms, 
// pana la terminarea copilului cate o litera, iar procesul copil ruleaza 
// executabilul unui fisier c "studenti.c" ce afiseaza studentii din anul 3 dintr-o anumita grupa. 
// Programul c este compilat in prealabil si primeste ca parametru grupa pentru care se doreste afisarea. 

#include <stdio.h>
#include <unistd.h>

int main (void)
{
    int pid = fork();
    if(pid == 0)
    {
        exec("./studenti.exe");
        printf("eroare");
    }
    int wstatus = 0;
    char c;
    for(int i = 0; i < 10; i++)
    {
        scanf("%c", &c);
        sleep(0.5);
    }
    wait(&wstatus);
    WIFEXITED(wstatus);
}
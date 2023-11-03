#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

int getchars(char *string)
{
    int chars = 0;
    int i = 0;
    while (string[i] != 0)
    {
        chars++;
        i++;
    }
    return chars;
}

int getFileSize(int fd, struct stat *st)
{
    if (fstat(fd, st) == -1)
    {
        return -1;
    }
    return st->st_size;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage error: %s <fisier_intrare> <fisier_statistica> <ch>\n", argv[0]);
        exit(-1);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1)
    {
        printf("Eroare la deschiderea fisierului de citire.\n");
        exit(-1);
    }

    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        printf("Eroare la deschiderea fisierului de scriere.\n");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    int litere_mari = 0;
    int litere_mici = 0;
    int cifre = 0;
    int aparitii_caracter = 0;

    struct stat st;
    int file_size = getFileSize(input_fd, &st);

    int reads = 0;

    while ((reads = read(input_fd, buffer, BUFFER_SIZE)) != 0)
    {
        for (int i = 0; i < reads; i++)
        {
            if (isupper(buffer[i]))
                litere_mari++;

            if (islower(buffer[i]))
                litere_mici++;

            if (isdigit(buffer[i]))
                cifre++;

            char ch = argv[3][0];
            if (buffer[i] == ch)
                aparitii_caracter++;
        }
    }

    char string[50];
    sprintf(string, "numar litere mici: %d\n", litere_mici);
    getchars(string);
    write(output_fd, string, getchars(string));
    sprintf(string, "numar litere mari: %d\n", litere_mari);
    write(output_fd, string, getchars(string));
    sprintf(string, "numar cifre: %d\n", cifre);
    write(output_fd, string, getchars(string));
    sprintf(string, "numar aparitii caracter: %d\n", aparitii_caracter);
    write(output_fd, string, getchars(string));
    sprintf(string, "dimensiune fisier: %d\n", file_size);
    write(output_fd, string, getchars(string));

    if (close(input_fd) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
    if (close(output_fd) != 0)
    {
        printf("Eroare la inchiderea fisierului de scriere.\n");
    }

    printf("Hello world!\n");
    return 0;
}
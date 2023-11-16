#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22

int getChars(char *string)
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

int getFileUID(int fd)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        return -1;
    }
    return st.st_uid;
}

int getFileSize(int fd)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        return -1;
    }
    return st.st_size;
}

char *getFileLastModification(int fd, char *buffer)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        return "ERROR";
    }

    time_t tt = st.st_mtime;
    struct tm tm = *localtime(&tt);

    sprintf(buffer, "%d.%d.%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    return buffer;
}

int getFileNoOfHardLinks(int fd)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        return -1;
    }
    return st.st_nlink;
}

char *getUserRightsForFile(int fd, char *rights)
{
    struct stat st;

    if (fstat(fd, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRUSR) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWUSR) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXUSR) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getGroupRightsForFile(int fd, char *rights)
{
    struct stat st;

    if (fstat(fd, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRGRP) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWGRP) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXGRP) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getOthersRightsForFile(int fd, char *rights)
{
    struct stat st;

    if (fstat(fd, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IROTH) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWOTH) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXOTH) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

int getBMPWidthHeight(int fd, int *width, int *height)
{
    int res = lseek(fd, WIDTH_OFFSET, SEEK_SET);
    if (res == -1)
    {
        return -1;
    }

    if ((read(fd, width, 4) != 4))
    {
        return -1;
    }
    if ((read(fd, height, 4) != 4)) //HEIGHT_OFFSET 22
    {
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage error: %s <fisier_intrare>\n", argv[0]);
        exit(-1);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1)
    {
        printf("Eroare la deschiderea fisierului de citire.\n");
        exit(-1);
    }

    int output_fd = open("statistica_s6.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        printf("Eroare la deschiderea fisierului de scriere.\n");
        exit(-1);
    }

    char *file_name = argv[1];

    int file_size = getFileSize(input_fd);
    int file_uid = getFileUID(input_fd);

    char time_buffer[50];

    char *modified_time = getFileLastModification(input_fd, time_buffer);
    int number_of_links = getFileNoOfHardLinks(input_fd);

    char user_rights_buffer[4];
    char group_rights_buffer[4];
    char others_rights_buffer[4];

    char *user_rights = getUserRightsForFile(input_fd, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_fd, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_fd, others_rights_buffer);

    char temporary_string[50];

    int bmp_width = 0;
    int bmp_height = 0;
    
    int retValBMPWidthHeight = getBMPWidthHeight(input_fd, &bmp_width, &bmp_height);

    if(retValBMPWidthHeight!=0)
    {
        printf("An error has encountered while trying to get the width and height.");
    }

    sprintf(temporary_string, "nume fisier: %s\n", file_name);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "inaltime: %d\n", bmp_width);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "lungime: %d\n", bmp_height);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "dimensiune: %d\n", file_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "identificatorul utilizatorului: %d\n", file_uid);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "timpul ultimei modificari: %s\n", modified_time);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "contorul de legaturi: %d\n", number_of_links);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces user: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces grup: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces altii: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));

    if (close(input_fd) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
    if (close(output_fd) != 0)
    {
        printf("Eroare la inchiderea fisierului de scriere.\n");
    }

    return 0;

}
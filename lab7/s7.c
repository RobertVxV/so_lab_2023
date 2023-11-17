#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22

#define MAX_FILE_PATH 1024

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

int getFileUID(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return -1;
    }
    return st.st_uid;
}

char *getFileName(char *absolute_path)
{
    char *filename = strrchr(absolute_path, '/');
    if (filename == NULL)
    {
        return "ERROR";
    }
    return filename + 1; // only filename without '/'
}

int getFileSize(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return -1;
    }
    return st.st_size;
}

int getLinkSize(char *filename)
{
    struct stat st;
    if (lstat(filename, &st) == -1)
    {
        return -1;
    }
    return st.st_size;
}

char *getFileLastModification(char *filename, char *buffer)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return "ERROR";
    }

    time_t tt = st.st_mtime;
    struct tm tm = *localtime(&tt);

    sprintf(buffer, "%d.%d.%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    return buffer;
}

int getFileNoOfHardLinks(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return -1;
    }
    return st.st_nlink;
}

char *getUserRightsForFile(char *filename, char *rights)
{
    struct stat st;

    if (stat(filename, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRUSR) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWUSR) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXUSR) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getGroupRightsForFile(char *filename, char *rights)
{
    struct stat st;

    if (stat(filename, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRGRP) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWGRP) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXGRP) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getOthersRightsForFile(char *filename, char *rights)
{
    struct stat st;

    if (stat(filename, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IROTH) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWOTH) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXOTH) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getUserRightsForLink(char *filename, char *rights)
{
    struct stat st;

    if (lstat(filename, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRUSR) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWUSR) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXUSR) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getGroupRightsForLink(char *filename, char *rights)
{
    struct stat st;

    if (lstat(filename, &st) == -1)
    {
        return "ERROR";
    }

    rights[0] = ((st.st_mode & S_IRGRP) != 0) ? 'R' : '-';
    rights[1] = ((st.st_mode & S_IWGRP) != 0) ? 'W' : '-';
    rights[2] = ((st.st_mode & S_IXGRP) != 0) ? 'X' : '-';

    rights[3] = '\0';

    return rights;
}

char *getOthersRightsForLink(char *filename, char *rights)
{
    struct stat st;

    if (lstat(filename, &st) == -1)
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
    if ((read(fd, height, 4) != 4)) // HEIGHT_OFFSET 22
    {
        return -1;
    }

    return 0;
}

int isRegularFile(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return 0;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

int isDirectory(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return 0;
    }
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

int isSymLink(char *filename)
{
    struct stat st;
    if (lstat(filename, &st) == -1)
    {
        return 0;
    }
    return S_ISLNK(st.st_mode);
}

int hasExtension(char *filename, char *extension)
{
    char *res = strstr(filename, extension); // search position
    if (res == NULL)
    {
        return 0;
    }
    res = res + strlen(extension); // increase res with the number of elements of extension
    if (*res == '\0')
    {
        return 1; // if filename ends with extension return true
    }
    return 0;
}

void processBMPFile(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int file_uid = getFileUID(input_filename);

    char time_buffer[50];

    char *modified_time = getFileLastModification(input_filename, time_buffer);
    int number_of_links = getFileNoOfHardLinks(input_filename);

    char user_rights_buffer[4];
    char group_rights_buffer[4];
    char others_rights_buffer[4];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[50];

    int bmp_width = 0;
    int bmp_height = 0;

    int retValBMPWidthHeight = getBMPWidthHeight(file_descriptor, &bmp_width, &bmp_height);

    if (retValBMPWidthHeight != 0)
    {
        printf("An error has encountered while trying to get the width and height.");
    }

    sprintf(temporary_string, "nume fisier: %s\n", getFileName(input_filename));
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
    write(output_fd, "\n", 1);

    if (close(file_descriptor) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
}

void processRegularFile(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int file_uid = getFileUID(input_filename);

    char time_buffer[50];

    char *modified_time = getFileLastModification(input_filename, time_buffer);
    int number_of_links = getFileNoOfHardLinks(input_filename);

    char user_rights_buffer[4];
    char group_rights_buffer[4];
    char others_rights_buffer[4];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[50];

    sprintf(temporary_string, "nume fisier: %s\n", getFileName(input_filename));
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
    write(output_fd, "\n", 1);

    if (close(file_descriptor) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
}

void processDirectory(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s", input_filename);
        return;
    }

    int file_uid = getFileUID(input_filename);

    char user_rights_buffer[4];
    char group_rights_buffer[4];
    char others_rights_buffer[4];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[50];

    sprintf(temporary_string, "nume director: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "identificatorul utilizatorului: %d\n", file_uid);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces user: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces grup: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces altii: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    write(output_fd, "\n", 1);

    if (close(file_descriptor) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
}

void processSymLink(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int link_size = getLinkSize(input_filename);

    char user_rights_buffer[4];
    char group_rights_buffer[4];
    char others_rights_buffer[4];

    char *user_rights = getUserRightsForLink(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForLink(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForLink(input_filename, others_rights_buffer);

    char temporary_string[50];

    sprintf(temporary_string, "nume legatura: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "dimensiune legatura: %d\n", link_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "dimensiune fisier legatura: %d\n", file_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces user legatura: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces grup legatura: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    sprintf(temporary_string, "drepturi de acces altii legatura: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    write(output_fd, "\n", 1);

    if (close(file_descriptor) != 0)
    {
        printf("Eroare la inchiderea fisierului de citire.\n");
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage error: %s <director intrare>\n", argv[0]);
        exit(-1);
    }

    DIR *input_dir = opendir(argv[1]);
    if (input_dir == NULL)
    {
        perror(argv[1]);
        exit(-1);
    }

    int output_fd = open("statistica_s7.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        printf("Eroare la deschiderea fisierului de scriere.\n");
        exit(-1);
    }

    struct dirent *dir_entry = NULL;

    char absolute_path[MAX_FILE_PATH];

    while ((dir_entry = readdir(input_dir)) != NULL)
    {
        int size = strlen(argv[1]) + strlen(dir_entry->d_name) + 1;
        if (size > MAX_FILE_PATH - 1 - 1) // 1 for terminator, 1 for array-1
        {
            printf("File path too big for reading.");
            continue;
        }

        sprintf(absolute_path, "./%s%s", argv[1], dir_entry->d_name);

        if (isRegularFile(absolute_path))
        {
            if (isSymLink(absolute_path))
            {
                processSymLink(absolute_path, output_fd);
            }
            else if (hasExtension(absolute_path, ".bmp"))
            {
                processBMPFile(absolute_path, output_fd);
            }
            else
            {
                processRegularFile(absolute_path, output_fd);
            }
        }

        else if (isDirectory(absolute_path))
        {
            processDirectory(absolute_path, output_fd);
        }
    }

    if (close(output_fd) != 0)
    {
        printf("Eroare la inchiderea fisierului de scriere.\n");
    }

    return 0;
}
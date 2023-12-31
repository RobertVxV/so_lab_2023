#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22
#define BITCOUNT_OFFSET 28
#define DATA_OFFSET 10
#define COLORTABLE_OFFSET 54

#define MAX_FILE_PATH 1024
#define MAX_FILE_NAME 256
#define BUFFER 4098     // is divisible by 3 for bmp
#define TIME_BUFFER 11  // 2 for day, 2 for month, 4 for year, 1 for terminator and 2 dots for separation - 11
#define RIGHTS_BUFFER 4 // read write execute terminator

// gets string and counts every byte until terminator is met and returns the number of bytes
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

// gets file full path and return only the file name
char *getFileName(char *input_file_path)
{
    char *filename = strrchr(input_file_path, '/');
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

// returns a string of file st_mtime formatted to "day.month.year"
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

uint8_t isRegularFile(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return 0;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

uint8_t isDirectory(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        return 0;
    }
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

uint8_t isSymLink(char *filename)
{
    struct stat st;
    if (lstat(filename, &st) == -1)
    {
        return 0;
    }
    return S_ISLNK(st.st_mode);
}

uint8_t hasExtension(char *filename, char *extension)
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
        printf("Error opening file for reading: %s\n", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int file_uid = getFileUID(input_filename);

    char time_buffer[TIME_BUFFER];

    char *modified_time = getFileLastModification(input_filename, time_buffer);
    int number_of_links = getFileNoOfHardLinks(input_filename);

    char user_rights_buffer[RIGHTS_BUFFER];
    char group_rights_buffer[RIGHTS_BUFFER];
    char others_rights_buffer[RIGHTS_BUFFER];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[BUFFER];

    int bmp_width = 0;
    int bmp_height = 0;

    int retValBMPWidthHeight = getBMPWidthHeight(file_descriptor, &bmp_width, &bmp_height);

    if (retValBMPWidthHeight != 0)
    {
        printf("An error has encountered while trying to get the width and height.");
    }

    snprintf(temporary_string, sizeof(temporary_string), "nume fisier: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "inaltime: %d\n", bmp_width);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "lungime: %d\n", bmp_height);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "dimensiune: %d\n", file_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "identificatorul utilizatorului: %d\n", file_uid);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "timpul ultimei modificari: %s\n", modified_time);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "contorul de legaturi: %d\n", number_of_links);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces user: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces grup: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces altii: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului de citire.\n");
    }
}

void processRegularFile(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int file_uid = getFileUID(input_filename);

    char time_buffer[BUFFER];

    char *modified_time = getFileLastModification(input_filename, time_buffer);
    int number_of_links = getFileNoOfHardLinks(input_filename);

    char user_rights_buffer[RIGHTS_BUFFER];
    char group_rights_buffer[RIGHTS_BUFFER];
    char others_rights_buffer[RIGHTS_BUFFER];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[50];

    snprintf(temporary_string, sizeof(temporary_string), "nume fisier: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "dimensiune: %d\n", file_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "identificatorul utilizatorului: %d\n", file_uid);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "timpul ultimei modificari: %s\n", modified_time);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "contorul de legaturi: %d\n", number_of_links);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces user: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces grup: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces altii: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului de citire.\n");
    }
}

void processDirectory(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", input_filename);
        return;
    }

    int file_uid = getFileUID(input_filename);

    char user_rights_buffer[RIGHTS_BUFFER];
    char group_rights_buffer[RIGHTS_BUFFER];
    char others_rights_buffer[RIGHTS_BUFFER];

    char *user_rights = getUserRightsForFile(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForFile(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForFile(input_filename, others_rights_buffer);

    char temporary_string[BUFFER];

    snprintf(temporary_string, sizeof(temporary_string), "nume director: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "identificatorul utilizatorului: %d\n", file_uid);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces user: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces grup: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces altii: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului de citire.\n");
    }
}

void processSymLink(char *input_filename, int output_fd)
{
    int file_descriptor = open(input_filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", input_filename);
        return;
    }

    int file_size = getFileSize(input_filename);
    int link_size = getLinkSize(input_filename);

    char user_rights_buffer[RIGHTS_BUFFER];
    char group_rights_buffer[RIGHTS_BUFFER];
    char others_rights_buffer[RIGHTS_BUFFER];

    char *user_rights = getUserRightsForLink(input_filename, user_rights_buffer);
    char *group_rights = getGroupRightsForLink(input_filename, group_rights_buffer);
    char *others_rights = getOthersRightsForLink(input_filename, others_rights_buffer);

    char temporary_string[BUFFER];

    snprintf(temporary_string, sizeof(temporary_string), "nume legatura: %s\n", getFileName(input_filename));
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "dimensiune legatura: %d\n", link_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "dimensiune fisier legatura: %d\n", file_size);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces user legatura: %s\n", user_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces grup legatura: %s\n", group_rights);
    write(output_fd, temporary_string, getChars(temporary_string));
    snprintf(temporary_string, sizeof(temporary_string), "drepturi de acces altii legatura: %s\n", others_rights);
    write(output_fd, temporary_string, getChars(temporary_string));

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului de citire.\n");
    }
}

// opens filename and returns the number of new lines
uint32_t getLines(char *filename)
{
    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", filename);
        return 0;
    }

    char buffer[BUFFER];
    int lineCounter = 0;
    int reads = 0;
    while ((reads = read(file_descriptor, buffer, BUFFER)) > 0)
    {
        for (int i = 0; i < reads; i++)
        {
            if (buffer[i] == '\n')
                lineCounter++;
        }
    }

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului de citire.\n");
        return 0;
    }

    return lineCounter;
}

// removes extension from filename
void removeExtension(char *filename)
{
    char *pos = NULL;
    pos = strrchr(filename, '.');
    if (pos == NULL)
    {
        return; // it has no extension already
    }
    int index = (int)(pos - filename);
    filename[index] = 0;
}

uint16_t getBMPBitsPerPixel(char *filename)
{
    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", filename);
        return 0;
    }

    int res = lseek(file_descriptor, BITCOUNT_OFFSET, SEEK_SET);
    if (res == -1)
    {
        return 0;
    }

    uint8_t bitcount_field[2];

    if (read(file_descriptor, bitcount_field, 2) != 2)
    {
        printf("Could not read bitcount field.\n");
        return 0;
    }

    uint16_t number = bitcount_field[0] | bitcount_field[1] << 8;

    if (close(file_descriptor) != 0)
    {
        printf("Error closing fisierului pentru editare BMP la grayscale.\n");
        return 0;
    }

    return number;
}

int convertGrayscaleBMPFile(char *filename)
{
    int file_descriptor = open(filename, O_RDWR);
    if (file_descriptor == -1)
    {
        printf("Error opening file for reading: %s\n", filename);
        return -1;
    }

    // image processing

    uint16_t bits_per_pixel = getBMPBitsPerPixel(filename);

    unsigned char pixel_buffer[BUFFER];
    memset(pixel_buffer, 0, sizeof(pixel_buffer));

    if (bits_per_pixel == 0)
    {
        return -1;
    }

    else if (bits_per_pixel == 1)
    {
        // monochrome picture already black and white
        // goes to end to close file and return 0;
    }

    else if (bits_per_pixel <= 8)
    {
        uint32_t numcolors = pow(2, bits_per_pixel);
        uint32_t size = 4 * numcolors;

        int res = lseek(file_descriptor, COLORTABLE_OFFSET, SEEK_SET);
        if (res == -1)
        {
            return -1;
        }
        if ((read(file_descriptor, pixel_buffer, size)) != size)
        {
            printf("Error at reading.\n");
            return -1;
        }

        for (int i = 0; i < size; i = i + 4) // size max 1024 (4*256)
        {
            unsigned char gray_color = 0.299 * pixel_buffer[i] + 0.587 * pixel_buffer[i + 1] + 0.114 * pixel_buffer[i + 2];
            memset(pixel_buffer + i, gray_color, 3);
        }

        res = lseek(file_descriptor, COLORTABLE_OFFSET, SEEK_SET);
        if (res == -1)
        {
            return -1;
        }

        if ((write(file_descriptor, pixel_buffer, size)) != size)
        {
            printf("Error at writing.\n");
            return -1;
        }
    }

    else
    {
        int width = 0;
        int height = 0;

        int retvalWidthHeight = getBMPWidthHeight(file_descriptor, &width, &height);
        if (retvalWidthHeight != 0)
        {
            printf("Error getting width and height.\n");
            return -1;
        }

        int res = lseek(file_descriptor, COLORTABLE_OFFSET, SEEK_SET);
        if (res == -1)
        {
            return -1;
        }

        int reads = 0;

        if ((reads = read(file_descriptor, pixel_buffer, BUFFER)) == -1)
        {
            printf("Error at reading.\n");
            return -1;
        }

        int i = 0;

        while (1)
        {
            if (reads < 3) // not a pixel read
            {
                break;
            }

            if (i + 2 > reads)
            {
                i = 0;

                res = lseek(file_descriptor, -reads, SEEK_CUR);
                if (res == -1)
                {
                    return -1;
                }

                if (write(file_descriptor, pixel_buffer, reads) != reads)
                {
                    printf("Error at writing pixels.\n");
                    return -1;
                }

                reads = read(file_descriptor, pixel_buffer, BUFFER);
                continue;
            }

            unsigned char gray_color = 0.299 * pixel_buffer[i] + 0.587 * pixel_buffer[i + 1] + 0.114 * pixel_buffer[i + 2];
            memset(pixel_buffer + i, gray_color, 3);

            i = i + 3;
        }
    }

    if (close(file_descriptor) != 0)
    {
        printf("Error closing BMP file at grayscale editing.\n");
        return -1;
    }

    return 0;
}

int countCorrectLinesForRegex(char *filepath, char *path_for_regex, char *ch)
{
    int pid = 1;
    int pfd1[2];
    int pfd2[2];

    FILE *stream = NULL;

    if (pipe(pfd1) < 0)
    {
        perror("Error creating pipe 1\n");
        return -1;
    }

    if (pipe(pfd2) < 0)
    {
        perror("Error creating pipe 2\n");
        return -1;
    }

    if ((pid = fork()) < 0)
    {
        perror("Error at fork.\n");
        return -1;
    }

    if (pid == 0)
    {
        // Child process: "cat" command
        if (close(pfd1[0]) == -1)
        {
            printf("Error closing pfd1[1]\n");
            return -1;
        }
        if (dup2(pfd1[1], 1) == -1)
        {
            printf("Error at dup2 pfd1[1]\n");
            return -1;
        }
        if (close(pfd1[1]) == -1)
        {
            printf("Error closing pfd1[1]\n");
            return -1;
        }
        execlp("cat", "cat", filepath, NULL);
        perror("Error at exec cat\n");
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("Error at fork.\n");
        return -1;
    }

    if (pid == 0)
    {
        // Child process: "regex.sh" command
        if (close(pfd1[1]) == -1)
        {
            printf("Error closing pfd1[1]\n");
            return -1;
        }
        if (close(pfd2[0]) == -1)
        {
            printf("Error closing pfd2[0]\n");
            return -1;
        }
        if (dup2(pfd1[0], 0) == -1)
        {
            printf("Error at dup2 pfd1[0]\n");
            return -1;
        }
        if (dup2(pfd2[1], 1) == -1)
        {
            printf("Error at dup2 pfd2[1]\n");
            return -1;
        }
        if (close(pfd1[0]) == -1)
        {
            printf("Error closing pfd1[0]\n");
            return -1;
        }
        if (close(pfd2[1]) == -1)
        {
            printf("Error closing pfd2[1]\n");
            return -1;
        }
        execlp(path_for_regex, path_for_regex, ch, NULL);
        perror("Error at exec regex.sh\n");
        return -1;
    }

    int counter_buff = 0;

    // Parent process
    if (close(pfd1[0]) == -1)
    {
        printf("Error closing pfd1[0]\n");
        return -1;
    }
    if (close(pfd1[1]) == -1)
    {
        printf("Error closing pfd1[1]\n");
        return -1;
    }
    if (close(pfd2[1]) == -1)
    {
        printf("Error closing pfd2[1]\n");
        return -1;
    }
    if ((stream = fdopen(pfd2[0], "r")) == NULL)
    {
        return -1;
    }
    fscanf(stream, "%d", &counter_buff);
    if (close(pfd2[0]) == -1)
    {
        printf("Error closing pfd2[0]\n");
        return -1;
    }

    return counter_buff;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage error: %s <director_intrare> <director_iesire> <c>\n", argv[0]);
        exit(-1);
    }

    DIR *input_dir = opendir(argv[1]);
    DIR *output_dir = opendir(argv[2]);

    if (input_dir == NULL)
    {
        perror(argv[1]);
        exit(-1);
    }
    if (output_dir == NULL)
    {
        perror(argv[2]);
        exit(-1);
    }

    struct dirent *dir_entry = NULL;

    char input_file_path[MAX_FILE_PATH];
    char output_file_path[MAX_FILE_PATH];

    int lines = 0;
    int counter = 0;

    while ((dir_entry = readdir(input_dir)) != NULL)
    {
        int size = strlen(argv[1]) + strlen(dir_entry->d_name) + 1;
        if (size > MAX_FILE_PATH - 1 - 1) // 1 for terminator, 1 for array-1
        {
            printf("File path too big for reading.");
            continue;
        }

        snprintf(input_file_path, sizeof(input_file_path), "./%s%s", argv[1], dir_entry->d_name);

        char output_file_name[MAX_FILE_NAME];
        snprintf(output_file_name, sizeof(output_file_name), "%s", dir_entry->d_name);
        if (isRegularFile(input_file_path)) // don't remove dot for directories.
        {
            removeExtension(output_file_name);
        }
        snprintf(output_file_path, sizeof(output_file_path), "./%s%s_statistica.txt", argv[2], output_file_name);

        int output_fd = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd == -1)
        {
            printf("Eroare la deschiderea fisierului de scriere: %s.\n", output_file_path);
            exit(-1);
        }

        int pid = 1;

        if (isRegularFile(input_file_path))
        {
            if ((pid = fork()) < 0)
            {
                perror("Eroare la fork\n");
                exit(1);
            }
            if (pid == 0)
            {
                if (isSymLink(input_file_path))
                {
                    processSymLink(input_file_path, output_fd);
                }
                else if (hasExtension(input_file_path, ".bmp"))
                {
                    processBMPFile(input_file_path, output_fd);
                }
                else
                {
                    processRegularFile(input_file_path, output_fd);
                }
                lines = getLines(output_file_path);
                exit(lines);
            }

            if (!hasExtension(input_file_path, ".bmp"))
            {
                int counter_buff = 0;
                counter_buff = countCorrectLinesForRegex(input_file_path, "./regex.sh", argv[3]);
                if (counter_buff == -1)
                {
                    printf("A aparut o eroare la procesarea numarului de linii corecte pentru fisierul: %s", input_file_path);
                }
                else
                {
                    counter = counter + counter_buff;
                }
            }
        }

        else if (isDirectory(input_file_path))
        {
            if ((pid = fork()) < 0)
            {
                perror("Eroare la fork\n");
                exit(1);
            }
            if (pid == 0)
            {
                processDirectory(input_file_path, output_fd);
                lines = getLines(output_file_path);
                exit(lines);
            }
        }

        if (isRegularFile(input_file_path) && hasExtension(input_file_path, ".bmp") && !isSymLink(input_file_path))
        {
            if ((pid = fork()) < 0)
            {
                perror("Eroare la fork\n");
                exit(1);
            }
            if (pid == 0)
            {
                int retval = 0;
                retval = convertGrayscaleBMPFile(input_file_path);
                if (retval != 0)
                {
                    printf("Imaginea %s, nu s-a convertit cu succes.\n", input_file_path);
                }
                exit(retval);
            }
        }

        if (close(output_fd) != 0)
        {
            printf("Eroare la inchiderea fisierului de scriere.\n");
        }
    }

    int status = 0;
    int wpid = 0;
    while ((wpid = wait(&status)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("S-a incheiat procesul cu PID-ul %d codul %d status %d.\n", wpid, status, WEXITSTATUS(status));
        }
    }

    printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c.\n", counter, argv[3][0]);

    if (closedir(input_dir) != 0)
    {
        printf("Eroare la inchiderea directorului de citire.");
        exit(-1);
    }

    if (closedir(output_dir) != 0)
    {
        printf("Eroare la inchiderea directorului de scriere.");
        exit(-1);
    }

    return 0;
}
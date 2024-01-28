#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

void writeString(char *path, int open_flag);

int main(int argc, char **argv)
{
    int opt, open_flag, fd, rtread, rtwrite;
    char buf[4096];
    char path[1024];
    const char* optstring = "a:";
    int append_flag = false;
    
    if (argc == 2)
    {
        strcpy(path, argv[1]);
        goto exec_tee;
    }
    
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
            case 'a':
                append_flag = true;
                strcpy(path, optarg);
                break;
            default:
                printf("An error has occured.\n");
                break;
        }
exec_tee:
        if (append_flag == true)
        {
            open_flag = O_RDWR | O_APPEND | O_CREAT;
        }
        else
        {
            open_flag = O_RDWR | O_CREAT | O_TRUNC;
        }
        writeString(path, open_flag);
    }
    
    return 0;
}

void writeString(char *path, int open_flag)
{
    int fd, rtread, rtwrite;
    char buf[4096];

    if ((fd = open(path, open_flag)) == -1)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    while((rtread = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        rtwrite = write(STDOUT_FILENO, buf, rtread);
        if (rtwrite != rtread)
        {
            perror("An error has occured while writing to stdout.\n");
            exit(EXIT_FAILURE);
        }
        rtwrite = write(fd, buf, rtread);
        if (rtwrite != rtread)
        {
            perror("An error has occured while writing to the file.\n");
            exit(EXIT_FAILURE);
        }
    }

    close(fd);                                                              }

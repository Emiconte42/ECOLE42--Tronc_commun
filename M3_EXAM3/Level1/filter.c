
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    char    c;
    int     i;
    int     wlen;
    int     ret;

    if (argc != 2 || argv[1][0] == '\0')
        return (1);
    wlen = strlen(argv[1]);
    i = 0;
    while ((ret = read(0, &c, 1)) > 0)
    {
        if (c == argv[1][i])
            i++;
        else
        {
            write(1, argv[1], i);
            i = 0;
            if (c == argv[1][0])
                i++;
            else
                write(1, &c, 1);
        }
        if (i == wlen)
        {
            while (i--)
                write(1, "*", 1);
            i = 0;
        }
    }
    if (ret < 0)
    {
        perror("Error");
        return (1);
    }
    write(1, argv[1], i);
    return (0);
}

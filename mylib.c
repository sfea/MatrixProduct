
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int* new_read(int old_fd, void* buf, int numb)
{
        int re = 1;
        int i = 1;
        int c = 0;

        while(c != numb)
        {
                re = read(old_fd, buf + c, numb);
                if(re < 0)
                {
                        perror("read");
                        exit(-1);
                }
                c += re;
        }
}

void new_write(int new_fd, void* buf, int numb)
{
        int c = 0, i = 0;
        int wr;

        for (i = 0;;i++)
        {
                wr = write(new_fd, buf + c, numb - c);
                if (wr >= 0)
                        c += wr;
                if (numb == c)
                        break;
                if (wr < 0)
                {
                        perror("write");
                        break;
                }
        }
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE_1 "./file.txt" //файл, куда записываются матрицы

int main(int argc, char** argv)
{
        int n = atoi(argv[1]);
        int fd = 0;
        char* a;
        int matr[2*n*n];
        int i = 0, j = 0;
        srand(time(NULL));

        fd = open(FILE_1, O_CREAT|O_RDWR, 0666);
        if (fd < 0)
        {
                perror("open");
                exit(1);
        }

        for(i = 0; i < 2*n*n; i++)
        {
                matr[i] = rand()%100;
                if (i % n == 0 && i != 0)
                        printf("\n");
                if (i == n*n)
                        printf("\n");
                printf("%d ", matr[i]);
        }

        printf("\n");

        new_write(fd, &n, sizeof(int));

        new_write(fd, matr, 2*n*n*sizeof(int));
}




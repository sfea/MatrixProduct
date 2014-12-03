#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define FILE_1 "./file.txt"
#define FILE_2 "./result.txt"

int* data_1 = 0;
int* data_2 = 0;
// AP: сделайте так, чтобы результат не был разделяемой переменной, а был доступен только в одном главном потоке, котоырй собирает результат
int n = 0;
int i = 0;

struct thread_info
{
	pthread_t id;
	int first_pos;
	int length;
};

void* thread_func(void* arg)
{
        struct thread_info* thr = arg;
        int i = 0, j = 0;
        int* matr = (int*)calloc(thr -> length, sizeof(int));
        int last_pos = thr -> first_pos + thr -> length;
        int str = 0, col = 0;

        for (i = 0; i < thr -> length; i++)
        {
                str = (thr -> first_pos + i) / n;
                col = (thr -> first_pos + i) % n;
                for (j = 0; j < n; j++)
                        matr[i] += data_1[str*n + j] * data_2[n*j + col];
        }

        pthread_exit(matr);
}


int main(int argc, char** argv)
{
	int m = strtol(argv[1], NULL, 10);
	int* fin = 0;
	int* res = 0;
	int fd_op_1 = 0, fd_op_2 = 0;;
	struct thread_info* thr = (struct thread_info*)calloc(m, sizeof(struct thread_info));
	int eff_thr_numb = 0;
	int fd_wr = 0;
	struct timeval* mytime = (struct timeval*)calloc(2, sizeof(struct timeval));
	gettimeofday(mytime, NULL);

	if ((fd_op_1 = open(FILE_1, O_RDONLY, 0666)) < 0)
	{
		perror("open_1");
		exit(1);
	}

	if ((fd_op_2 = open(FILE_2, O_CREAT|O_RDWR, 0666)) < 0)
        {
                perror("open_2");
                exit(1);
        }

	new_read(fd_op_1, &n, sizeof(int));

        data_1 = (int*)calloc(n*n, sizeof(int));
        data_2 = (int*)calloc(n*n, sizeof(int));
        res = (int*)calloc(n*n, sizeof(int));
        fin = (int*)calloc(m, sizeof(int));
	// AP: везде где вы используете потоковые чтение и запись надо пользоваться функциями своей библиотеки пбферизованного чтения записи
	new_read(fd_op_1, data_1, n*n*sizeof(int));

	new_read(fd_op_1, data_2, n*n*sizeof(int));

	if (m < n*n)
	{
		eff_thr_numb = n*n / m;
		for (i = 0; i < m; i++)
		{
			thr[i].length = eff_thr_numb;
			thr[i].first_pos = i*eff_thr_numb;
		}

		if (n*n % m != 0)
			thr[m - 1].length += n*n % m;
	}

	else
	{
		m = n*n;
		for (i = 0; i < m; i++)
		{
			thr[i].length = 1;
			thr[i].first_pos = i;
		}
	}

	for (i = 0; i < m; i++)

		pthread_create(&thr[i].id, NULL, thread_func, &thr[i]);

	for (i = 0; i < m; i++)
        {
                pthread_join(thr[i].id, (void**)&fin);
                memcpy(res, fin, thr[i].length*sizeof(int));
                res += thr[i].length;
        }
        
        for (i = 0; i < m; i++)
                res -= thr[i].length;	//Возвращение указателя в прежнее место
		
	dup2(fd_op_2, STDOUT_FILENO);
	printf("\nМатрица произведения:\n");
	for (i = 0; i < n*n; i++)
	{
		if (i % n == 0)
		printf("\n");
		printf("%d ", res[i]);
	}
	printf("\n");
	gettimeofday(mytime + 1, NULL);
	printf("working time = %ldmc\n", mytime[1].tv_usec - mytime[0].tv_usec +
		1000000 * (mytime[1].tv_sec - mytime[0].tv_sec));
	close(fd_op_2);
}

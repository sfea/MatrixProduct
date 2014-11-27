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
int* res = 0;
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
	int i = 0;
	int last_pos = thr -> first_pos + thr -> length;
	int pos = 0;
	int str = 0, col = 0;

	for (pos = thr -> first_pos; pos < last_pos; pos++)
	{
		str = pos / n;
		col = pos % n;

		for (i = 0; i < n; i++)
			res[pos] += data_1[str*n + i] * data_2[n*i + col];
	}
}


int main(int argc, char** argv)
{
	int m = strtol(argv[1], NULL, 10);
	int f = 0;
	int fd_rd = 0;
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

	if ((fd_rd = read(fd_op_1, &n, sizeof(int))) != sizeof(int))
	{
		perror("read_1");
		exit(1);
	}

        data_1 = (int*)calloc(n*n, sizeof(int));
        data_2 = (int*)calloc(n*n, sizeof(int));
        res = (int*)calloc(n*n, sizeof(int));

	if ((fd_rd = read(fd_op_1, data_1, n*n*sizeof(int))) != n*n*sizeof(int))
	{
		perror("read_2");
		exit(1);
	}

	if ((fd_rd = read(fd_op_1, data_2, n*n*sizeof(int))) < 0)
	{
		perror("read_3");
		exit(1);
	}

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
		pthread_join(thr[i].id, NULL);

	f = fork();

	if (f > 0)
	{
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
}

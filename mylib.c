#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int new_read(char* buf, int old_fd)
{
	int re = 1;
	int i = 1;
	for (i = 1;;i++)
	{
      if (re = read(old_fd, buf, 1024) == 1024)
			buf = realloc(buf, i*1024);
		else
		{
			buf = realloc(buf, (i - 1)*1024 + re);
			break;
		}
		if (re < 0)
			return -1;
	}

	free(buf);
}

void new_write(char* buf, int new_fd)
{
	int c = 0;
	int wr = 0;
	int len = 0;
	len = strlen(buf);
	printf("%d\n", len);
	while(wr = write(new_fd, buf + c, len - c) > 0)
	{
		c += wr;
		if (len == c) break;
		if (wr < 0)
		{
			perror("write");
			break;
		}
	}
}

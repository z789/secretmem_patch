#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <linux/memfd.h>
#include "secmem.h"

int main(void)
{
	char data[] = "Secret data!";
	size_t size = sizeof(data);
	char *s;

	printf("PID: %d\n", getpid());

	s = smalloc(size);
	if (!s)
		error(2, errno, "secret_alloc() failed");

	strncpy(s, data, size);
	printf("Copied %zu bytes to secret area %p\n", size, s);
	//printf("Secretmem:%s\n",s);
	memset(data, 0, sizeof(data));
	printf("data:%s\n", data);

	printf("Waiting...\n");
	pause();

	sfree(s);
	return 0;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <linux/memfd.h>

#ifndef MFD_SECRET
#define MFD_SECRET              0x0008U
#endif

#define __NR_memfd_create 319

static int memfd_secret(unsigned int flags)
{
	return syscall(__NR_memfd_create, "secretmem", MFD_SECRET | flags);
}

static void *secret_alloc(size_t size)
{
	int fd = -1;
	void *m;
	void *result = NULL;

	fd = memfd_secret(0);
	if (fd < 0)
		goto out;

	if (ftruncate(fd, size) < 0)
		goto out;

	m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (m == MAP_FAILED)
		goto out;

	result = m;

out:
	if (fd >= 0)
		close(fd);
	return result;
}

static void secret_free(void *p, size_t size)
{
	munmap(p, size);
}

int main(void)
{
	char data[] = "Secret data!";
	size_t size = sizeof(data);
	char *s;

	printf("PID: %d\n", getpid());

	s = secret_alloc(size);
	if (!s)
		error(2, errno, "secret_alloc() failed");

	strncpy(s, data, size);
	printf("Copied %zu bytes to secret area %p\n", size, s);
	//printf("Secretmem:%s\n",s);
	memset(data, 0, sizeof(data));
	printf("data:%s\n", data);

	printf("Waiting...\n");
	pause();

	secret_free(s, size);
	return 0;
}

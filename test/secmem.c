#include <sys/mman.h>
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <linux/mman.h>
#include "secmem.h"

struct secmem_head {
	off_t len;
	char ptr[0];
};

#if LINUX_VERSION_CODE >= 0x51400
#define __NR_memfd_secret 447
static int memfd_secret(unsigned int flags)
{
	return syscall(__NR_memfd_secret, flags);
}
#else

#ifndef MFD_SECRET
#define MFD_SECRET              0x0008U
#endif
#define __NR_memfd_create 319
static int memfd_secret(unsigned int flags)
{
	return syscall(__NR_memfd_create, "secretmem", MFD_SECRET | flags);
}
#endif

void *smalloc(size_t size)
{
	struct secmem_head *head = NULL;
	int fd = -1;
	void *m = NULL;
	void *result = NULL;

	if (size <= 0)
		goto out;

	fd = memfd_secret(0);
	if (fd < 0)
		goto out;

	if (ftruncate(fd, size + sizeof(*head)) < 0)
		goto out;

	m = mmap(NULL, size+sizeof(*head), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (m == MAP_FAILED)
		goto out;

	head = (struct secmem_head *)m;
	head->len  = size;
	result = head->ptr;

out:
	if (fd >= 0)
		close(fd);
	return result;
}

void *scalloc(size_t nmemb, size_t size)
{
	void *result = NULL;
	size_t len = 0;

	if (nmemb <=0 || size <= 0)
		goto end;
	len = nmemb * size;

	result = smalloc(len);
        if (result)
		memset(result, 0, len);	
end:
	return result;
}

void *srealloc(void *ptr, size_t size)
{
	struct secmem_head *head = NULL;
	void *new = NULL;
	off_t old_size = 0; 

	if (size < 0)
		return NULL;

	if (!ptr)
		return smalloc(size);

	if (ptr && size == 0) {
		sfree(ptr);
		return NULL;
	}

	new = smalloc(size);
	if (new) {
		head = (struct secmem_head *)((char *)ptr - sizeof(*head));
		old_size = head->len;
		if (size > old_size)
			size = old_size;
		memcpy(new, ptr, size);
		sfree(ptr);

		return new;
	}
	return ptr;
}

void *sreallocarray(void *ptr, size_t nmemb, size_t size)
{
	void *result = NULL;
	unsigned long long len = nmemb * size;

	if (sizeof(off_t) == 8 && len > 0X7FFFFFFFFFFFFFFF) {
		errno = ENOMEM;
		goto end;
	}

	if (sizeof(off_t) == 4 && len > 0X7FFFFFFF) {
		errno = ENOMEM;
		goto end;
	}

	result = srealloc(ptr, len);
	if (ptr && len > 0 && !result)
		errno = ENOMEM;
end:
	return result;
}

void sfree(void *p)
{
	struct secmem_head *head = NULL;
	if (p) {
		head = (struct secmem_head *)((char *)p-sizeof(*head));
		munmap((void*)head, head->len + sizeof(*head));
	}
	return;
}

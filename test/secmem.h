#ifndef _SECMEM_H
#define _SECMEM_H
/*
 * Alloc security memory API if the memfd_secret syscall is valid
 */

#include <unistd.h>

void *smalloc(size_t size);
void *scalloc(size_t nmemb, size_t size);
void *srealloc(void *ptr, size_t size);
void *sreallocarray(void *ptr, size_t nmemb, size_t size);
void sfree(void *p);
#endif

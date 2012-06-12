/*
 *  nonstd.c - A collection of trivial wrappers to allow typical libraries
 *             to work within the yaboot environment.
 *
 *  Copyright 2011 Tony Breeds, IBM Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "ctype.h"
#include "types.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "prom.h"
#include "nonstd.h"
#include "debug.h"

#define	__unused	__attribute__((unused))

static FILE _stdout;
static FILE _stderr;
FILE *stdout = &_stdout;
FILE *stderr = &_stderr;

static int fake_errno;

int * __errno_location(void)
{
	DEBUG_F("Stub function called");
	return &fake_errno;
}

uid_t geteuid(void)
{
	DEBUG_F("Stub function called");
	return 0;
}

uid_t getuid(void)
{
	DEBUG_F("Stub function called");
	return 0;
}

pid_t getpid(void)
{
	DEBUG_F("Stub function called");
	return 1;
}

/* selected by roll of a six sided dice ... that's random right? */
int rand(void)
{
	DEBUG_F("Stub function called");
	return 4;
}

void srand(unsigned int seed __unused)
{
	DEBUG_F("Stub function called");
}

long int random(void)
{
	DEBUG_F("Stub function called");
	return 4;
}

void srandom(unsigned int seed __unused)
{
	DEBUG_F("Stub function called");
}

unsigned int sleep(unsigned int seconds)
{
	prom_sleep(seconds);
	return 0;
}

int stat(const char *path __unused, struct stat *buf __unused)
{
	DEBUG_F("Stub function called");
	return EACCES;
}

int stat64(const char *path __unused, struct stat *buf __unused)
{
	DEBUG_F("Stub function called");
	return EACCES;
}

int fstat(int fd __unused, struct stat *buf __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

int fstat64(int fd __unused, struct stat *buf __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

int __xstat64(int vers __unused, const char *name __unused,
              struct stat64 *buf __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

int __fxstat64(int vers __unused, int fd __unused, struct stat64 *buf __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

int open(const char *pathname, int flags __unused, mode_t mode __unused)
{
	return (int) prom_open((char *)pathname);
}

int open64(const char *pathname, int flags __unused, mode_t mode __unused)
{
	return (int) prom_open((char *)pathname);
}

int __open64_2(const char *pathname, int flags __unused)
{
	return (int) prom_open((char *)pathname);
}

off_t lseek(int fd __unused, off_t offset __unused, int whence __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

off64_t lseek64(int fd __unused, off64_t offset __unused, int whence __unused)
{
	DEBUG_F("Stub function called");
	return EBADF;
}

ssize_t read(int filedes, void *buf, size_t nbyte)
{
	return prom_read((void *)filedes, buf, nbyte);
}

int close(int fd __unused)
{
	prom_close((void *)fd);
	return 0;
}

int gethostname(char *name __unused, size_t len __unused)
{
	DEBUG_F("Stub function called");
	return EPERM;
}

int gettimeofday(struct timeval *tv __unused, struct timezone *tz __unused)
{
	DEBUG_F("Stub function called");
	return EPERM;
}

int printf(const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

int fprintf(FILE *stream __unused, const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

int fputs(const char *s, FILE *stream __unused)
{
	prom_printf("%s", s);

	return 0;
}

int fflush(FILE *stream __unused)
{
	DEBUG_F("Stub function called");
	return 0;
}

char *getenv(const char *name __unused)
{
	DEBUG_F("Stub function called");
	return NULL;
}

int __printf_chk(int flag __unused, const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	DEBUG_F("Stub function called");
	return 0;
}

int __sprintf_chk(char *str __unused, int flag __unused,
                  size_t strlen __unused, const char * format __unused, ...)
{
	DEBUG_F("Stub function called");
	return 0;

}

int __fprintf_chk(FILE *stream __unused, int flag __unused,
                  const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

void *__memcpy_chk(void *dest, const void *src, size_t n,
                   size_t destlen __unused)
{
	DEBUG_F("Stub function called");
	/* FIXME: We really could check that dest+destlen < src, to ensure
	 * we're not overwriting the src */
	return memcpy(dest, src, n);
}

void perror(const char *s)
{
	DEBUG_F("Stub function called");
	prom_printf(s);
}

void exit(int status __unused)
{
	prom_exit();
	for(;;);
}

int ioctl(int d __unused, int request __unused, ...)
{
	DEBUG_F("Stub function called");
	return 0;
}

/* As we do not implement fopen() I think we're only going to get called
 * with stdout and stderr.  If that's the case we degenerate to prom_write()
 * on stdout */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if ((stream == stdout) || (stream == stderr)) {
		DEBUG_F("Stub function called on known stream");
		prom_write(prom_stdout, (void *)ptr, size * nmemb);
	} else {
		DEBUG_F("Stub function called on unknown stream");
	}
	return nmemb;
}

long sysconf(int name __unused)
{
	DEBUG_F("Stub function called");
	return -1;
}

int getpagesize(void)
{
	DEBUG_F("Stub function called");
	/* I think this is safe to assume */
	return 4096;
}

void qsort(void *base __unused, size_t nmemb __unused, size_t size __unused,
           int(*compar)(const void *, const void *))
{
	DEBUG_F("Stub function called");
	/* I'm quite nervous about not implementing this.  Could we end up with
	 * disk corruption.  Couldn't we? */
}

/* FIXME: I'd like to call prom_write here, but we can't ber certain that
 *        we'll have "text" so just move along nothing to see here
 */
ssize_t write(int fd, const void *buf, size_t count)
{
	DEBUG_F("Stub function called");
	return 0;
}

int fallocate(int fd __unused, int mode __unused, off_t offset __unused,
              off_t len __unused)
{
	DEBUG_F("Stub function called");
	return 0;
}

unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
	return simple_strtoull(nptr, endptr, base);
}

int fsync(int fd __unused)
{
	DEBUG_F("Stub function called");
	return 0;
}

/* Return EFAULT here as it's too hard in yaboot to know the size of struct
 * utsname.  If we don't touch it and return 0 bad things might happen.
 * Lets hope the caller handles failure
 */
int uname(struct utsname *buf __unused)
{
	DEBUG_F("Stub function called");
	return EFAULT;
}

int getrlimit(int resource __unused, struct rlimit *rlim __unused)
{
	DEBUG_F("Stub function called");
	return 0;
}

int setrlimit(int resource __unused, const struct rlimit *rlim __unused)
{
	DEBUG_F("Stub function called");
	return 0;
}

void __stack_chk_fail(void)
{
	DEBUG_F("Stub function called");
}

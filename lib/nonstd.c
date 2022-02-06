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
#include "ctype.h"
#include "prom.h"
#include "nonstd.h"

FILE *stdout;

int printf(const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

int fprintf(FILE *stream, const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

int fputs(const char *s, FILE *stream)
{
	prom_printf("%s", s);

	return 0;
}

int fflush(FILE *stream)
{
	return 0;
}

char *getenv(const char *name)
{
	return NULL;
}

int lseek(int fd, int offset, int whence) {
	// XXX: This whence addition seems wrong ..
	return prom_lseek((void *)fd, whence + offset);
}

int lseek64(int fd, int64_t offset, int whence) {
	return lseek(fd, offset, whence);
}

int open(const char *pathname, int flags) {
	return (int) prom_open((char *)pathname);
}

int open64(const char *pathname, int flags) {
	return open(pathname, flags);
}

// Internal glibc fortify calls.
int __open64_2(const char *path, int flags) {
	return open64(path, flags);
}

int read(int fd, void *buf, size_t count) {
	return prom_read((void *)fd, buf, count);
}

int write(int fd, const void *buf, size_t count) {
	return prom_write((void *)fd, (void *)buf, count);
}

int close(int fd) {
	prom_close((void *)fd);
	return 0;
}

int pread(int fd, void *buf, size_t count, int32_t offset) {
	int curr = lseek(fd, 0, 0 /*SEEK_CUR*/);
	lseek(fd, offset, 0 /*SEEK_SET*/);
	int ret = read(fd, buf, count);
	lseek(fd, curr, 0 /*SEEK_SET*/);
	return ret;
}

int pread64(int fd, void *buf, int64_t count, int64_t offset) {
	return pread(fd, buf, count, offset);
}

int pwrite(int fd, const void *buf, size_t count, int32_t offset) {
	int curr = lseek(fd, 0, 0 /*SEEK_CUR*/);
	lseek(fd, offset, 0 /*SEEK_SET*/);
	int ret = write(fd, buf, count);
	lseek(fd, curr, 0 /*SEEK_SET*/);
	return ret;
}

int pwrite64(int fd, const void *buf, int64_t count, int64_t offset) {
	return pwrite(fd, buf, count, offset);
}

// No fsync, just assume we've sync'd
int fsync(int fd) {
	return 0;
}

// ext2 libs only use this to turn off caches currently
int fcntl(int fd, int cmd, ...) {
	return 0;
}

void exit(int status) {
	prom_exit();
}

int __printf_chk(int flag, const char *format, ...) {
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

int __sprintf_chk(char * str, int flag, size_t strlen, const char * format, ...) {
	va_list ap;
	va_start(ap, format);
	// No sprintf? :(
	va_end(ap);
	return 0;

}

int __fprintf_chk(FILE *stream, int flag, const char *format, ...) {
	va_list ap;
	va_start (ap, format);
	prom_vfprintf (prom_stdout, format, ap);
	va_end (ap);

	return 0;
}

void *memcpy(void *dest, const void *src, size_t n);
void *__memcpy_chk(void *dest, const void *src, size_t n, size_t destlen) {
	return memcpy(dest, src, n);
}

// But these are all dummy functions
int __xstat64 (int __ver, const char *__filename, void *__stat_buf) {
	return 0;
}

int stat64(const char *path, void *stat_buf) {
	return 0;
}

int fstat64(int fildes, void *stat_buf) {
	return 0;
}

int __fxstat64 (int __ver, int __filedesc, void *__stat_buf) {
	return 0;
}

signed int random(void) {
	return 0;
}

void srandom(unsigned int seed) {
	return;
}

int rand(void) {
	return 0;
}

void srand(unsigned int seed) {
	return;
}

unsigned int sleep(unsigned int seconds) {
	return 0;
}

int gettimeofday(void *tv, void *tz) {
	return 0;
}

long sysconf(int name) {
	return 0;
}

int getpagesize(void) {
	return 0;
}

int gethostname(char *name, size_t len) {
	return 0;
}

int getpid(void) {
	return 0;
}

int getuid(void) {
	return 0;
}

void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *)) {
	return;
}

int * __errno_location(void) {
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, void *stream) {
	return 0;
}

int ioctl(int d, int request, ...) {
	return 0;
}

int fallocate(int fd, int mode, unsigned int offset, unsigned int len) {
	return 0;
}

int uname(void *buf) {
	return 0;
}

int setrlimit(int resource, void *rlim) {
	return 0;
}

unsigned long long int strtoull(const char *nptr, char **endptr, int base) {
	return 0;
}

int getrlimit(int resource, void *rlim) {
	return 0;
}

int stderr = 0;
int perror = 0;

/*
 *  nonstd.h - A collection of trivial wrappers to allow typical libraries
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

#ifndef NONSTD_H
#define NONSTD_H

/* Copied from asm-generic/errno-base.h */
#define	EPERM		 1	/* Operation not permitted */
#define	EBADF		 9	/* Bad file number */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */

typedef int FILE;

typedef unsigned int uid_t;
typedef int pid_t;
typedef unsigned int mode_t;
typedef int ssize_t;
typedef long long off64_t;
typedef long off_t;

struct stat;
struct stat64;
struct timezone;
struct timeval;
struct rlimit;
struct utsname;

extern FILE *stdout;
extern FILE *stderr;

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int fputs(const char *s, FILE *stream);
int fflush(FILE *stream);
char *getenv(const char *name);
int gethostname(char *name, size_t len);
int gettimeofday(struct timeval *tv, struct timezone *tz);
int * __errno_location(void);
unsigned int sleep(unsigned int seconds);
int rand(void);
void srand(unsigned int seed);
long int random(void);
void srandom(unsigned int seed);
uid_t geteuid(void);
uid_t getuid(void);
pid_t getpid(void);
int stat(const char *path, struct stat *buf);
int stat64(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int fstat64(int fd, struct stat *buf);
int open(const char *pathname, int flags, mode_t mode);
int open64(const char *pathname, int flags, mode_t mode);
off_t lseek(int fd, off_t offset, int whence);
off64_t lseek64(int fd, off64_t offset, int whence);
ssize_t read(int fildes, void *buf, size_t nbyte);
int close(int fd);
void *calloc(size_t nmemb, size_t size);
void perror(const char *s);
void exit(int status);
int ioctl(int d, int request, ...);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
long sysconf(int name);
int getpagesize(void);
void qsort(void *base, size_t nmemb, size_t size,
           int(*compar)(const void *, const void *));
ssize_t write(int fd, const void *buf, size_t count);
int fallocate(int fd, int mode, off_t offset, off_t len);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);
int fsync(int fd);
int __open64_2(const char *pathname, int flags);
int __xstat64(int vers, const char *name, struct stat64 *buf);
int uname(struct utsname *buf);
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
int __fxstat64(int vers, int fd, struct stat64 *buf);
#endif

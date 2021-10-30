#include "drivers/dbgu.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static byte* heap_top = (byte*) 0x20000000;

void *_sbrk(int incr) {
	byte* tmp = heap_top;
	if (heap_top + incr <= (byte*) (0x23FFFFFF + 1)) {
		heap_top += incr;
		return tmp;
	} else {
		errno = ENOMEM;
		return (void*) -1;
	}
}

int _open(const char *path __attribute__((unused)), int flags __attribute__((unused)), ...) {
    return 1;
}

int _close(int fd __attribute__((unused))) {
    return 0;
}

int _fstat(int fd __attribute__((unused)), struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd) {
    return 1;
}

int _lseek(int fd __attribute__((unused)), off_t pos __attribute__((unused)), int whence __attribute__((unused))) {
    return -1;
}


int _read(int fd __attribute__((unused)), char *buf, size_t cnt __attribute__((unused))) {
    *buf = dbgu_get_byte();
	return 1;
}

int _write(int fd __attribute__((unused)), const char *buf, size_t cnt) {
	return dbgu_write(cnt, buf).io;
}

void _exit(int exitcode __attribute__((unused))) {
	while (1) {
		//asm("wfi");
	}
}

pid_t _getpid(void) {
	return 1;
}

int _kill(pid_t pid, int sig) {
	if (pid == 1) {
		_exit(1);
	} else {
		return -1;
	}
}
#include <ulib.h>
#include <stdio.h>
#include <string.h>
#include <dir.h>
#include <file.h>
#include <stat.h>
#include <dirent.h>
#include <unistd.h>

#define PROG_FILE_NAME "myfile"

#define printf(...)                     fprintf(1, __VA_ARGS__)
#define BUFSIZE                         4096
#define min(a,b)                        (((a)<(b))?(a):(b))

int
getstat(const char *name, struct stat *stat) {
    int fd, ret;
    if ((fd = open(name, O_RDONLY)) < 0) {
        return fd;
    }
    ret = fstat(fd, stat);
    close(fd);
    return ret;
}

int
main(int argc, char **argv) {
    cprintf("Usage: %s dst_file src_file(must exists)\n", argv[0]);
    int fd1 = -1, fd2 = -1;
    if (argc>1)
        fd1 = open(argv[1], O_CREAT|O_RDWR);
    else
        fd1 = open(PROG_FILE_NAME, O_CREAT|O_RDWR);

    if (argc>2)
        fd2 = open(argv[2], O_RDONLY);
//    cprintf("fd1: %d, fd2: %d\n");

//    read(int fd, void *base, size_t len);

    if (argc>2 && fd2 >= 0 && fd1 >= 0) {
        struct stat __stat, *stat = &__stat;
        int ret;
        if ((ret = getstat(argv[2], stat)) != 0) {
            return ret;
        }
        char buf[BUFSIZE+1];
        int remainlen = stat->st_size;
//        printf("src file size: %d bytes\n", remainlen);

        while(0 < remainlen) {
            memset(buf, 0, BUFSIZE+1);
            int copylen = min(BUFSIZE, remainlen);
            read(fd2, buf, copylen);
            write(fd1, buf, copylen);
            remainlen -= copylen;
        }
    }
    close(fd1);
    close(fd2);
    return 0;
}

#include "benchmark.h"

double get_duration(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
}

double seqread_mark(long filesize, const char *target)
{
    void *block = NULL;
    if (posix_memalign(&block, ALIGNMENT, SEQ_BLOCKSIZE) != 0)
    {
        perror("Failed to allocate aligned memory");
        return -1;
    }
    if (!block)
    {
        fprintf(stderr, "posix_memalign returned NULL pointer\n");
        return -1;
    }

    int fd = open(target, O_RDONLY | O_DIRECT);
    if (fd == -1)
    {
        perror("File error");
        free(block);
        return -1;
    }
    struct timespec start, end;

    long bytes_result = 0;
    long bytes_total = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (bytes_total < filesize)
    {
        bytes_result = read(fd, block, SEQ_BLOCKSIZE);
        if (bytes_result == -1)
        {
            perror("Sequential read error");
            close(fd);
            free(block);
            return -1;
        }
        bytes_total += bytes_result;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    close(fd);
    free(block);
    return (double)bytes_total / (get_duration(start, end) * 1024 * 1024);
}

double seqwrite_mark(long filesize, const char *target)
{
    void *block = NULL;
    if (posix_memalign(&block, ALIGNMENT, SEQ_BLOCKSIZE) != 0)
    {
        perror("Failed to allocate aligned memory");
        return -1;
    }
    if (!block)
    {
        fprintf(stderr, "posix_memalign returned NULL pointer\n");
        return -1;
    }

    memset(block, 0, SEQ_BLOCKSIZE);

    int fd = open(target, O_WRONLY | O_DIRECT | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("File error");
        free(block);
        return -1;
    }
    struct timespec start, end;

    long bytes_result = 0;
    long bytes_total = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (bytes_total < filesize)
    {
        bytes_result = write(fd, block, SEQ_BLOCKSIZE);
        if (bytes_result == -1)
        {
            perror("Sequential write error");
            close(fd);
            free(block);
            return -1;
        }
        bytes_total += bytes_result;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    close(fd);
    free(block);
    return (double)bytes_total / (get_duration(start, end) * 1024 * 1024);
}

double rndread_mark(long filesize, const char *target)
{
    void *block = NULL;
    if (posix_memalign(&block, ALIGNMENT, RND_BLOCKSIZE) != 0)
    {
        perror("Failed to allocate aligned memory");
        return -1;
    }
    if (!block)
    {
        fprintf(stderr, "posix_memalign returned NULL pointer\n");
        return -1;
    }

    int fd = open(target, O_RDONLY | O_DIRECT);
    if (fd == -1)
    {
        perror("File error");
        free(block);
        return -1;
    }

    struct timespec start, end;

    long bytes_result = 0;
    long bytes_total = 0;
    long max_blocks = filesize / RND_BLOCKSIZE;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (bytes_total < filesize)
    {
        off_t offset = ((off_t)rand() % max_blocks) * RND_BLOCKSIZE;
        lseek(fd, offset, SEEK_SET);
        bytes_result = read(fd, block, RND_BLOCKSIZE);
        if (bytes_result == -1)
        {
            perror("Random read error");
            close(fd);
            free(block);
            return -1;
        }
        bytes_total += bytes_result;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    close(fd);
    free(block);
    return (double)bytes_total / (get_duration(start, end) * 1024 * 1024);
}

double rndwrite_mark(long filesize, const char *target)
{
    void *block = NULL;
    if (posix_memalign(&block, ALIGNMENT, RND_BLOCKSIZE) != 0)
    {
        perror("Failed to allocate aligned memory");
        return -1;
    }
    if (!block)
    {
        fprintf(stderr, "posix_memalign returned NULL pointer\n");
        return -1;
    }

    memset(block, 0, RND_BLOCKSIZE);

    int fd = open(target, O_TRUNC | O_WRONLY | O_DIRECT);
    if (fd == -1)
    {
        perror("File error");
        free(block);
        return -1;
    }

    struct timespec start, end;

    long bytes_result = 0;
    long bytes_total = 0;
    long max_blocks = filesize / RND_BLOCKSIZE;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (bytes_total < filesize)
    {
        off_t offset = ((off_t)rand() % max_blocks) * RND_BLOCKSIZE;
        lseek(fd, offset, SEEK_SET);
        bytes_result = write(fd, block, RND_BLOCKSIZE);
        if (bytes_result == -1)
        {
            perror("Random write error");
            close(fd);
            free(block);
            return -1;
        }
        bytes_total += bytes_result;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    close(fd);
    free(block);
    return (double)bytes_total / (get_duration(start, end) * 1024 * 1024);
}

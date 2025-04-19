#ifndef BENCHMARK_H
#define BENCHMARK_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define RND_BLOCKSIZE 4096
#define SEQ_BLOCKSIZE 1048576
#define ALIGNMENT 512

double get_duration(struct timespec start, struct timespec end);

double seqread_mark(long filesize, const char *target);
double seqwrite_mark(long filesize, const char *target);
double rndread_mark(long filesize, const char *target);
double rndwrite_mark(long filesize, const char *target);

#endif
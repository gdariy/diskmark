#ifndef FINDMOUNTS_H
#define FINDMOUNTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define MAX_PATH_LEN 256

typedef struct
{
    char device[MAX_PATH_LEN];
    char mountPoint[MAX_PATH_LEN];
    char options[MAX_PATH_LEN];
    unsigned long long totalBytes;
    unsigned long long availBytes;
    int isReadOnly;
    int isValid;
    int isReady;
} MountInfo;

int parse_mount_line(const char *line, MountInfo *info);

int get_mounts(MountInfo **volumes_array);
#endif
#include "findmounts.h"

int parse_mount_line(const char *line, MountInfo *info)
{
    memset(info, 0, sizeof(MountInfo));
    info->isValid = 0;

    int result = sscanf(line, "%s %s %*s %s %*s %*s",
                        info->device, info->mountPoint, info->options);

    if (result == 3)
    {
        if (strstr(info->device, "/dev/"))
            info->isValid = 1;

        if (strstr(info->options, "ro") != NULL && strstr(info->options, "rw") == NULL)
        {
            info->isReadOnly = 1;
        }
        else
        {
            info->isReadOnly = 0;
        }
    }
    return info->isValid;
}

int get_mounts(MountInfo **mounts_array)
{
    FILE *fp;
    char *home_path;
    char line[MAX_LINE_LENGTH];
    MountInfo temp_info;
    struct statvfs vfs_stat;

    int count = 0;
    *mounts_array = NULL;

    home_path = getenv("HOME");
    
    if (home_path)
    {
            strncpy(temp_info.device, "home", MAX_PATH_LEN);
            strncpy(temp_info.mountPoint, home_path, MAX_PATH_LEN);
            temp_info.isValid = 1;
            temp_info.isReadOnly = 0;
            
            if (statvfs(temp_info.mountPoint, &vfs_stat) == 0)
            {
                temp_info.isReady = 1;
                temp_info.totalBytes = vfs_stat.f_blocks * vfs_stat.f_bsize;
                temp_info.availBytes = vfs_stat.f_bavail * vfs_stat.f_bsize;

                count++;
                *mounts_array = malloc(sizeof(MountInfo));
                if (!(*mounts_array))
                {
                    perror("Allocation error in get_mounts functions");
                    return 0;
                }
                memcpy(&(*mounts_array)[count - 1], &temp_info, sizeof(MountInfo));
            }
            else
            {
                fprintf(stderr, "Warning: statvfs failed for %s\n", temp_info.mountPoint);
                temp_info.isReady = 0;
            }
    }

    fp = fopen("/proc/mounts", "r");
    if (fp == NULL)
    {
        perror("Failed to open /proc/mounts");
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (parse_mount_line(line, &temp_info))
        {
            if (statvfs(temp_info.mountPoint, &vfs_stat) == 0)
            {
                temp_info.isReady = 1;
                temp_info.totalBytes = (long long)vfs_stat.f_blocks * vfs_stat.f_bsize;
                temp_info.availBytes = (long long)vfs_stat.f_bavail * vfs_stat.f_bsize;

                if (temp_info.isValid && temp_info.isReady && !temp_info.isReadOnly && !access(temp_info.mountPoint, W_OK))
                {
                    count++;
                    MountInfo *temp = realloc(*mounts_array, count * sizeof(MountInfo));
                    if (!temp)
                    {
                        perror("Allocation error in get_mounts functions");
                        free(*mounts_array);
                        fclose(fp);
                        return 0;
                    }
                    *mounts_array = temp;
                    memcpy(&(*mounts_array)[count - 1], &temp_info, sizeof(MountInfo));
                }
            }
            else
            {
                fprintf(stderr, "Warning: statvfs failed for %s\n", temp_info.mountPoint);
                temp_info.isReady = 0;
            }
        }
    }

    fclose(fp);
    return count;
}